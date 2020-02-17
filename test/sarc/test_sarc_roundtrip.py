import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("sarc/files/*.sarc")


@pytest.mark.parametrize("file", cases)
def test_sarc_roundtrip(file):
    arc = oead.Sarc(cases_data[file])
    writer = oead.SarcWriter.from_sarc(arc)
    alignment, data2 = writer.write()
    assert cases_data[file] == bytes(data2)
