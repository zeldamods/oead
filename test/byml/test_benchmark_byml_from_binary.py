import pytest
import byml
import oead

from utils import make_test_cases

cases, data = make_test_cases("byml/files/*.byml")


def byml_parse(data):
    return byml.Byml(data).parse()


@pytest.mark.parametrize("file", cases)
def test_parse_byml(benchmark, file):
    benchmark.group = "parse: " + file
    benchmark(byml_parse, data[file])


@pytest.mark.parametrize("file", cases)
def test_parse_oead(benchmark, file):
    benchmark.group = "parse: " + file
    benchmark(oead.Byml.from_binary, data[file])
