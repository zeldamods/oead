import aamp
import pytest
import oead
from pathlib import Path

from utils import make_test_cases_aamp

cases, data = make_test_cases_aamp()


@pytest.mark.parametrize("file", cases)
def test_aamp_from_bin_aamp(benchmark, file):
    benchmark.group = "from_bin: " + file
    benchmark(lambda d: aamp.Reader(d).parse(), data[file])


@pytest.mark.parametrize("file", cases)
def test_aamp_from_bin_oead(benchmark, file):
    benchmark.group = "from_bin: " + file
    benchmark(oead.aamp.ParameterIO.from_binary, data[file])
