#############
AAMP (Python)
#############

.. include:: _aamp_status.rst

Quick usage
===========

* To read a binary parameter archive: :func:`oead.aamp.ParameterIO.from_binary`
* To write a binary parameter archive: :func:`oead.aamp.ParameterIO.to_binary`
* To read a YAML text parameter archive: :func:`oead.aamp.ParameterIO.from_text`
* To write a YAML text parameter archive: :func:`oead.aamp.ParameterIO.to_text`

.. code-block:: py

    import oead
    with open("Horse.baiprog", "rb") as f:
       pio = oead.aamp.ParameterIO.from_binary(f.read())
    pio.version
    # 0
    pio.type
    # "xml"
    pio.objects["DemoAIActionIdx"].params["Demo_Wait"]
    # Parameter(107)
    pio.objects["DemoAIActionIdx"].params["Demo_Wait"].v
    # 107

Parameters
==========

.. autoclass:: oead.aamp.Parameter

    See also :cpp:type:`oead::aamp::Parameter`

.. autoclass:: oead.aamp.Name

    See also :cpp:type:`oead::aamp::Name`

.. autoclass:: oead.aamp.ParameterMap

    Lightweight dict-like object wrapping a :cpp:type:`oead::aamp::ParameterMap`.

.. autoclass:: oead.aamp.ParameterObjectMap

    Lightweight dict-like object wrapping a :cpp:type:`oead::aamp::ParameterObjectMap`.

.. autoclass:: oead.aamp.ParameterListMap

    Lightweight dict-like object wrapping a :cpp:type:`oead::aamp::ParameterListMap`.

.. note:: Just like the C++ :cpp:struct:`oead::aamp::Name`, :class:`oead.aamp.Name` can be constructed from either a hash or a string, so it is possible to access elements in a map by name rather than by hash. Both ``object.params[crc32_hash]`` and ``object.params["NameString"]`` will work.

.. autoclass:: oead.aamp.ParameterList

    See also :cpp:type:`oead::aamp::ParameterList`

.. autoclass:: oead.aamp.ParameterObject

    See also :cpp:type:`oead::aamp::ParameterObject`

Parameter IO
============

.. autoclass:: oead.aamp.ParameterIO

    See also :cpp:type:`oead::aamp::ParameterIO`

Name utilities
==============

Because binary parameter archives only store CRC32 hashes of structure names, recovering the original names -- which is useful for converting archives to a human-readable format -- requires the use of a name table.

When serializing to YAML, by default oead will use a table that contains strings from *Breath of the Wild*'s executable.

.. autoclass:: oead.aamp.NameTable

    See also :cpp:type:`oead::aamp::NameTable`

.. note:: For safety reasons, the underlying maps are not exposed.

.. autofunction:: oead.aamp.get_default_name_table

    See also :cpp:func:`oead::aamp::GetDefaultNameTable`
