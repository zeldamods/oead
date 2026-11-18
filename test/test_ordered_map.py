import oead

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
    for serialized in [
        oead.byml.from_binary(oead.byml.to_binary(hash32, False, 10)),
        oead.byml.from_text(oead.byml.to_text(hash32))
    ]:
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
    for serialized in [
        oead.byml.from_binary(oead.byml.to_binary(hash64, False, 10)),
        oead.byml.from_text(oead.byml.to_text(hash64))
    ]:
        assert oead.byml.to_text(hash64) == oead.byml.to_text(serialized)
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
    for serialized in [
        oead.byml.from_binary(oead.byml.to_binary(ordered, False, 10)),
        oead.byml.from_text(oead.byml.to_text(ordered))
    ]:
        assert oead.byml.to_text(ordered) == oead.byml.to_text(serialized)
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

def test_nested_ordered_hash32():
    hash32 = oead.byml.OrderedHash32({
        1 : oead.byml.OrderedHash32({
            1 : oead.U32(1)
        }),
        2 : oead.byml.OrderedHash32({
            2 : oead.U32(2)
        }),
    })
    serialized = oead.byml.from_binary(oead.byml.to_binary(hash32, False, 10))
    assert all(k1 == k2 for k1, k2 in zip(serialized.keys(), hash32.keys()))
    assert all(serialized[key] == hash32[key] for key in serialized.keys())

def test_nested_ordered_hash64():
    hash64 = oead.byml.OrderedHash64({
        1 : oead.byml.OrderedHash64({
            1 : oead.U32(1)
        }),
        2 : oead.byml.OrderedHash64({
            2 : oead.U32(2)
        }),
    })
    serialized = oead.byml.from_binary(oead.byml.to_binary(hash64, False, 10))
    assert all(k1 == k2 for k1, k2 in zip(serialized.keys(), hash64.keys()))
    assert all(serialized[key] == hash64[key] for key in serialized.keys())

def test_nested_ordered_hash():
    ordered = oead.byml.OrderedHash({
        "1" : oead.byml.OrderedHash({
            "1" : oead.U32(1)
        }),
        "2" : oead.byml.OrderedHash({
            "2" : oead.U32(2)
        }),
    })
    serialized = oead.byml.from_binary(oead.byml.to_binary(ordered, False, 10))
    assert all(k1 == k2 for k1, k2 in zip(serialized.keys(), ordered.keys()))
    assert all(serialized[key] == ordered[key] for key in serialized.keys())

def test_nested_ordered_hash():
    ordered = oead.byml.OrderedHash({
        "1" : oead.byml.OrderedHash({
            "1" : oead.U32(1)
        }),
        "2" : oead.byml.OrderedHash({
            "2" : oead.U32(2)
        }),
    })
    serialized = oead.byml.from_binary(oead.byml.to_binary(ordered, False, 10))
    assert all(k1 == k2 for k1, k2 in zip(serialized.keys(), ordered.keys()))
    assert all(serialized[key] == ordered[key] for key in serialized.keys())

def test_ordered_hash_dedupe():
    expected = bytes([
        0x59, 0x42, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
        0xc0, 0x02, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
        0x30, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xd3, 0x00, 0x00, 0x00,
        0x00,
    ])
    hash32 = oead.byml.Array([oead.byml.OrderedHash32({1:oead.U32(1)}), oead.byml.OrderedHash32({1:oead.U32(1)})])
    assert oead.byml.to_binary(hash32, False, 10) == expected