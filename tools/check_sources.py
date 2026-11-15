"""Check recovered packaging, not hardware behavior or target builds."""
from pathlib import Path
import json
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
ip = root / "hardware/ip/multiply_1_0"
ns = {"s": "http://www.spiritconsortium.org/XMLSchema/SPIRIT/1685-2009"}
component = ET.parse(ip / "component.xml").getroot()
files = component.findall(".//s:file/s:name", ns)
for name in files:
    assert (ip / name.text).is_file(), f"Missing IP file: {name.text}"
vlnv = ":".join(component.find(f"s:{field}", ns).text
                for field in ("vendor", "library", "name", "version"))
bd = json.loads((root / "hardware/vivado/lab4_block_design.bd").read_text())["design"]
assert bd["design_info"]["device"] == "xc7z010clg400-1"
assert bd["design_info"]["tool_version"] == "2023.1"
assert bd["components"]["multiply_0"]["vlnv"] == vlnv
segment = bd["addressing"]["/processing_system7_0"]["address_spaces"]["Data"]["segments"]["SEG_multiply_0_S00_AXI_reg"]
assert int(segment["offset"], 16) == 0x43C00000
assert segment["range"] == "64K"
driver = root / "software/linux/chardev/multiply.c"
builtin = root / "software/linux/builtin/multiplier_driver/multiplier.c"
assert driver.read_bytes() == builtin.read_bytes(), "Built-in source copy drifted"
print(f"PASS: {len(files)} IP file references exist; BD/IP VLNV {vlnv} matches")
print("PASS: device/tool metadata and BD mapping 0x43C00000/64K match")
print("PASS: reconstructed built-in source matches canonical Lab 6 driver")
