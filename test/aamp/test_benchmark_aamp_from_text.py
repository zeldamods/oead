import aamp
import aamp.yaml_util
import pytest
import oead
from pathlib import Path
import yaml

from utils import make_test_cases_aamp

cases, data = make_test_cases_aamp()


def aamp_to_text(data):
    return oead.aamp.ParameterIO.from_binary(data).to_text()


text_data = {k: aamp_to_text(v) for k, v in data.items()}


@pytest.mark.parametrize("file", cases)
def test_aamp_from_text_aamp(benchmark, file):
    aamp.yaml_util.register_constructors(loader=yaml.CLoader)
    benchmark.group = "from_text: " + file
    benchmark(lambda d: yaml.load(d, Loader=yaml.CLoader), text_data[file])


@pytest.mark.parametrize("file", cases)
def test_aamp_from_text_oead(benchmark, file):
    benchmark.group = "from_text: " + file
    benchmark(oead.aamp.ParameterIO.from_text, text_data[file])
