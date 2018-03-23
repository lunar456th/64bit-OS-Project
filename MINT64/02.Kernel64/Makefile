NASM64 = nasm -f elf64
GCC64 = gcc -c -m64 -ffreestanding
LD64 = ld -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e main -Ttext 0x200000
OBJCOPY64 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECT_DIRECTORY = Temp
SOURCE_DIRECTORY = Source

all: prepare Kernel64.bin

prepare:
	mkdir -p $(OBJECT_DIRECTORY)

dep:
	@echo === Make Dependancy File ===
	make -C $(OBJECT_DIRECTORY) -f ../Makefile InternalDependency
	@echo === Dependancy Search Complete ===

ExecuteInternalBuild: dep
	make -C $(OBJECT_DIRECTORY) -f ../Makefile Kernel64.elf

Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECT_DIRECTORY)/Kernel64.elf $@
		
clean:
	rm -f *.bin
	rm -f $(OBJECT_DIRECTORY)/*.*

ENTRYPOINT_SOURCEFILE = ../$(SOURCE_DIRECTORY)/EntryPoint.s
ENTRYPOINT_OBJECTFILE = EntryPoint.o

C_SOURCEFILES = $(wildcard ../$(SOURCE_DIRECTORY)/*.c)
ASSEMBLY_SOURCEFILES = $(wildcard ../$(SOURCE_DIRECTORY)/*.asm)
C_OBJECTFILES = $(notdir $(patsubst %.c, %.o, $(C_SOURCEFILES)))
ASSEMBLY_OBJECTFILES = $(notdir $(patsubst %.asm, %.o, $(ASSEMBLY_SOURCEFILES)))

$(ENTRYPOINT_OBJECTFILE): $(ENTRYPOINT_SOURCEFILE)
	$(NASM64) -o $@ $<

%.o: ../$(SOURCE_DIRECTORY)/%.c
	$(GCC64) -c $<

%.o: ../$(SOURCE_DIRECTORY)/%.asm
	$(NASM64) -o $@ $<

InternalDependency:
	$(GCC64) -MM $(C_SOURCEFILES) > Dependency.dep

Kernel64.elf: $(ENTRYPOINT_OBJECTFILE) $(C_OBJECTFILES) $(ASSEMBLY_OBJECTFILES)
	$(LD64) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif
