#!/usr/bin/env python3
"""Regenerate py/oead-stubs from the installed oead module.

Requires the stubs dependency group (pip install --group stubs).
Pass --check to fail if the committed stubs are out of date.
"""

import argparse
import collections
import difflib
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

STUBS_DIR = Path(__file__).resolve().parent.parent / "py" / "oead-stubs"

# py::implicitly_convertible is invisible in pybind11 signatures, so the builtin types that
# the bindings accept in place of these classes have to be added to parameters by hand.
IMPLICIT_CONVERSIONS = {
    "bytes": ["Bytes"],
    "list": [
        "BufferInt", "BufferF32", "BufferU32", "BufferBool", "BufferString",
        "Array", "FieldArray", "StructArray",
    ],
    "dict": [
        "Hash", "ParameterMap", "ParameterObjectMap", "ParameterListMap", "FileMap", "Struct",
    ],
    "int | str": ["Name"],
}
# Parameter is convertible from any of its value types; the alias is defined from its v setter.
PARAMETER_CONVERSIONS = {"_ParameterValue": ["Parameter"]}

DEF_RE = re.compile(r"^( *def \w+\()(.*)(\)(?: -> .*)?:.*)$", re.MULTILINE)
ATTRIBUTE_RE = re.compile(r"^( +)(\w+): (?!typing\.ClassVar)(.+)$", re.MULTILINE)
CLASS_RE = re.compile(r"^ *class \w+")
GETITEM_RE = re.compile(r"^ +def __getitem__\(self, \w+: (?!slice\b)(.+)\) -> (.+):$")
GET_RE = re.compile(
    r"^( +)def get\(self, key: (.+), default: typing\.Any = None\) -> typing\.Any:$"
)
VIEW_RE = re.compile(r"^( +def (keys|values|items)\(self\) -> typing\.\w+View):$")
VALUE_SETTER_RE = re.compile(r"^ +def v\(self, \w+: (.+)\) -> None:$", re.MULTILINE)


class Rewrites:
    """Counts what each rewrite matched. They are all tied to pybind11-stubgen's output format
    and would otherwise silently stop applying when it changes."""

    def __init__(self) -> None:
        self.counts: collections.Counter[str] = collections.Counter()

    def sub(self, name: str, pattern: str | re.Pattern[str], repl, text: str) -> str:
        text, count = re.subn(pattern, repl, text)
        self.count(name, count)
        return text

    def count(self, name: str, count: int = 1) -> None:
        self.counts[name] += count

    def unapplied(self) -> list[str]:
        return [name for name, count in self.counts.items() if not count]


def add_declaration(text: str, declaration: str) -> str:
    return text.replace("\n__all__", f"\n{declaration}\n__all__", 1)


def widen_type(annotation: str, conversions: dict[str, list[str]]) -> tuple[str, int]:
    count = 0

    def widen(match: re.Match[str]) -> str:
        nonlocal count
        before = match.string[: match.start()]
        # Conversions only apply to the argument itself, not to type arguments or defaults.
        if before.count("[") > before.count("]") or " = " in before.rsplit(", ", 1)[-1]:
            return match.group(0)
        count += 1
        return f"{match.group(0)} | {builtin}"

    for builtin, names in conversions.items():
        pattern = r"(?<![\w.])(?:[A-Za-z]\w*\.)*(?:%s)\b(?!\.)" % "|".join(names)
        annotation = re.sub(pattern, widen, annotation)
    return annotation, count


def widen_parameters(
    text: str, rewrites: Rewrites, name: str, conversions: dict[str, list[str]]
) -> str:
    def widen(match: re.Match[str]) -> str:
        params, count = widen_type(match.group(2), conversions)
        rewrites.count(name, count)
        return match.group(1) + params + match.group(3)

    rewrites.count(name, 0)
    return DEF_RE.sub(widen, text)


def split_widenable_attributes(text: str, rewrites: Rewrites) -> str:
    """Turn attributes that accept more types than they return into properties."""

    def split(match: re.Match[str]) -> str:
        indent, name, annotation = match.groups()
        if not widen_type(annotation, IMPLICIT_CONVERSIONS)[1]:
            return match.group(0)
        rewrites.count("attribute")
        return (
            f"{indent}@property\n"
            f"{indent}def {name}(self) -> {annotation}:\n"
            f"{indent}    ...\n"
            f"{indent}@{name}.setter\n"
            f"{indent}def {name}(self, value: {annotation}) -> None:\n"
            f"{indent}    ..."
        )

    rewrites.count("attribute", 0)
    return ATTRIBUTE_RE.sub(split, text)


