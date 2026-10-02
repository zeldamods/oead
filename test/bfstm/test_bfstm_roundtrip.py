import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("bfstm/files/*.bfstm")

@pytest.mark.parametrize("file", cases)
def test_bfstm_roundtrip(file):
    stm = oead.audio.Fstm(cases_data[file])
    new_stm_data = stm.to_binary()

    assert cases_data[file] == bytes(new_stm_data)