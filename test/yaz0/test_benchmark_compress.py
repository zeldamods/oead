import pytest
import libyaz0
import oead
import wszst_yaz0
from files import TEST_FILES, TEST_FILE_DATA, TEST_FILE_DATA_UNCOMP


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_libyaz0_lv1(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(libyaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=1)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_libyaz0_lv6(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(libyaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=6)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_libyaz0_lv9(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(libyaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=9)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_syaz0_lv6(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(oead.yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=6)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_syaz0_lv7(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(oead.yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=7)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_syaz0_lv8(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(oead.yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=8)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_syaz0_lv9(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(oead.yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=9)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_wszst_yaz0_lv1(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(wszst_yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=1)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_wszst_yaz0_lv6(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(wszst_yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=6)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_wszst_yaz0_lv9(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(wszst_yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=9)


@pytest.mark.parametrize("file", TEST_FILES)
def test_comp_wszst_yaz0_lv10(benchmark, file):
    benchmark.group = "comp: " + file
    benchmark(wszst_yaz0.compress, TEST_FILE_DATA_UNCOMP[file], level=10)
