import os
import sys

elf_path = sys.argv[1]
bin_path = sys.argv[2]
flash_kb = int(sys.argv[3])   # 2048KB for RP2040

def fmt(x):
    if x < 1024:
        return f"{x} B"
    elif x < 1024*1024:
        return f"{x/1024:.2f} KB"
    else:
        return f"{x/1024/1024:.2f} MB"

def get_size(path):
    if os.path.exists(path):
        return os.path.getsize(path)
    return 0

bin_size = get_size(bin_path)

print("\n[ BIN INFO ]")
# print(f"Path : {bin_path}")
print(f"Size : {bin_size} bytes ({fmt(bin_size)})")

# print("\n[ FLASH INFO ]")
# print("Use arm-none-eabi-size above for FLASH usage")

print("\n[ FLASH INFO ]")
flash_used = None

flash_used = bin_size
percent = (flash_used / (flash_kb * 1024)) * 100

print(f"FLASH: {fmt(flash_used)} / {flash_kb} KB ({percent:.2f}%)")