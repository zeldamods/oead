#####################
Common types (Python)
#####################

Buffers
=======

All of the following types support the buffer protocol.

.. autoclass:: oead.Bytes

    This class is the equivalent to ``std::vector<u8>``.

.. autoclass:: oead.BufferInt

    This class is the equivalent to ``std::vector<int>``.

.. autoclass:: oead.BufferF32

    This class is the equivalent to ``std::vector<f32>``.

.. autoclass:: oead.BufferU32

    This class is the equivalent to ``std::vector<u32>``.

Numbers
=======

In order to preserve type information -- which matters for binary formats such as `BYML` -- when passing types between oead and Python code, several number wrapper classes are used.

All of the wrappers can be converted to ``int`` or ``float`` with ``int()`` or ``float()``.

.. warning::
    Unlike Python's ``int`` class, these integer classes represent fixed-size integers. Just like in C++, unsigned integer types use modulo 2^N arithmetic (e.g. -1 is 255) and signed integer overflow is undefined behavior.

.. autoclass:: oead.U8

    Unsigned 8-bit integer.

.. autoclass:: oead.U16

    Unsigned 16-bit integer.

.. autoclass:: oead.U32

    Unsigned 32-bit integer.

.. autoclass:: oead.U64

    Unsigned 64-bit integer.

.. autoclass:: oead.S8

    Signed 8-bit integer.

.. autoclass:: oead.S16

    Signed 16-bit integer.

.. autoclass:: oead.S32

    Signed 32-bit integer.

.. autoclass:: oead.S64

    Signed 64-bit integer.

.. autoclass:: oead.F32

    binary32 floating point number.

.. autoclass:: oead.F64

    binary64 floating point number.

Utils
=====

.. autoclass:: oead.Vector2f

    See also :cpp:type:`oead::Vector2f`

.. autoclass:: oead.Vector3f

    See also :cpp:type:`oead::Vector3f`

.. autoclass:: oead.Vector4f

    See also :cpp:type:`oead::Vector4f`

.. autoclass:: oead.Quatf

    See also :cpp:type:`oead::Quatf`

.. autoclass:: oead.Color4f

    See also :cpp:type:`oead::Color4f`

.. autoclass:: oead.Curve

    See also :cpp:type:`oead::Curve`

Strings
=======

.. note:: Any string that is too long to be stored in a ``FixedSafeString`` is truncated.

.. warning:: As Python does not have implicit conversions, ``str()`` must be explicitly called to turn a FixedSafeString into an actual string.

.. autoclass:: oead.FixedSafeString16
.. autoclass:: oead.FixedSafeString32
.. autoclass:: oead.FixedSafeString48
.. autoclass:: oead.FixedSafeString64
.. autoclass:: oead.FixedSafeString128
.. autoclass:: oead.FixedSafeString256

.. seealso:: :cpp:struct:`template <size_t N> oead::FixedSafeString`
