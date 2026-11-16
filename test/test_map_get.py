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


def test_ordered_hash32_serialization():
    hash32 = oead.byml.OrderedHash32({
        1 : oead.U32(1),
        9 : oead.U32(9),
        2 : oead.U32(2),
        8 : oead.U32(8),
        3 : oead.U32(3),
        7 : oead.U32(7),
        4 : oead.U32(4),
        6 : oead.U32(6),
        5 : oead.U32(5),
    })
    serialized = oead.byml.from_binary(oead.byml.to_binary(hash32, False, 10))
    assert oead.byml.to_text(hash32) == oead.byml.to_text(serialized)
    assert all(k1 == k2 for k1, k2 in zip(serialized.keys(), hash32.keys()))
    assert all(serialized[key] == hash32[key] for key in serialized.keys())
    items = list(serialized.items())
    assert items[0][1] == hash32[1]
    assert items[1][1] == hash32[9]
    assert items[2][1] == hash32[2]
    assert items[3][1] == hash32[8]
    assert items[4][1] == hash32[3]
    assert items[5][1] == hash32[7]
    assert items[6][1] == hash32[4]
    assert items[7][1] == hash32[6]
    assert items[8][1] == hash32[5]


def test_ordered_hash64_serialization():
    hash64 = oead.byml.OrderedHash64({
        1 : oead.U32(1),
        9 : oead.U32(9),
        2 : oead.U32(2),
        8 : oead.U32(8),
        3 : oead.U32(3),
        7 : oead.U32(7),
        4 : oead.U32(4),
        6 : oead.U32(6),
        5 : oead.U32(5),
    })
    serialized = oead.byml.from_binary(oead.byml.to_binary(hash64, False, 10))
    assert all(k1 == k2 for k1, k2 in zip(serialized.keys(), hash64.keys()))
    assert all(serialized[key] == hash64[key] for key in serialized.keys())
    items = list(serialized.items())
    assert items[0][1] == hash64[1]
    assert items[1][1] == hash64[9]
    assert items[2][1] == hash64[2]
    assert items[3][1] == hash64[8]
    assert items[4][1] == hash64[3]
    assert items[5][1] == hash64[7]
    assert items[6][1] == hash64[4]
    assert items[7][1] == hash64[6]
    assert items[8][1] == hash64[5]


def test_ordered_hash_serialization():
    ordered = oead.byml.OrderedHash({
        "1" : oead.U32(1),
        "9" : oead.U32(9),
        "2" : oead.U32(2),
        "8" : oead.U32(8),
        "3" : oead.U32(3),
        "7" : oead.U32(7),
        "4" : oead.U32(4),
        "6" : oead.U32(6),
        "5" : oead.U32(5),
    })
    serialized = oead.byml.from_binary(oead.byml.to_binary(ordered, False, 10))
    assert all(k1 == k2 for k1, k2 in zip(serialized.keys(), ordered.keys()))
    assert all(serialized[key] == ordered[key] for key in serialized.keys())
    items = list(serialized.items())
    assert items[0][1] == ordered["1"]
    assert items[1][1] == ordered["9"]
    assert items[2][1] == ordered["2"]
    assert items[3][1] == ordered["8"]
    assert items[4][1] == ordered["3"]
    assert items[5][1] == ordered["7"]
    assert items[6][1] == ordered["4"]
    assert items[7][1] == ordered["6"]
    assert items[8][1] == ordered["5"]