oead supports Yaz0 decompression and fast compression (using `syaz0 <https://github.com/zeldamods/syaz0>`_)

Performance
===========
Decompression performance is on par with existing Yaz0 decoders.

As of late December 2019, syaz0 is able to compress files much faster than existing Yaz0 encoders. Files that are representative of *Breath of the Wild* assets were compressed 20x to 30x faster than with existing public tools for an equivalent or better compression ratio, and 70-80x faster (with a slightly worse ratio) in extreme cases.

At the default compression level, file sizes are typically within 1% of Nintendo's.

For detailed benchmarks, see the results files in the `test directory of the syaz0 project <https://github.com/zeldamods/syaz0/tree/master/test>`_.
