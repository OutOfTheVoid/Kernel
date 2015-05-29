CC=i686-elf-gcc
CXX=i686-elf-g++
ASM=nasm
LD=i686-elf-gcc

LINK_FLAGS= -ffreestanding -O2 -nostdlib -lgcc
ASM_FLAGS= -w+orphan-labels -f elf
C_FLAGS= -c -Wall -Wextra -masm=intel $(LINK_FLAGS) -I$(KINCLUDE)
CPP_FLAGS= $(C_FLAGS) -fno-exceptions -fno-rtti

LINKER_SCRIPT=src/Link.ld

KINCLUDE=include
KSOURCE=src
KOBJ=obj
KBIN=bin
KIMG=img

VDISK_UUID=2d29e9b4-4862-49ce-9942-5be30ff5c873
VM_NAME=Kernel

all: build test

build: bin/Kernel.bin 
	
test: img/grubhd.vdmk vm_start

KERNELBIN_OBJS= $(KOBJ)/boot/multiboot/boot.o $(KOBJ)/init/kinit/kinit.o $(KOBJ)/hw/cpu/hang.o $(KOBJ)/hw/video/VText.o $(KOBJ)/util/string/string.o $(KOBJ)/system/func/panic.o $(KOBJ)/system/func/kprintf.o $(KOBJ)/mm/paging/PFA.o $(KOBJ)/hw/cpu/GDT.o $(KOBJ)/hw/cpu/GDT.asm.o $(KOBJ)/mm/paging/PageTable.o $(KOBJ)/mm/paging/PageTable.asm.o $(KOBJ)/mm/paging/kernelpt.asm.o $(KOBJ)/interrupt/IState.o $(KOBJ)/mm/paging/AddressSpace.o $(KOBJ)/hw/cpu/IDT.o $(KOBJ)/hw/cpu/IDT.asm.o $(KOBJ)/interrupt/InterruptHandlers.o $(KOBJ)/interrupt/InterruptHandlers.asm.o $(KOBJ)/interrupt/APIC.o $(KOBJ)/interrupt/PIC.o $(KOBJ)/interrupt/IRQ.o $(KOBJ)/hw/acpi/ACPITable.o $(KOBJ)/system/func/pmalloc.o $(KOBJ)/mm/MM.o $(KOBJ)/mm/KMalloc.o

$(KBIN)/Kernel.bin: $(KSOURCE)/Link.ld $(KERNELBIN_OBJS)
	$(LD) $(KERNELBIN_OBJS) $(LINK_FLAGS) -o $(KBIN)/kimg.elf -T $(KSOURCE)/Link.ld

$(KOBJ)/boot/multiboot/boot.o: $(KSOURCE)/boot/multiboot/boot.asm
	$(ASM) $(KSOURCE)/boot/multiboot/boot.asm $(ASM_FLAGS) -o $(KOBJ)/boot/multiboot/boot.o

$(KOBJ)/init/kinit/kinit.o: $(KINCLUDE)/init/kinit/kinit.h $(KINCLUDE)/boot/multiboot/multiboot.h $(KINCLUDE)/hw/video/VText.h $(KINCLUDE)/hw/cpu/CPUID.h $(KSOURCE)/init/kinit/kinit.cpp 
	$(CXX) $(KSOURCE)/init/kinit/kinit.cpp $(CPP_FLAGS) -o $(KOBJ)/init/kinit/kinit.o

$(KOBJ)/hw/cpu/hang.o: $(KINCLUDE)/hw/cpu/hang.h $(KSOURCE)/hw/cpu/hang.asm
	$(ASM) $(KSOURCE)/hw/cpu/hang.asm $(ASM_FLAGS) -o $(KOBJ)/hw/cpu/hang.o

$(KOBJ)/hw/video/VText.o: $(KINCLUDE)/hw/video/VText.h $(KSOURCE)/hw/video/VText.cpp
	$(CXX) $(KSOURCE)/hw/video/VText.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/hw/video/VText.o

