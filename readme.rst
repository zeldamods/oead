======
 oead
======

**oead** is a C++ library for common file formats that are used in modern first-party Nintendo EAD (now EPD) titles.

Python bindings for 3.6+ are also available.

Features
========

Currently, oead only handles very common formats that are extensively used in recent games such as *Breath of the Wild* and *Super Mario Odyssey*.

* `AAMP <https://zeldamods.org/wiki/AAMP>`_ (binary parameter archive): Only version 2 is supported.
* `BYML <https://zeldamods.org/wiki/BYML>`_ (binary YAML): Versions 1, 2, 3, and 4 are supported.
* `SARC <https://zeldamods.org/wiki/SARC>`_ (archive)
* `Yaz0 <https://zeldamods.org/wiki/Yaz0>`_ (compression algorithm)

oead also supports a recent Grezzo format that is used in *Link's Awakening (Switch)*:

* `gsheet <https://zeldamods.org/las/Datasheet>`_ (Grezzo datasheet)

Getting started
===============

To install the Python module, simply run:

   pip install oead

This will download and install a precompiled version of oead for the following platforms:

* Windows (x86-64 / 64-bit)
* Recent Linux distributions (x86-64, glibc and musl)

Precompiled builds for macOS are currently unavailable as the wheel build process seems to be broken.
Please help us fix this `release failure <https://github.com/zeldamods/oead/actions/runs/9588470856/job/26440567392>` if you know how!

The following versions of Python are supported:

* CPython 3.6 to 3.12
* PyPy3.7 to 3.9

If you are using any other platform, you must build oead from source (refer to the next section).

.. warning::
   Windows users must ensure that they have the `latest Visual C++ 2019 Redistributable <https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads>`_ installed.

For more information, refer to the `documentation <https://oead.readthedocs.io/>`_.

Building from source
====================

Building oead from source requires:

* CMake 3.12+
* A compiler that supports C++17
* Everything needed to build libyaml

First, clone the repository then enter the oead directory and run ``git submodule update --init --recursive``.

Building the Python module
--------------------------

* To install the module, run ``pip install -e .``. This requires the following Python modules to be installed: setuptools, wheel
* If you just want to build the Python module from source without installing it, run ``python setup.py bdist_wheel``.

C++ usage
---------

Linking to the ``oead`` target is sufficient to use the library.


Contributing
============

* Issue tracker: `<https://github.com/zeldamods/oead/issues>`_
* Source code: `<https://github.com/zeldamods/oead>`_

This project is licensed under the GPLv2+ license.
