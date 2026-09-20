import gc

import pytest
import oead


def make_sarc() -> bytes:
    writer = oead.SarcWriter()
    for i in range(3):
        writer.files[f"file{i}.bin"] = bytes([0x41 + i]) * 5000
    return bytes(writer.write()[1])


def is_pinned(data: bytearray) -> bool:
    try:
        data.extend(b"\0")
    except BufferError:
        return True
    return False


def test_sarc_pins_its_buffer():
    data = bytearray(make_sarc())
    arc = oead.Sarc(data)
    assert is_pinned(data)
    del arc
    gc.collect()
    assert not is_pinned(data)


@pytest.mark.parametrize("get_file", [
    lambda arc: arc.get_file("file0.bin"),
    lambda arc: arc.get_file(0),
    lambda arc: next(arc.get_files()),
])
def test_file_keeps_archive_alive(get_file):
    data = bytearray(make_sarc())
    file = get_file(oead.Sarc(data))
    gc.collect()
    assert is_pinned(data)
    assert file.name == "file0.bin"
    del file
    gc.collect()
    assert not is_pinned(data)


def test_file_data_keeps_archive_alive():
    data = bytearray(make_sarc())
    view = oead.Sarc(data).get_file("file0.bin").data
    gc.collect()
    assert is_pinned(data)
    assert bytes(view) == b"A" * 5000
    assert view.readonly
    del view
    gc.collect()
    assert not is_pinned(data)


def test_non_contiguous_buffer_is_rejected():
    with pytest.raises(TypeError):
        oead.yaz0.compress(memoryview(bytes(100))[::2])
    with pytest.raises(TypeError):
        oead.Sarc(memoryview(make_sarc())[::2])


def test_non_buffer_is_rejected():
    with pytest.raises(TypeError):
        oead.yaz0.decompress([1, 2, 3])
