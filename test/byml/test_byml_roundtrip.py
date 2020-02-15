import pytest
import oead

from utils import make_test_cases

cases_bin, data_bin = make_test_cases("byml/files/*.byml")
cases_text, data_text = make_test_cases("byml/files/*.yml")


@pytest.mark.parametrize("file", cases_bin)
def test_byml_roundtrip_bin(file):
    data = oead.byml.from_binary(data_bin[file])
    serialized = oead.byml.to_binary(data, big_endian=False, version=2)
    data2 = oead.byml.from_binary(serialized)
    assert data == data2


@pytest.mark.parametrize("file", cases_bin)
def test_byml_roundtrip_bin_big_endian(file):
    data = oead.byml.from_binary(data_bin[file])
    serialized = oead.byml.to_binary(data, big_endian=True, version=2)
    data2 = oead.byml.from_binary(serialized)
    assert data == data2


@pytest.mark.parametrize("file", cases_text)
def test_byml_roundtrip_text(file):
    data = oead.byml.from_text(data_text[file])
    serialized = oead.byml.to_text(data)
    data2 = oead.byml.from_text(serialized)
    assert data == data2


@pytest.mark.parametrize("file", cases_bin)
def test_byml_roundtrip_bin_to_text(file):
    data = oead.byml.from_binary(data_bin[file])
    serialized = oead.byml.to_text(data)
    data2 = oead.byml.from_text(serialized)
    assert data == data2


@pytest.mark.parametrize("file", cases_text)
def test_byml_roundtrip_text_to_bin(file):
    data = oead.byml.from_text(data_text[file])
    serialized = oead.byml.to_binary(data, big_endian=False, version=2)
    data2 = oead.byml.from_binary(serialized)
    assert data == data2