def type_map_methods(text: str, rewrites: Rewrites) -> str:
    """Parametrise the views and get() of each map with the types of its __getitem__."""
    lines = []
    item_types = None
    uses_typevar = False
    rewrites.count("map view", 0)
    rewrites.count("map get", 0)
    for line in text.split("\n"):
        if CLASS_RE.match(line):
            item_types = None
        elif match := GETITEM_RE.match(line):
            item_types = match.groups()
        elif item_types and (match := VIEW_RE.match(line)):
            key, value = item_types
            args = {"keys": key, "values": value, "items": f"{key}, {value}"}[match.group(2)]
            line = f"{match.group(1)}[{args}]:"
            rewrites.count("map view")
        elif item_types and (match := GET_RE.match(line)):
            indent, key = match.groups()
            value = item_types[1]
            optional = value if "None" in value.split(" | ") else f"{value} | None"
            line = (
                f"{indent}@typing.overload\n"
                f"{indent}def get(self, key: {key}) -> {optional}:\n"
                f"{indent}    ...\n"
                f"{indent}@typing.overload\n"
                f"{indent}def get(self, key: {key}, default: _T) -> {value} | _T:"
            )
            uses_typevar = True
            rewrites.count("map get")
        lines.append(line)
    text = "\n".join(lines)
    if uses_typevar:
        text = add_declaration(text, '_T = typing.TypeVar("_T")')
    return text


def widen_parameter_values(text: str, rewrites: Rewrites) -> str:
    rewrites.count("Parameter value", 0)
    match = VALUE_SETTER_RE.search(text)
    if not match:
        return text
    (alias,) = PARAMETER_CONVERSIONS
    text = widen_parameters(text, rewrites, "Parameter value", PARAMETER_CONVERSIONS)
    return add_declaration(text, f"{alias}: typing.TypeAlias = {match.group(1)}")


def postprocess(text: str, is_root: bool, rewrites: Rewrites) -> str:
    # pybind11-stubgen drops the map view classes but only rewrites references to them
    # in the root module.
    text = rewrites.sub("view class", r"\boead\.(Keys|Values|Items)View\b", r"typing.\1View", text)
    if is_root:
        # It also qualifies some names in the root module with the package's own name
        # without importing it.
        text = rewrites.sub("root qualification", r"\boead\.(?=[A-Z])", "", text)

    # These two read the types that widening changes, so they have to run before it:
    # a view yields Name, not Name | int | str.
    text = type_map_methods(text, rewrites)
    text = split_widenable_attributes(text, rewrites)
    text = widen_parameters(text, rewrites, "implicit conversion", IMPLICIT_CONVERSIONS)
    text = widen_parameter_values(text, rewrites)

    text = rewrites.sub(
        "__buffer__",
        r"def __buffer__\(self, flags\):",
        "def __buffer__(self, flags: int, /) -> memoryview:",
        text,
    )
    text = rewrites.sub(
        "__release_buffer__",
        r"def __release_buffer__\(self, buffer\):",
        "def __release_buffer__(self, buffer: memoryview, /) -> None:",
        text,
    )
    # collections.abc.Buffer only exists from Python 3.12; type checkers resolve
    # typing_extensions in stubs without it being installed.
    if "collections.abc.Buffer" in text:
        text = rewrites.sub(
            "Buffer", r"\bcollections\.abc\.Buffer\b", "typing_extensions.Buffer", text
        )
        text = text.replace("\nimport typing\n", "\nimport typing\nimport typing_extensions\n", 1)
    return text


def generate(out_dir: Path) -> Path:
    # python -m and -c put the working directory on sys.path; running from the repository
    # root would pick up any in-tree build of the module instead of the installed one.
    module_path = subprocess.run(
        [sys.executable, "-c", "import oead; print(oead.__file__)"],
        check=True,
        cwd=out_dir,
        capture_output=True,
        text=True,
    ).stdout.strip()
    print(f"Generating stubs from {module_path}", file=sys.stderr)
    subprocess.run(
        [sys.executable, "-m", "pybind11_stubgen", "oead", "-o", str(out_dir), "--exit-code"],
        check=True,
        cwd=out_dir,
    )
    generated = out_dir / "oead"

    rewrites = Rewrites()
    for stub in generated.glob("*.pyi"):
        stub.write_text(postprocess(stub.read_text(), stub.name == "__init__.pyi", rewrites))
    if unapplied := rewrites.unapplied():
        sys.exit(
            "These rewrites matched nothing; has the pybind11-stubgen output format changed? "
            + ", ".join(unapplied)
        )
    return generated


def diff(generated: Path, committed: Path) -> list[str]:
    lines: list[str] = []
    roots = [root for root in (generated, committed) if root.is_dir()]
    names = {path.name for root in roots for path in root.iterdir()}
    for name in sorted(names):
        old, new = committed / name, generated / name
        lines += difflib.unified_diff(
            old.read_text().splitlines(keepends=True) if old.is_file() else [],
            new.read_text().splitlines(keepends=True) if new.is_file() else [],
            f"committed/{name}",
            f"generated/{name}",
        )
    return lines


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    with tempfile.TemporaryDirectory() as tmp:
        generated = generate(Path(tmp))
        if args.check:
            if not (lines := diff(generated, STUBS_DIR)):
                return 0
            sys.stderr.writelines(lines)
            print("py/oead-stubs is out of date; run tools/generate_stubs.py", file=sys.stderr)
            return 1

        staged = STUBS_DIR.with_name(STUBS_DIR.name + ".new")
        if staged.exists():
            shutil.rmtree(staged)
        shutil.copytree(generated, staged)
        if STUBS_DIR.exists():
            shutil.rmtree(STUBS_DIR)
        staged.rename(STUBS_DIR)
    return 0


if __name__ == "__main__":
    sys.exit(main())
