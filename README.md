# AlbaOS #
AlbaOS is a 32 bit operating system developed by me as a hobby <3

![KCQMHsd](https://github.com/CamH04/AlbaOS/assets/104907445/782c68b5-ff62-4646-909c-ddcfdfb60b8c)



Some Ascii art of my owls: 
```                
(0,0)
/)_)/
 **

(-,-_)
(x-x(v)7

  _____       
 / ___ \      
(|(0,0)|)     
  /)_)/       
   **


 (0,0)
==)_)==
  **


(@,@)
[(_)]
 **

(^,^)
/)_)/
 **

(v,v)
/)_)/
 **

       <3
  (^,^)  (^,^)
  /)_)/  /)_)/
---**-----**----
```

## How To Run AlbaOS On A Virtual Machine

+ Clone the repo localy / Download the zip folder

+ In your virtual machine select the boot / installer disc file as mykernel.iso (in this example im using VMware workstation)

![uygArES](https://github.com/CamH04/AlbaOS/assets/104907445/1d880cb5-9830-4ff1-b29e-3e346baa0a31)


+ Run the virtual machine!


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
+ working console output (printf)
+ global descriptor tables / ports
+ interrupts
+ keyboard
+ mouse
+ PCI (Peripheral Component Interconnect) and BAR (base address registers) 
+ Owl Art (:
+ Random numbers
+ Vga graphics mode
+ basic graphics framework
