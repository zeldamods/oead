#########
Utilities
#########

This page documents some of the utilities found in ``oead/util/``.

.. warning:: Anything that is not explicitly documented on this page should be considered an implementation detail that is not to be relied upon.

Swap utils
==========
``#include <oead/util/swap.h>``

.. doxygenenum:: oead::util::Endianness
.. doxygenfunction:: oead::util::SwapValue
.. doxygenstruct:: oead::util::EndianInt
.. doxygentypedef:: oead::util::BeInt
.. doxygentypedef:: oead::util::LeInt

Variant utils
=============
``#include <oead/util/variant_utils.h>``

.. doxygenstruct:: oead::util::Variant
.. doxygenfunction:: oead::util::Visit
.. doxygenfunction:: oead::util::Match
