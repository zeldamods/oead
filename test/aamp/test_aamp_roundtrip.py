import pytest
import oead
from pathlib import Path
import warnings

from utils import make_test_cases

cases_bin, data_bin = make_test_cases("aamp/files/**/*.b*")


@pytest.mark.parametrize("file", cases_bin)
def test_aamp_roundtrip_bin_semantic_equality(file):
    data = oead.aamp.ParameterIO.from_binary(data_bin[file])
    serialized = data.to_binary()
    data2 = oead.aamp.ParameterIO.from_binary(serialized)
    assert data == data2, "mismatched content"


@pytest.mark.parametrize("file", cases_bin)
def test_aamp_roundtrip_bin_binary_equality(file):
    data = oead.aamp.ParameterIO.from_binary(data_bin[file])
    serialized = bytes(data.to_binary())
    assert len(data_bin[file]) == len(serialized)
    if data_bin[file] != serialized:
        warnings.warn(UserWarning("mismatched serialized data for %s" % file))
    # assert data_bin[file] == serialized, "mismatched serialized data"


@pytest.mark.parametrize("file", cases_bin)
def test_aamp_roundtrip_bin_to_text(file):
    data = oead.aamp.ParameterIO.from_binary(data_bin[file])
    serialized = data.to_text()
    data2 = oead.aamp.ParameterIO.from_text(serialized)
    assert data == data2


def test_aamp_roundtrip_types():
    with (Path(__file__).parent / "test.yml").open("r") as test_yml:
        yml_data = test_yml.read()
    data: oead.aamp.ParameterIO = oead.aamp.ParameterIO.from_text(yml_data)
    assert data.type == "oead_test"
    assert data.version == 10

    text_serialized = data.to_text()
    assert data == oead.aamp.ParameterIO.from_text(text_serialized)

    bin_serialized = data.to_binary()
    assert data == oead.aamp.ParameterIO.from_binary(bin_serialized)
