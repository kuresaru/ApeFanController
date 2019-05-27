SET C51INC=D:\DevTools\keil5\C51\Inc;D:\DevTools\keil5\C51\Inc\STC
SET C51LIB=D:\DevTools\keil5\C51\Lib
SET CPU_TYPE=STC15F104W
SET CPU_VENDOR=STC
SET UV2_TARGET=Target 1
SET CPU_XTAL=0x005B8D80
"D:\DevTools\keil5\C51\BIN\A51.EXE" @".\Objects\STARTUP._ia"
"D:\DevTools\keil5\C51\BIN\C51.EXE" @".\Objects\main.__i"
"D:\DevTools\keil5\C51\BIN\C51.EXE" @".\Objects\delay.__i"
"D:\DevTools\keil5\C51\BIN\C51.EXE" @".\Objects\timer.__i"
"D:\DevTools\keil5\C51\BIN\BL51.EXE" @".\Objects\AFC.lnp"
"D:\DevTools\keil5\C51\BIN\OH51.EXE" ".\Objects\AFC" 
