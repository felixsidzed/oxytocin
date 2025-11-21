import os
import sys
import subprocess
from glob import glob
from tempfile import TemporaryDirectory

CC = "x86_64-elf-gcc"
AS = "nasm"
LD = "x86_64-elf-ld"
QEMU = "qemu-system-x86_64"
TEA = "./tea.exe" # https://github.com/felixsidzed/tea

CCFLAGS = "-ffreestanding -std=gnu99 -O2 -Isrc -Isrc/std -mcmodel=kernel -fno-stack-protector -mno-red-zone -Wall -Werror -include src/common.h -include src/std/stdint.h"
ASFLAGS = ""
LDFLAGS = "-nostd -T linker.ld"
TEAFLAGS = "--triple x86_64-elf -v -64 -O0 -Isrc"

QEMUFLAGS = "-net none -cpu qemu64,+ssse3,+sse4.1,+sse4.2"

def run(cmd: str, wait=True, **kw) -> int:
	print(cmd)
	if wait:
		try:
			return subprocess.check_call(cmd, shell=True, **kw)
		except subprocess.CalledProcessError as e:
			sys.exit(1)
			return 0
	else:
		subprocess.Popen(cmd, shell=True, **kw)
		return 0

def build(debug=False):
	files = []
	for ext in ("c", "s"): # , "tea"
		files.extend(glob(f"./**/*.{ext}", recursive=True))

	print(f"found {len(files)} source files")

	objects = []
	for file in files:
		name, ext = file.rsplit(".", 1)
		name = os.path.basename(name)
		if ext == "s":
			out = os.path.join("build", f"{name}")

			if name in ("stage1", "stage2"):
				run(f"{AS} {ASFLAGS} -f bin {file} -o {out}.bin")
			else:
				run(f"{AS} {ASFLAGS} -f elf64 {file} -o {out}.o")
				out += ".o"
				if name == "stage3": objects.insert(0, out)
				else: objects.append(out)
		elif ext == "tea":
			out = os.path.join("build", f"{name}.o")
			run(f"{TEA} {TEAFLAGS} {file} -o {out}")
			objects.append(out)
		else:
			out = os.path.join("build", f"{name}.o")
			if debug:
				run(f"{CC} {CCFLAGS} -g -c {file} -o {out}")
			else:
				run(f"{CC} {CCFLAGS} -c {file} -o {out}")
			objects.append(out)

	iso = os.path.join("build", "os.iso")
	stage1 = os.path.join("build", "stage1.bin")
	stage2 = os.path.join("build", "stage2.bin")
	kernel = os.path.join("build", "kernel.raw")

	if len(objects) == 0:
		print("no object files to link")
		return 1

	if debug:
		run(f"{LD} {LDFLAGS} -o {os.path.join('build', 'kernel.elf')} {' '.join(objects)}")
	_ = os.path.join("build", "kernel.bin")
	run(f"{LD} {LDFLAGS} -o {_} {' '.join(objects)}")
	run(f"objcopy -O binary {_} {kernel}")
    
	with TemporaryDirectory() as tmp:
		bootDir = os.path.join(tmp, "boot")
		os.makedirs(bootDir, exist_ok=True)
		
		image = os.path.join(bootDir, "image.bin")
		
		with open(image, "wb") as outfile:
			with open(stage1, "rb") as infile:
				outfile.write(infile.read())
			
			with open(stage2, "rb") as infile:
				outfile.write(infile.read())
			
			klba = outfile.tell()
			if klba % 512 != 0:
				outfile.write(b'\0' * (512 - (klba % 512)))
				klba = outfile.tell()
			
			print(f"kernel lba: {klba // 512}")
			
			with open(kernel, "rb") as infile:
				outfile.write(infile.read())

			ksize = (outfile.tell() - klba) // 512
			print(f"kernel size: {ksize} sectors")

			with open(stage1, "r+b") as f:
				f.seek(0x20)
				chunk = f.read(24)
				offset = 0x20 + chunk.find((0x67).to_bytes(2, "little"))
				if offset == 0x1F:
					print("packet.size offset not found (is the size 0x67?)")
					sys.exit(1)
				print(f"packet.size at {offset:#x}")
				f.seek(offset)
				f.write(((klba // 512) + ksize).to_bytes(2, "little"))
		
		run(
			f"xorriso -as mkisofs -b boot/{os.path.basename(image)} -c boot/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -isohybrid-mbr {image} -o {iso} {tmp}",
			stdout=subprocess.DEVNULL,
			stderr=subprocess.DEVNULL,
			stdin=subprocess.DEVNULL,
		)
	return iso

def main(argv) -> int:
	iso = build(len(argv) > 1 and argv[1] == "debug")
	if len(argv) <= 1:
		return 0

	if argv[1] == "run":
		return run(f"{QEMU} -hdd {iso} {QEMUFLAGS}")
	elif argv[1] == "debug":
		run(f"{QEMU} -hdd {iso} -d int {QEMUFLAGS} -s -S", False)
		return run(f"gdb -ex 'target remote localhost:1234' {os.path.join('build', 'kernel.elf')}", True)
	
	return 0

if __name__ == "__main__":
	sys.exit(main(sys.argv))
