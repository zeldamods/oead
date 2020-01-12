from pathlib import Path
import pytest


def make_test_cases(pattern):
    array_cases = []
    array_data = dict()
    for path in (Path(__file__).parent.parent).glob(pattern):
        with path.open("rb") as f:
            array_cases.append(pytest.param(path.name, id='/'))
            array_data[path.name] = f.read()
    return (array_cases, array_data)


def make_test_cases_from_file_list(files):
    array_cases = []
    array_data = dict()
    parent_dir = Path(__file__).parent.parent
    for path in ((parent_dir / file) for file in files):
        with path.open("rb") as f:
            array_cases.append(pytest.param(path.name, id='/'))
            array_data[path.name] = f.read()
    return (array_cases, array_data)

def make_test_cases_aamp():
    return make_test_cases_from_file_list([
        Path("aamp") / "files" / "normal.bwinfo",
        Path("aamp") / "files" / "MusicList.bmscdef",
        Path("aamp") / "files" / "Demo010_0.bdemo",
        Path("aamp") / "files" / "TBox_Field_Stone.bxml",
        Path("aamp") / "files" / "ASList" / "Npc_Rito_Musician.baslist",
        Path("aamp") / "files" / "ASList" / "UK_OldKorogu.baslist",
        Path("aamp") / "files" / "AIProgram" / "Lizalfos_Ice.baiprog",
        Path("aamp") / "files" / "AIProgram" / "Player_Link.baiprog",
        Path("aamp") / "files" / "AIProgram" / "Horse.baiprog",
    ])
