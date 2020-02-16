#  网上的相关资料整理


### [如何将vdi文件转换成原始格式的磁盘镜像文件](https://superuser.com/questions/241269/exporting-a-virtualbox-vdi-to-a-harddrive-to-boot-it-natively)

    VBoxManage clonemedium --format RAW debian.vdi debian.img

### 反汇编

    objdump -l lan_os > disasm.txt
    makefile 要加上 gcc -g

### [bios int 0x13](https://blog.csdn.net/zxl3901/article/details/50072539)

