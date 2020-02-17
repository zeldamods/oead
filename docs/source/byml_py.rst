#############
BYML (Python)
#############

.. include:: parts/py_common.rst
.. include:: parts/byml_common.rst

Quick usage
===========

* To read a BYML: :func:`oead.byml.from_binary`
* To write a BYML: :func:`oead.byml.to_binary`
* To read a YAML: :func:`oead.byml.from_text`
* To write a YAML: :func:`oead.byml.to_text`

.. code-block:: py

    import oead
    with open("ActorInfo.product.byml", "rb") as f:
       info = oead.Byml.from_binary(f.read())
    # info is a dict-like
    info["Actors"]
    # Array(...)
    info["Actors"][0]
    # Hash({'bugMask': Byml(S32(1)), ..., 'name': Byml('EnemyFortressMgrTag'), ...})
    info["Actors"][0]["name"]
    # 'EnemyFortressMgrTag'

Reference
=========

.. class:: oead.byml.Array

    Lightweight list-like object. Can be cast to a list.

.. class:: oead.byml.Hash

    Lightweight dict-like object. Can be cast to a dict.

.. autofunction:: oead.byml.from_binary

    See also :cpp:type:`oead::Byml::FromBinary`

.. autofunction:: oead.byml.to_binary

    See also :cpp:type:`oead::Byml::ToBinary`

.. autofunction:: oead.byml.from_text

    See also :cpp:type:`oead::Byml::FromText`

.. autofunction:: oead.byml.to_text

    See also :cpp:type:`oead::Byml::ToText`

.. note:: The following getters mirror the behaviour of Nintendo's BYML library. Some of them will perform type conversions automatically. If value types are incorrect, a TypeError exception is thrown.

.. autofunction:: oead.byml.get_bool
.. autofunction:: oead.byml.get_double
.. autofunction:: oead.byml.get_float
.. autofunction:: oead.byml.get_int
.. autofunction:: oead.byml.get_int64
.. autofunction:: oead.byml.get_string
.. autofunction:: oead.byml.get_uint
.. autofunction:: oead.byml.get_uint64
