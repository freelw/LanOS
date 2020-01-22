# 如何编译和运行demo


## 编译
### 依赖软件
* [docker](https://www.docker.com/)

### 编译步骤

1. clone 工程
	
		git clone https://github.com/freelw/LanOS.git

2. 加载`lan_os_rc`

		source lan_os_rc
		
3. 进入一个demo编译，这里以`protect_mode_demo`为例，执行`lan_make`首次执行编译会拉取编译镜像

		$ cd demos/protect_mode_demo/
		$ lan_make 
		Unable to find image 'freelw/lan_make:latest' locally
		latest: Pulling from freelw/lan_make
		5c939e3a4d10: Pull complete 
		c63719cdbe7a: Pull complete 
		19a861ea6baf: Pull complete 
		651c9d2d6c4f: Pull complete 
		7e23ae3a25f1: Pull complete 
		51bb6ba7ea1d: Pull complete 
		Digest: sha256:2b43e62a591836b22007c9daa6cf6cad2244d5f032390e676e8ab786b823e207
		Status: Downloaded newer image for freelw/lan_make:latest
		nasm -felf64 -o boot.o boot.s 
		gcc -c lan_main.c
		nasm -o loader.bin loader.s -l loader.lst
		ld -T n.lds -o lan_os boot.o lan_main.o
		dd if=loader.bin of=a.img bs=512 count=1 conv=notrunc
		1+0 records in
		1+0 records out
		512 bytes copied, 0.00183839 s, 279 kB/s
		dd if=lan_os of=a.img bs=512 count=17 skip=4096 seek=1 conv=notrunc
		8+1 records in
		8+1 records out
		4304 bytes (4.3 kB, 4.2 KiB) copied, 0.00605137 s, 711 kB/s
		head -c 1474560 /dev/zero > a.vfd
		dd if=a.img of=a.vfd bs=512 count=18 conv=notrunc
		9+1 records in
		9+1 records out
		4816 bytes (4.8 kB, 4.7 KiB) copied, 0.00466217 s, 1.0 MB/s

4.清理编译结果的方法，执行`lan_clean`，同样首次执行会拉取镜像

		$ lan_clean 
		Unable to find image 'freelw/lan_clean:latest' locally
		latest: Pulling from freelw/lan_clean
		5c939e3a4d10: Already exists 
		c63719cdbe7a: Already exists 
		19a861ea6baf: Already exists 
		651c9d2d6c4f: Already exists 
		a43ee7839f30: Pull complete 
		fb472c71d652: Pull complete 
		Digest: sha256:276b8860b910214803ee2c91a89bbb7692fd0135f2d538ac553e9d09b699ec90
		Status: Downloaded newer image for freelw/lan_clean:latest
		rm lan_os lan_main.o boot.o loader.bin loader.lst a.img a.vfd
		
## 运行
### 依赖软件
* [virtual box](https://www.virtualbox.org/)

### 运行方法
* 将编译生成的a.vfd文件作为一个floppy加入到虚拟机的存储中
* 分配多于16M的内存给虚拟机
* 无需创建硬盘
* 启动虚拟机