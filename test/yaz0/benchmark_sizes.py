from texttable import Texttable

import libyaz0
import oead
import wszst_yaz0
from files import TEST_FILES, TEST_FILE_DATA, TEST_FILE_DATA_UNCOMP

ENCODERS = {
    "libyaz0_lv1": lambda data: libyaz0.compress(data, level=1),
    "libyaz0_lv6": lambda data: libyaz0.compress(data, level=6),
    "libyaz0_lv9": lambda data: libyaz0.compress(data, level=9),
    "libyaz0_lv10": lambda data: libyaz0.compress(data, level=10),
    "syaz0_lv6": lambda data: oead.yaz0.compress(data, level=6),
    "syaz0_lv7": lambda data: oead.yaz0.compress(data, level=7),
    "syaz0_lv8": lambda data: oead.yaz0.compress(data, level=8),
    "syaz0_lv9": lambda data: oead.yaz0.compress(data, level=9),
    "wszst_yaz0_lv1": lambda data: wszst_yaz0.compress(data, level=1),
    "wszst_yaz0_lv6": lambda data: wszst_yaz0.compress(data, level=6),
    "wszst_yaz0_lv9": lambda data: wszst_yaz0.compress(data, level=9),
    "wszst_yaz0_lv10": lambda data: wszst_yaz0.compress(data, level=10),
}

def test_file(name) -> None:
    print(f">>>>>>>>>> {name} <<<<<<<<<<")
    print()

    t = Texttable(max_width=100)
    t.set_deco(Texttable.BORDER | Texttable.HEADER | Texttable.VLINES)
    t.set_precision(3)
    t.header(["Encoder", "Size", "Diff", "Diff%"])
    t.set_cols_align(["l", "r", "r", "r"])

    rows = []

    for encoder, encode in ENCODERS.items():
        nintendo_len = len(TEST_FILE_DATA[name])
        encoder_len = len(encode(TEST_FILE_DATA_UNCOMP[name]))
        diff = encoder_len - nintendo_len
        ratio = encoder_len / nintendo_len
        percentage = (ratio - 1) * 100.0
        rows.append([encoder, encoder_len, diff, percentage])

    # Sort by diff.
    rows.sort(key=lambda row: row[2])
    t.add_rows(rows, header=False)

    print(t.draw())
    print()

for name in TEST_FILE_DATA.keys():
    test_file(name)
