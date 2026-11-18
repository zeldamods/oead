import oead

def test_null_root():
    data = None
    assert oead.byml.from_binary(oead.byml.to_binary(data, False, 10)) == data
    assert oead.byml.from_text(oead.byml.to_text(data)) == data


def test_u32_root():
    data = oead.U32(10)
    assert oead.byml.from_binary(oead.byml.to_binary(data, False, 10)) == data
    assert oead.byml.from_text(oead.byml.to_text(data)) == data


def test_f32_root():
    data = oead.F32(10.0)
    assert oead.byml.from_binary(oead.byml.to_binary(data, False, 10)) == data
    assert oead.byml.from_text(oead.byml.to_text(data)) == data


def test_u64_root():
    data = oead.U64(10)
    assert oead.byml.from_binary(oead.byml.to_binary(data, False, 10)) == data
    assert oead.byml.from_text(oead.byml.to_text(data)) == data


def test_f64_root():
    data = oead.F64(10.0)
    assert oead.byml.from_binary(oead.byml.to_binary(data, False, 10)) == data
    assert oead.byml.from_text(oead.byml.to_text(data)) == data


def test_aligned_binary_root():
    data = oead.byml.AlignedBinary(b"Hello, World\n!", oead.U32(32))
    assert oead.byml.from_binary(oead.byml.to_binary(data, False, 10)) == data
    assert oead.byml.from_text(oead.byml.to_text(data)) == data