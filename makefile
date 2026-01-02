# im sorry about this makefile >v<
# sudo apt-get install g++ binutils libc6-dev-i386 grub-legacy xorriso

GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-threadsafe-statics -lm
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
	  obj/common/asl.o\
	  obj/common/asl_maths.o\
	  obj/common/asl_string.o\
	  obj/common/asl_mem.o\
	  obj/gdt.o \
	  obj/memorymanagement.o \
	  obj/owlart.o \
	  obj/drivers/driver.o \
	  obj/hardwarecommunication/port.o \
	  obj/hardwarecommunication/interruptstubs.o \
	  obj/hardwarecommunication/interrupts.o \
	  obj/multitasking.o \
	  obj/drivers/amd_am79c973.o \
	  obj/hardwarecommunication/pci.o \
	  obj/hardwarecommunication/power.o \
	  obj/hardwarecommunication/acpi.o \
	  obj/hardwarecommunication/apm_bios_call.o \
	  obj/drivers/cmos.o \
	  obj/drivers/keyboard.o \
	  obj/drivers/mouse.o \
	  obj/drivers/vga.o \
	  obj/drivers/ata.o \
	  obj/drivers/pit.o \
	  obj/drivers/virtual_keyboard.o \
	  obj/drivers/audio.o \
	  obj/gui/widget.o \
	  obj/gui/desktop.o \
	  obj/gui/window.o \
	  obj/gui/sim.o \
	  obj/gui/raycasting.o \
	  obj/filesys/ofs.o \
	  obj/networking/eframe.o \
	  obj/networking/arp.o \
	  obj/cli.o \
	  obj/app.o \
	  obj/nests/filenest.o \
	  obj/nests/raycastdemo.o \
	  obj/nests/snake.o \
	  obj/nests/meatball.o \
	  obj/nests/pipe.o \
	  obj/nests/terminalgame.o \
	  obj/playstart.o \
	  obj/cpuid.o \
	  obj/syscalls.o \
	  obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

		#the linker has truley baffled me as of today, i will never question it again. ima just let it do its thing
albaos.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

albaos.iso: albaos.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp albaos.bin iso/boot/albaos.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "Alba Os            " {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/albaos.bin'    >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=albaos.iso iso
	rm -rf iso

install: albaos.bin
	sudo cp $< /boot/albaos.bin


run: albaos.iso
	qemu-system-x86_64 -enable-kvm \
	-boot menu=on \
	-drive id=disk,file=albaos.iso,format=raw,if=none \
	-device piix4-ide,id=piix4 -device ide-hd,drive=disk,bus=piix4.0 \
	-smp 1 -m 8M \
	-vga virtio \
	-machine type=pc,accel=kvm




vrun: albaos.iso
	(killall VirtualBox && sleep 1) || true
	VBoxManage startvm 'AlbaOSVM' --type gui


.PHONY: clean
clean:
	rm -rf obj albaos.bin albaos.iso
