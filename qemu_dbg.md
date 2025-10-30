
```
( echo 'change vnc password' ; sleep 1 ; echo '0' ) | qemu-system-x86_64 -vnc :0,password=on -monitor stdio -drive file=./a.vfd,index=0,if=floppy,format=raw -boot a -s -S
# -s：是 -gdb tcp::1234 的快捷方式，表示启动 GDB 远程调试服务器，监听 TCP 端口 1234
# -S：表示 Qemu 启动后立即停止 CPU 运行，等待 GDB 连接后才能继续执行。
```

```
gdb-multiarch
target remote:1234
b *0x7c00
```