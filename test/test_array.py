import pytest
import oead

def test_mono_typed_array():
    array = oead.byml.MonoTypedArray(oead.byml.Type.Float)
    array.elements.append(oead.F32(1.0))
    array.elements.append(oead.F32(2.0))
    array.elements.append(oead.F32(3.0))
    array.elements.append(oead.F32(4.0))
    array.elements.append(oead.F32(5.0))
    serialized = oead.byml.from_binary(oead.byml.to_binary(array, False, 10))
    assert oead.byml.to_text(array) == oead.byml.to_text(serialized)

def test_mismatching_element_raises_type_error():
    with pytest.raises(oead.TypeError):
        array = oead.byml.MonoTypedArray(oead.byml.Type.UInt)
        array.elements.append(oead.F32(1.0))
        oead.byml.to_text(array)
    with pytest.raises(oead.TypeError):
        array = oead.byml.MonoTypedArray(oead.byml.Type.Hash64)
        array.elements.append(oead.byml.Hash())
        oead.byml.to_binary(array, False, 10)
    with pytest.raises(oead.TypeError):
        oead.byml.from_text("!typed_array { Type: Int, Elements: [1.0] }")