import io
import pytest
import byml
import byml.yaml_util
import oead
import yaml

from utils import make_test_cases

cases, data = make_test_cases("byml/files/*.byml")

byml.yaml_util.add_representers(yaml.CDumper)


def byml_to_text(data):
    return yaml.dump(data, Dumper=yaml.CDumper, allow_unicode=True)


def oead_to_text(instance):
    return oead.byml.to_text(instance)


@pytest.mark.parametrize("file", cases)
def test_to_text_byml(benchmark, file):
    benchmark.group = "to text: " + file
    x = byml.Byml(data[file]).parse()
    benchmark(byml_to_text, x)


@pytest.mark.parametrize("file", cases)
def test_to_text_oead(benchmark, file):
    benchmark.group = "to text: " + file
    instance = oead.byml.from_binary(data[file])
    benchmark(oead_to_text, instance)
