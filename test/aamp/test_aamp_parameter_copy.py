import copy

import oead


def test_copy_returns_parameter():
    param = oead.aamp.Parameter(oead.Vector3f())
    copied = copy.copy(param)
    assert isinstance(copied, oead.aamp.Parameter)
    assert copied == param


def test_deepcopy_is_independent():
    param = oead.aamp.Parameter(oead.Vector3f())
    copied = copy.deepcopy(param)
    copied.v.x = 5.0
    assert param.v.x == 0.0
    assert copied.v.x == 5.0
