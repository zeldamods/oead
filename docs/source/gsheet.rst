######
gsheet
######

``#include <oead/gsheet.h>``

For more information about Grezzo datasheets, `refer to the wiki article <https://zeldamods.org/las/Datasheet>`_.

Read-only API and binary format structs
=======================================

This API is used to read binary datasheets efficiently.

In line with Nintendo's naming conventions, structs that are directly used in the binary format have a `Res` prefix in their name.

.. doxygenstruct:: oead::gsheet::ResHeader
.. doxygenstruct:: oead::gsheet::ResField
.. doxygenclass:: oead::gsheet::Sheet
.. doxygentypedef:: oead::gsheet::FieldMap
.. doxygenfunction:: oead::gsheet::MakeFieldMap

Utilities for defining datasheet structures
-------------------------------------------

These are useful to define structures which datasheet values can be casted to.

.. doxygenstruct:: oead::gsheet::Nullable
.. doxygenstruct:: oead::gsheet::Array
.. doxygenstruct:: oead::gsheet::String

Read-write API
==============

This API is used to read, modify and write binary datasheets. Datasheets can also be constructed from scratch.

.. doxygenstruct:: oead::gsheet::Field
.. doxygenstruct:: oead::gsheet::Data
.. doxygenstruct:: oead::gsheet::SheetRw
