import oead

def test_dict_becomes_hash():
    assert isinstance(oead.byml.from_text(oead.byml.to_text({})), oead.byml.Hash)


def test_list_becomes_bytes():
    assert isinstance(oead.byml.from_text(oead.byml.to_text([])), oead.Bytes)

