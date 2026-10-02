import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("bars/files/*.bars")

@pytest.mark.parametrize("file", cases)
def test_bars_roundtrip(file):
    res = oead.audio.Bars(cases_data[file])
    new_res_data = res.to_binary()

    assert cases_data[file] == bytes(new_res_data)