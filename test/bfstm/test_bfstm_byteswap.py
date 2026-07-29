import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("bfstm/files/*.bfstm")

@pytest.mark.parametrize("file", cases)
def test_bfstm_byteswap(file):
    stm = oead.audio.Fstm(cases_data[file])
    
    stm.endian = oead.Endianness.Little
    data_le = stm.to_binary() 

    stm.endian = oead.Endianness.Big
    data_be = stm.to_binary()

    stm_le = oead.audio.Fstm(data_le)
    stm_be = oead.audio.Fstm(data_be)

    data_le = stm_le.to_binary()
    data_be = stm_be.to_binary()

    original_bytes = stm.to_binary()

    assert bytes(data_le) == bytes(original_bytes) or bytes(data_be) == bytes(original_bytes)
    

    