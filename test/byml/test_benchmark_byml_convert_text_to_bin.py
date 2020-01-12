import io
import pytest
import byml.yaml_util
import oead
import yaml

from utils import make_test_cases

cases, data = make_test_cases("byml/files/*.yml")

byml.yaml_util.add_constructors(yaml.CLoader)


def byml_convert(data):
    b = yaml.load(data, Loader=yaml.CLoader)
    stream = io.BytesIO()
    return byml.Writer(b, be=False, version=2).write(stream)

def oead_convert(data):
    b = oead.Byml.from_text(data)
    return b.to_binary(big_endian=False, version=2)

@pytest.mark.parametrize("file", cases)
def test_convert_byml(benchmark, file):
    benchmark.group = "convert_text_to_bin: " + file
    benchmark(byml_convert, data[file])


@pytest.mark.parametrize("file", cases)
def test_convert_oead(benchmark, file):
    benchmark.group = "convert_text_to_bin: " + file
    benchmark(oead_convert, data[file])
