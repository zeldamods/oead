import pytest
import oead


@pytest.mark.parametrize("data", [b"", b"Yaz0", b"\0" * 16])
def test_decompress_unsafe_checks_header(data):
    with pytest.raises(oead.InvalidDataError):
        oead.yaz0.decompress_unsafe(data)
