import pytest
import oead
from files import TEST_FILES, TEST_FILE_DATA, TEST_FILE_DATA_UNCOMP


@pytest.mark.parametrize("file", TEST_FILES)
def test_decompression(file):
    assert TEST_FILE_DATA_UNCOMP[file] == oead.yaz0.decompress(TEST_FILE_DATA[file])


@pytest.mark.parametrize("file", TEST_FILES)
def test_roundtrip(file):
    assert TEST_FILE_DATA_UNCOMP[file] == oead.yaz0.decompress(oead.yaz0.compress(TEST_FILE_DATA_UNCOMP[file]))

