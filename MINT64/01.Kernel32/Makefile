NASM32 = nasm
GCC32 = gcc -c -m32 -ffreestanding
LD32 = ld -melf_i386 -T ../elf_i386.x -nostdlib -e main -Ttext 0x10200 # when loading, .text is loaded on 0x10200. and linking refers ../elf_i386.x.
OBJCOPY32 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

OBJECT_DIRECTORY = Temp
SOURCE_DIRECTORY = Source

all: prepare Kernel32.bin

prepare:
	mkdir -p $(OBJECT_DIRECTORY)

$(OBJECT_DIRECTORY)/EntryPoint.bin: $(SOURCE_DIRECTORY)/EntryPoint.s
	$(NASM32) -o $@ $< # $@ is the name of output file, and $< the first prerequisite.

dep:
	@echo === Make Dependency File ===
	make -C $(OBJECT_DIRECTORY) -f ../Makefile InternalDependency
	@echo === Dependency Search Complete ===

ExecuteInternelBuild: dep
	make -C $(OBJECT_DIRECTORY) -f ../Makefile Kernel32.elf

$(OBJECT_DIRECTORY)/Kernel32.elf.bin: ExecuteInternelBuild
	$(OBJCOPY32) $(OBJECT_DIRECTORY)/Kernel32.elf $@

Kernel32.bin: $(OBJECT_DIRECTORY)/EntryPoint.bin $(OBJECT_DIRECTORY)/Kernel32.elf.bin
	cat $^ > $@ # $^ is the name list of all prerequisites.

clean:
	rm -f *.bin
	rm -f $(OBJECT_DIRECTORY)/*.*

C_ENTRYPOINT_OBJECTFILE = main.o
C_SOURCEFILES = $(wildcard ../$(SOURCE_DIRECTORY)/*.c)
ASSEMBLY_SOURCEFILES = $(wildcard ../$(SOURCE_DIRECTORY)/*.asm)
C_OBJECTFILES = $(subst main.o, , $(notdir $(patsubst %.c, %.o, $(C_SOURCEFILES))))
ASSEMBLY_OBJECTFILES = $(notdir $(patsubst %.asm, %.o, $(ASSEMBLY_SOURCEFILES)))

%.o: ../$(SOURCE_DIRECTORY)/%.c
	$(GCC32) -c $<

%.o: ../$(SOURCE_DIRECTORY)/%.asm
	$(NASM32) -f elf32 -o $@ $<

InternalDependency:
	$(GCC32) -MM $(C_SOURCEFILES) > Dependency.dep

Kernel32.elf: $(C_ENTRYPOINT_OBJECTFILE) $(C_OBJECTFILES) $(ASSEMBLY_OBJECTFILES)
	$(LD32) -o $@ $^

ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif
