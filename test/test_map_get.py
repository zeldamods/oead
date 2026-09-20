import gc

import pytest
import oead


@pytest.fixture
def byml_hash():
    return oead.byml.from_text("{s: text, i: 1, f: 1.5, b: true, n: null, h: {x: 1}, a: [1]}")


@pytest.mark.parametrize("key", ["s", "i", "f", "b", "n", "h", "a"])
def test_get_matches_getitem(byml_hash, key):
    assert byml_hash.get(key) == byml_hash[key]


@pytest.mark.parametrize("default", [None, 5, "text", [], (), 1.5, oead.S32(5)])
def test_get_missing_returns_default(byml_hash, default):
    assert byml_hash.get("missing", default) is default


def test_get_missing_without_default_returns_none(byml_hash):
    assert byml_hash.get("missing") is None


def test_get_returns_reference(byml_hash):
    byml_hash.get("h")["y"] = "new"
    assert byml_hash["h"]["y"] == "new"


def test_get_result_outlives_map():
    inner = oead.byml.from_text("{h: {x: 1}}").get("h")
    gc.collect()
    assert inner["x"] == oead.S32(1)


def test_get_ordered_map():
    params = oead.aamp.ParameterMap({"k": oead.aamp.Parameter(True)})
    assert params.get("k") == params["k"]
    assert params.get("missing") is None
    assert params.get("missing", 5) == 5
    assert params.get(12345) is None


def test_get_sarc_file_map():
    writer = oead.SarcWriter()
    writer.files["a"] = b"123"
    assert bytes(writer.files.get("a")) == b"123"
    assert writer.files.get("missing", b"") == b""


def test_wrong_self_raises_type_error():
    with pytest.raises(TypeError):
        oead.byml.Hash.keys(5)
    with pytest.raises(TypeError):
        oead.aamp.ParameterMap.keys(5)
