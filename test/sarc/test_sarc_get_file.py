import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("sarc/files/*.sarc")


@pytest.mark.parametrize("file", cases)
def test_sarc_get_file(file):
    arc = oead.Sarc(cases_data[file])
    for sarc_file in arc.get_files():
        assert arc.get_file(sarc_file.name) is not None


@pytest.mark.parametrize("file", cases)
def test_sarc_get_file_by_index(file):
    arc = oead.Sarc(cases_data[file])
    for i, sarc_file in enumerate(arc.get_files()):
        by_index = arc.get_file(i)
        assert by_index.name == sarc_file.name
        assert bytes(by_index.data) == bytes(sarc_file.data)


@pytest.mark.parametrize("file", cases)
def test_sarc_get_file_bad_argument(file):
    arc = oead.Sarc(cases_data[file])
    with pytest.raises(TypeError):
        arc.get_file(0.5)
