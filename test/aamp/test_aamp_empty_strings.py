import pytest
import oead
from pathlib import Path


# Catches possible YAML parsing regressions such as empty, tagged strings being turned
# into tildes:
# https://github.com/biojppm/rapidyaml/commit/98d3d9763584313a#commitcomment-42460490
def test_aamp_empty_strings():
    with (Path(__file__).parent / "test_empty_strings.yml").open("r") as test_yml:
        yml_data = test_yml.read()
    data: oead.aamp.ParameterIO = oead.aamp.ParameterIO.from_text(yml_data)
    assert data.type == "oead_test"
    assert data.version == 10

    assert str(data.objects["TestContent"].params["Str64_empty"].v) == ""
    assert str(data.objects["TestContent"].params["Str64_empty2"].v) == ""

    text_serialized = data.to_text()
    assert data == oead.aamp.ParameterIO.from_text(text_serialized)

    bin_serialized = data.to_binary()
    assert data == oead.aamp.ParameterIO.from_binary(bin_serialized)
