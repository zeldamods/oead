import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("bars/files/*.bars")

@pytest.mark.parametrize("file", cases)
def test_bars_byteswap(file):
    res = oead.audio.Bars(cases_data[file])

    data_le = res.to_binary(oead.Endianness.Little)
    data_be = res.to_binary(oead.Endianness.Big)

    res_le = oead.audio.Bars(data_le)
    res_be = oead.audio.Bars(data_be)

    data_le = res_le.to_binary(oead.Endianness.Little)
    data_be = res_be.to_binary(oead.Endianness.Big)

    original_bytes = res.to_binary()

    assert bytes(data_le) == bytes(original_bytes) or bytes(data_be) == bytes(original_bytes)
    

    