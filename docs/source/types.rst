############
Common types
############

Numbers
=======

In order to preserve type information -- which matters for binary formats such as `BYML` -- when passing types between oead and Python code, several number wrapper classes are used.

In C++, using the wrappers requires no special treatment since they are constructible from and implicitly convertible to numbers.

.. doxygenstruct:: oead::Number
.. doxygentypedef:: oead::U8
.. doxygentypedef:: oead::U16
.. doxygentypedef:: oead::U32
.. doxygentypedef:: oead::U64
.. doxygentypedef:: oead::S8
.. doxygentypedef:: oead::S16
.. doxygentypedef:: oead::S32
.. doxygentypedef:: oead::S64
.. doxygentypedef:: oead::F32
.. doxygentypedef:: oead::F64

Utils
=====

.. doxygenstruct:: oead::Vector2
.. doxygentypedef:: oead::Vector2f
.. doxygenstruct:: oead::Vector3
.. doxygentypedef:: oead::Vector3f
.. doxygenstruct:: oead::Vector4
.. doxygentypedef:: oead::Vector4f
.. doxygenstruct:: oead::Quat
.. doxygentypedef:: oead::Quatf
.. doxygenstruct:: oead::Color4f
.. doxygenstruct:: oead::Curve

Strings
=======

.. note:: Any string that is too long to be stored in a ``FixedSafeString`` is truncated.

.. doxygenstruct:: oead::FixedSafeString

.. note::
    In sead, this is actually a derived class of ``sead::BufferedSafeString``
    which is in turn derived from ``sead::SafeString``.

    Since the latter is essentially a {vptr, const char* cstr} pair
    and the former is a std::string_view, we will not bother implementing
    those base classes.
