####
Yaz0
####

.. include:: parts/yaz0_common.rst

API
===

``#include <oead/yaz0.h>``

.. doxygenstruct:: oead::yaz0::Header
.. doxygenfunction:: oead::yaz0::GetHeader

.. doxygenfunction:: oead::yaz0::Compress
.. doxygenfunction:: oead::yaz0::Decompress(tcb::span<const u8>)
.. doxygenfunction:: oead::yaz0::Decompress(tcb::span<const u8>, tcb::span<u8>)
.. doxygenfunction:: oead::yaz0::DecompressUnsafe
