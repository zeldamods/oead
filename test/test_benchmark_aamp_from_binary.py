import aamp
import pytest
import oead
from pathlib import Path

from utils import make_test_cases

cases_bin, data_bin = make_test_cases("aamp/files/**/*.b*")


@pytest.mark.parametrize("file", cases_bin)
def test_aamp_from_bin_aamp(benchmark, file) -> None:
    benchmark.group = "from_bin: " + file
    benchmark(lambda d: aamp.Reader(d).parse(), data_bin[file])


@pytest.mark.parametrize("file", cases_bin)
def test_aamp_from_bin_oead(benchmark, file) -> None:
    benchmark.group = "from_bin: " + file
    benchmark(oead.aamp.ParameterIO.from_binary, data_bin[file])
