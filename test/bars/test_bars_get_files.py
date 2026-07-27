import pytest
import oead

from utils import make_test_cases

cases, cases_data = make_test_cases("bars/files/*.bars")

@pytest.mark.parametrize("file", cases)
def test_bars_get_file(file):
    res = oead.audio.Bars(cases_data[file])
    for asset in res.get_files():
        assert res.get_file(asset.metadata.asset_name) is not None