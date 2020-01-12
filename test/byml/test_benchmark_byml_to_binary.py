import io
import pytest
import byml
import oead

from utils import make_test_cases

cases, data = make_test_cases("byml/files/*.byml")


def byml_to_bin(data):
    stream = io.BytesIO()
    return byml.Writer(data, be=False, version=2).write(stream)


def oead_to_bin(instance):
    return instance.to_binary(big_endian=False, version=2)


@pytest.mark.parametrize("file", cases)
def test_to_bin_byml(benchmark, file):
    benchmark.group = "to bin: " + file
    x = byml.Byml(data[file]).parse()
    benchmark(byml_to_bin, x)


@pytest.mark.parametrize("file", cases)
def test_to_bin_oead(benchmark, file):
    benchmark.group = "to bin: " + file
    instance = oead.Byml.from_binary(data[file])
    benchmark(oead_to_bin, instance)
