import pytest
import oead

from utils import make_test_cases

cases_bin, data_bin = make_test_cases("gsheet/files/*.gsheet")


@pytest.mark.parametrize("file", cases_bin)
def test_gsheet_roundtrip_bin(file):
    sheet = oead.gsheet.parse(data_bin[file])
    serialized = sheet.to_binary()
    assert data_bin[file] == serialized
