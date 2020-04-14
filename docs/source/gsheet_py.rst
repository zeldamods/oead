###############
gsheet (Python)
###############

For more information about Grezzo datasheets, `refer to the wiki article <https://zeldamods.org/las/Datasheet>`_.

.. autoclass:: oead.gsheet.Struct

Lists
=====

These classes work like Python lists, but they can only store elements of a single type, i.e. all elements must be booleans, or all strings, etc.

.. class:: oead.gsheet.StructArray
.. autoclass:: oead.gsheet.BoolArray
.. autoclass:: oead.gsheet.IntArray
.. autoclass:: oead.gsheet.FloatArray
.. autoclass:: oead.gsheet.StringArray

Read-write API
==============

This API is used to read, modify and write binary datasheets. Datasheets can also be constructed from scratch.

.. autoclass:: oead.gsheet.Field
.. autoclass:: oead.gsheet.Sheet
.. autofunction:: oead.gsheet.parse
