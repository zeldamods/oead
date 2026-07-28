import pytest
import byml.yaml_util
import oead
import yaml

from utils import make_test_cases

cases, data = make_test_cases("byml/files/*.yml")

byml.yaml_util.add_constructors(yaml.CLoader)

def byml_parse(data):
    return yaml.load(data, Loader=yaml.CLoader)


@pytest.mark.parametrize("file", cases)
def test_parse_byml(benchmark, file):
    benchmark.group = "from_text: " + file
    benchmark(byml_parse, data[file])


@pytest.mark.parametrize("file", cases)
def test_parse_oead(benchmark, file):
    benchmark.group = "from_text: " + file
    benchmark(oead.byml.from_text, data[file])