$(KOBJ)/util/string/string.o: $(KINCLUDE)/util/string/string.h $(KSOURCE)/util/string/string.cpp $(KOBJ)/hw/video/VText.o
	$(CXX) $(KSOURCE)/util/string/string.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/util/string/string.o
	
$(KOBJ)/system/func/kprintf.o: $(KINCLUDE)/system/func/kprintf.h $(KSOURCE)/system/func/kprintf.cpp
	$(CXX) $(KSOURCE)/system/func/kprintf.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/system/func/kprintf.o

$(KOBJ)/system/func/panic.o: $(KINCLUDE)/system/func/panic.h $(KSOURCE)/system/func/panic.cpp $(KOBJ)/system/func/kprintf.o
	$(CXX) $(KSOURCE)/system/func/panic.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/system/func/panic.o
	
$(KOBJ)/mm/paging/PFA.o: $(KINCLUDE)/mm/KMalloc.h $(KOBJ)/system/func/panic.o
	$(CXX) $(KSOURCE)/mm/paging/PFA.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/mm/paging/PFA.o
	
$(KOBJ)/hw/cpu/GDT.o: $(KINCLUDE)/hw/cpu/GDT.h $(KSOURCE)/hw/cpu/GDT.cpp
	$(CXX) $(KSOURCE)/hw/cpu/GDT.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/hw/cpu/GDT.o
	
$(KOBJ)/hw/cpu/GDT.asm.o: $(KSOURCE)/hw/cpu/GDT.asm
	$(ASM) $(KSOURCE)/hw/cpu/GDT.asm $(ASM_FLAGS) -o $(KOBJ)/hw/cpu/GDT.asm.o
	
$(KOBJ)/hw/cpu/IDT.o: $(KINCLUDE)/hw/cpu/IDT.h $(KSOURCE)/hw/cpu/IDT.cpp
	$(CXX) $(KSOURCE)/hw/cpu/IDT.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/hw/cpu/IDT.o
	
$(KOBJ)/hw/cpu/IDT.asm.o: $(KSOURCE)/hw/cpu/IDT.asm
	$(ASM) $(KSOURCE)/hw/cpu/IDT.asm $(ASM_FLAGS) -o $(KOBJ)/hw/cpu/IDT.asm.o
	
$(KOBJ)/mm/paging/PageTable.o: $(KINCLUDE)/mm/paging/PageTable.h $(KSOURCE)/mm/paging/PageTable.cpp
	$(CXX) $(KSOURCE)/mm/paging/PageTable.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/mm/paging/PageTable.o
	
$(KOBJ)/mm/paging/PageTable.asm.o: $(KSOURCE)/mm/paging/PageTable.asm
	$(ASM) $(KSOURCE)/mm/paging/PageTable.asm $(ASM_FLAGS) -o $(KOBJ)/mm/paging/PageTable.asm.o
	
$(KOBJ)/mm/paging/kernelpt.asm.o: $(KSOURCE)/mm/paging/kernelpt.asm
	$(ASM) $(KSOURCE)/mm/paging/kernelpt.asm $(ASM_FLAGS) -o $(KOBJ)/mm/paging/kernelpt.asm.o
	
$(KOBJ)/interrupt/IState.o: $(KINCLUDE)/interrupt/IState.h $(KSOURCE)/interrupt/IState.cpp
	$(CXX) $(KSOURCE)/interrupt/IState.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/interrupt/IState.o
	
$(KOBJ)/interrupt/InterruptHandlers.o: $(KINCLUDE)/interrupt/InterruptHandlers.h $(KSOURCE)/interrupt/InterruptHandlers.cpp
	$(CXX) $(KSOURCE)/interrupt/InterruptHandlers.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/interrupt/InterruptHandlers.o
	
$(KOBJ)/interrupt/InterruptHandlers.asm.o: $(KSOURCE)/interrupt/InterruptHandlers.asm
	$(ASM) $(KSOURCE)/interrupt/InterruptHandlers.asm $(ASM_FLAGS) -o $(KOBJ)/interrupt/InterruptHandlers.asm.o

