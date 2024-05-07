# AlbaOS #
AlbaOS is a 32 bit operating system developed by me as a hobby <3


![Capture](https://github.com/CamH04/AlbaOS/assets/104907445/1fe59130-dfef-43ba-aa56-82f03429d57f)



Some Ascii art of my owls: 
```                
(0,0)
/)_)/
 **

(-,-_)
(x-x(v)7

(@,@)
[(_)]
 **

(^,^)
/)_)/
 **

       <3
  (^,^)  (^,^)
  /)_)/  /)_)/
---**-----**----
```

## How To Run AlbaOS On A Virtual Machine

+ Clone the repo localy / Download the zip folder

+ In your virtual machine select the boot / installer disc file as albaos.iso (in this example im using VMware workstation)

![uygArES](https://github.com/CamH04/AlbaOS/assets/104907445/1d880cb5-9830-4ff1-b29e-3e346baa0a31)


+ Run the virtual machine!

## How To Compile AlbaOS On Linux
+ make sure you have the installs listed below
+ cd to root folder (/AlbaOs)
+ use command: " make clean " to remove all generated .o , .bin and ,iso files
+ use command: " make albaos.iso " to generate all needed files



## Installs

```
sudo apt-get install g++
sudo apt-get install binutils
sudo apt-get install libc-dev-i386
sudo apt-get install grub-legacy
sudo apt-get install xorriso

```

+ g++ is a the compiler for the c++ code as i dont want to write my own
+ binutils is a binary manager as well as managing the assembaly code
+ libc-dev-i386 is the 32 bit architecture for the kernel
+ grub legacy allows for multibooting and installing the os into the grub
+ xorriso so the digital disc file (.iso) can be generated

(note) : grub-legacy is unavailable but grub-common or grub2 will work fine


## Stuff i've implemented 
+ working console output (printf) and coloured printf
+ global descriptor tables / ports
+ interrupts
+ keyboard
+ mouse
+ PCI (Peripheral Component Interconnect) and BAR (base address registers) 
+ Owl Art (:
+ Random numbers
+ Vga graphics mode
+ basic graphics framework
+ multitasking
+ speaker audio with PIT
+ Command Line With args
+ heap with malloc
