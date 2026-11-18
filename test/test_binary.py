import pytest
import oead

def test_zero_aligned_binary():
    with pytest.raises(oead.InvalidDataError):
        oead.byml.to_binary(oead.byml.AlignedBinary(b"", oead.U32(0)), False, 10)