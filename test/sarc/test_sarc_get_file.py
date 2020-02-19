import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("sarc/files/*.sarc")


@pytest.mark.parametrize("file", cases)
def test_sarc_get_file(file):
    arc = oead.Sarc(cases_data[file])
    for sarc_file in arc.get_files():
        assert arc.get_file(sarc_file.name) is not None
