
# sudo apt-get install g++ binutils libc6-dev-i386
# sudo apt-get install VirtualBox grub-legacy xorriso

GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -fno-threadsafe-statics
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
          obj/gdt.o \
          obj/owlart.o \
          obj/drivers/driver.o \
          obj/drivers/pit.o \
          obj/hardwarecommunication/port.o \
          obj/hardwarecommunication/interruptstubs.o \
          obj/hardwarecommunication/interrupts.o \
          obj/multitasking.o \
          obj/hardwarecommunication/pci.o \
          obj/drivers/keyboard.o \
          obj/drivers/mouse.o \
          obj/drivers/audio.o \
          obj/drivers/vga.o \
	  obj/gui/widget.o \
          obj/gui/window.o \
          obj/gui/desktop.o \
          obj/kernel.o

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

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

.PHONY: clean
clean:
	rm -rf obj albaos.bin albaos.iso