$(KOBJ)/interrupt/PIC.o: $(KSOURCE)/interrupt/PIC.cpp $(KINCLUDE)/interrupt/PIC.h $(KINCLUDE)/hw/cpu/IO.h
	$(CXX) $(KSOURCE)/interrupt/PIC.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/interrupt/PIC.o
	
$(KOBJ)/interrupt/APIC.o: $(KSOURCE)/interrupt/APIC.cpp $(KINCLUDE)/interrupt/APIC.h $(KINCLUDE)/hw/cpu/CPUID.h
	$(CXX) $(KSOURCE)/interrupt/APIC.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/interrupt/APIC.o

$(KOBJ)/interrupt/IRQ.o: $(KSOURCE)/interrupt/IRQ.cpp $(KINCLUDE)/interrupt/IRQ.h
	$(CXX) $(KSOURCE)/interrupt/IRQ.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/interrupt/IRQ.o

$(KOBJ)/mm/paging/AddressSpace.o: $(KINCLUDE)/mm/paging/AddressSpace.h $(KSOURCE)/mm/paging/AddressSpace.cpp
	$(CXX) $(KSOURCE)/mm/paging/AddressSpace.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/mm/paging/AddressSpace.o
	
$(KOBJ)/hw/acpi/ACPITable.o: $(KINCLUDE)/hw/acpi/ACPITable.h $(KSOURCE)/hw/acpi/ACPITable.cpp
	$(CXX) $(KSOURCE)/hw/acpi/ACPITable.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/hw/acpi/ACPITable.o
	
$(KOBJ)/system/func/pmalloc.o: $(KINCLUDE)/system/func/pmalloc.h $(KINCLUDE)/math/bitmath.h $(KINCLUDE)/mm/paging/AddressSpace.h $(KINCLUDE)/mm/paging/PFA.h $(KSOURCE)/system/func/pmalloc.cpp 
	$(CXX) $(KSOURCE)/system/func/pmalloc.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/system/func/pmalloc.o
	
$(KOBJ)/mm/MM.o: $(KINCLUDE)/mm/MM.h $(KSOURCE)/mm/MM.cpp $(KINCLUDE)/mm/paging/PFA.h $(KINCLUDE)/mm/paging/PageTable.h $(KINCLUDE)/mm/paging/AddressSpace.h
	$(CXX) $(KSOURCE)/mm/MM.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/mm/MM.o
	
$(KOBJ)/mm/KMalloc.o: $(KINCLUDE)/mm/KMalloc.h $(KSOURCE)/mm/KMalloc.cpp $(KINCLUDE)/system/func/pmalloc.h
	$(CXX) $(KSOURCE)/mm/KMalloc.cpp $(CPP_FLAGS) -I$(KINCLUDE) -o $(KOBJ)/mm/KMalloc.o

$(KIMG)/grubhd.vdmk: bin/kimg.elf img/grubdisk.img
	hdiutil attach $(KIMG)/grubdisk.img > /dev/null
	-@rm /Volumes/GRUB2/boot/kimg.elf
	-@cp $(KBIN)/kimg.elf /Volumes/GRUB2/boot/kimg.elf
	-@sleep 0.5
	-@hdiutil detach /Volumes/GRUB2/ -force > /dev/null
	-@rm img/grubhd.vmdk
	-@VBoxManage convertdd $(KIMG)/grubdisk.img $(KIMG)/grubhd.vmdk --format VMDK --uuid $(VDISK_UUID) 2> /dev/null
	
vm_start: img/grubhd.vdmk
	-@VBoxManage controlvm $(VM_NAME) poweroff
	-@sleep 0.5
	-@VBoxManage startvm $(VM_NAME)
	
clean:
	-@find $(KOBJ) -type f -delete
	-@rm $(KBIN)/Kernel.bin

testpath:
	i686-elf-gcc --version