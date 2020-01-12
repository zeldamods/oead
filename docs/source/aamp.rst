####
AAMP
####

.. include:: _aamp_status.rst

Parameters
==========

.. doxygenclass:: oead::aamp::Parameter
.. doxygenstruct:: oead::aamp::Name

.. doxygentypedef:: oead::aamp::ParameterMap
.. doxygentypedef:: oead::aamp::ParameterObjectMap
.. doxygentypedef:: oead::aamp::ParameterListMap

.. note:: :cpp:struct:`oead::aamp::Name` can be constructed from either a hash or a string, so it is possible to access elements in a map by name rather than by hash. Both ``object.params[crc32_hash]`` and ``object.params["NameString"]`` will work.

.. doxygenstruct:: oead::aamp::ParameterObject
.. doxygenstruct:: oead::aamp::ParameterList

Parameter IO
============

.. doxygenstruct:: oead::aamp::ParameterIO

Name utilities
==============

Because binary parameter archives only store CRC32 hashes of structure names, recovering the original names -- which is useful for converting archives to a human-readable format -- requires the use of a name table.

When serializing to YAML, by default oead will use a table that contains strings from *Breath of the Wild*'s executable.

.. doxygenstruct:: oead::aamp::NameTable

.. doxygenfunction:: oead::aamp::GetDefaultNameTable
