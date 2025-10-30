#!/bin/bash
( echo 'change vnc password' ; sleep 1 ; echo '0' ) | qemu-system-x86_64 -vnc :0,password=on -monitor stdio -drive file=./a.vfd,index=0,if=floppy,format=raw -boot a
