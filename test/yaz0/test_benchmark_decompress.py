import pytest
import libyaz0
import oead
import wszst_yaz0
from files import TEST_FILES, TEST_FILE_DATA


@pytest.mark.parametrize("file", TEST_FILES)
def test_wszst_yaz0(benchmark, file):
    benchmark.group = "decomp: " + file
    wszst_yaz0._use_c_module = False
    benchmark(wszst_yaz0.decompress, TEST_FILE_DATA[file])


@pytest.mark.parametrize("file", TEST_FILES)
def test_wszst_yaz0_c(benchmark, file):
    benchmark.group = "decomp: " + file
    wszst_yaz0._use_c_module = True
    benchmark(wszst_yaz0.decompress, TEST_FILE_DATA[file])


@pytest.mark.parametrize("file", TEST_FILES)
def test_syaz0(benchmark, file):
    benchmark.group = "decomp: " + file
    benchmark(oead.yaz0.decompress, TEST_FILE_DATA[file])


@pytest.mark.parametrize("file", TEST_FILES)
def test_syaz0_fast(benchmark, file):
    benchmark.group = "decomp: " + file
    benchmark(oead.yaz0.decompress_unsafe, TEST_FILE_DATA[file])


@pytest.mark.parametrize("file", TEST_FILES)
def test_libyaz0(benchmark, file):
    benchmark.group = "decomp: " + file
    benchmark(libyaz0.decompress, TEST_FILE_DATA[file])
