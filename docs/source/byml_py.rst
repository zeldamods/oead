#############
BYML (Python)
#############

.. include:: _byml_status.rst

Quick usage
===========

* To read a BYML: :func:`oead.Byml.from_binary`
* To write a BYML: :func:`oead.Byml.to_binary`
* To read a YAML: :func:`oead.Byml.from_text`
* To write a YAML: :func:`oead.Byml.to_text`

.. code-block:: py

    import oead
    with open("ActorInfo.product.byml", "rb") as f:
       info = oead.Byml.from_binary(f.read())
    info
    # Byml(...)
    info.type()
    # Type.Hash
    info.v["Actors"].type()
    # Type.Array
    info.v["Actors"].v[0]
    # Byml(Hash({'bugMask': Byml(S32(1)), ..., 'name': Byml('EnemyFortressMgrTag'), ...}))
    info.v["Actors"].v[0].v
    # Hash({'bugMask': Byml(S32(1)), ..., 'name': Byml('EnemyFortressMgrTag'), ...})
    info.v["Actors"].v[0].v["name"]
    # Byml('EnemyFortressMgrTag')
    info.v["Actors"].v[0].v["name"].v
    # 'EnemyFortressMgrTag'

    info.v["Actors"].v[0].v["name"] = 'test'
    info.v["Actors"].v[0].v["name"]
    # Byml('test')

Reference
=========

.. autoclass:: oead.Byml

    See also :cpp:type:`oead::Byml`

.. note:: The typed getters mirror the behaviour of Nintendo's BYML library. Some of them will perform type conversions automatically. If value types are incorrect, a TypeError exception is thrown.
