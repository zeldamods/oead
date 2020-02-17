======
 oead
======

**oead** is a C++ library for common file formats that are used in modern first-party Nintendo EAD (now EPD) titles.

Python bindings for 3.6+ are also available.

Features
========

Currently, oead only handles two very common formats that are extensively used in recent games such as *Breath of the Wild* and *Super Mario Odyssey*.

* `BYML <https://zeldamods.org/wiki/BYML>`_ (binary YAML): Versions 2, 3, and 4 are supported.
* `AAMP <https://zeldamods.org/wiki/AAMP>`_ (binary parameter archive): Only version 2 is supported.
* `SARC <https://zeldamods.org/wiki/SARC>`_ (archive)
* `Yaz0 <https://zeldamods.org/wiki/Yaz0>`_ (compression algorithm)

Getting started
===============

Install with::

   pip install oead

.. warning::
   Windows users must ensure that they have the `latest Visual C++ 2019 Redistributable <https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads>`_ installed.

For more information, refer to the `documentation <https://oead.readthedocs.io/>`_.

Building from source
====================

Building oead from source requires:

* CMake 3.10+
* A compiler that supports C++17
* Everything needed to build libyaml

Linking to the ``oead`` target is sufficient to use the library.

To build the Python bindings from source, run ``python setup.py bdist_wheel``. This requires setuptools to be installed.

Contributing
============

* Issue tracker: `<https://github.com/zeldamods/oead/issues>`_
* Source code: `<https://github.com/zeldamods/oead>`_

This project is licensed under the GPLv2+ license.
