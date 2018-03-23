dir=/home/sj/git/MINT64
img=Disk.img

all: BootLoader Kernel32 Kernel64 Utility Disk.img
	qemu-system-x86_64 -L . -m 64 -fda $(dir)/$(img) -localtime -M pc

BootLoader:
	@echo
	@echo ============= Build Boot Loader ==============
	@echo

	make -C 00.BootLoader

	@echo
	@echo ============= Build Boot Loader ==============
	@echo

Kernel32:
	@echo
	@echo ============= Build 32bit Kernel =============
	@echo

	make -C 01.Kernel32

	@echo
	@echo =============== Build Complete ===============
	@echo
	
Kernel64:
	@echo
	@echo ============= Build 64bit Kernel =============
	@echo

	make -C 02.Kernel64

	@echo
	@echo =============== Build Complete ===============
	@echo

Utility:
	@echo
	@echo ============ Utility Build Start =============
	@echo

	make -C 04.Utility

	@echo
	@echo =========== Utility Build Complete ===========
	@echo

Disk.img: 00.BootLoader/BootLoader.bin 01.Kernel32/Kernel32.bin 02.Kernel64/Kernel64.bin
	@echo
	@echo =========== Disk Image Build Start ===========
	@echo

	./ImageMaker.exe $^

	@echo
	@echo ============= All Build Complete =============
	@echo

clean:
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	make -C 02.Kernel64 clean
	make -C 04.Utility clean
	rm -f Disk.img
