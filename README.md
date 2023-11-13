# AlbaOS #
AlbaOS is a 32 bit operating system developed by me

![image](https://github.com/CamH04/AlbaOS/assets/104907445/0342bfcb-fbdf-4e46-b6c0-f127e1655e38)



Some Ascii art of my owls: 
[New Text Document.txt](https://github.com/CamH04/AlbaOS/files/13266996/New.Text.Document.txt)


## How To Run AlbaOS On A Virtual Machine

+ Clone the repo localy / Download the zip folder

![image](https://github.com/CamH04/AlbaOS/assets/104907445/d822d2a2-9967-41ca-aab4-98f97f8d42ce)

+ In your virtual machine seleect the boot / installer disc file as mykernel.iso (in this example im using VMware workstation)

![image](https://github.com/CamH04/AlbaOS/assets/104907445/7ff1f85a-9976-4c50-8719-8a6796b16018)

+ Run the virtual machine!


## Installs

```
sudo apt-get install g++
sudo apt-get install binutils
sudo apt-get install libc-dev-i386

```

+ g++ is a the compiler for the c++ code as i dont want to write my own
+ binutils is a binary manager as well as managing the assembaly code
+ libc-dev-i386 is the 32 bit architecture for the kernel
