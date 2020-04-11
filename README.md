# LanOS -- 梳理linux0.12知识点系列

	这个工程的目标是梳理《Linux源码剖析》的知识点并从0.12版本的源码中抠出部分代码构建一个
	能用现代linux发行版方便编译的迷你系统，从而验证各个知识点

## 引用资源

* [《Linux源码剖析》最新的电子版 ](http://www.oldlinux.org/download/CLK-5.0-WithCover.pdf)
* [ linux 0.1x源码 ](http://oldlinux.org/Linux.old/kernel/0.1x/)
* [ oldlinux官网 ](http://www.oldlinux.org/)
* 《ORANGE'S:一个操作系统的实现》

感谢赵炯博士，感谢于渊老师，感谢川合秀实先生，并致以最高的敬意！

## 本教程的特点

* 大量参考linux0.12的设计思路建立demo，有助于之后阅读linux源码
* 将尽可能多的linux中汇编实现的代码用c重新实现，提高可读性，降低学习成本
* 使用docker编译，使得编译环境极易搭建

## **超快速编译方法（依赖docker & 联网）**
### 执行以下命令拉取代码并编译(首次执行make_os.sh会比较慢，因为要拉取docker编译镜像)

	git clone https://github.com/freelw/LanOS.git
	cd LanOS
	./make_os.sh

### 执行效果

	dantadeMacBook-Pro:yard danta$ git clone https://github.com/freelw/LanOS.git
	Cloning into 'LanOS'...
	remote: Enumerating objects: 517, done.
	remote: Counting objects: 100% (517/517), done.
	remote: Compressing objects: 100% (322/322), done.
	remote: Total 1316 (delta 304), reused 379 (delta 181), pack-reused 799
	Receiving objects: 100% (1316/1316), 2.78 MiB | 42.00 KiB/s, done.
	Resolving deltas: 100% (788/788), done.
	dantadeMacBook-Pro:yard danta$ cd LanOS
	dantadeMacBook-Pro:LanOS danta$ ./make_os.sh
	builing lan os...
	cleaning temp files...
	file [a.vfd] is our os image with a simple ramdisk filesystem.

## 知识点目录

* [如何编译和运行demo](https://github.com/freelw/LanOS/blob/master/doc/how_to_build.md)
* [梳理linux0.12知识点系列 之 让汇编到C不再遥远](https://github.com/freelw/LanOS/tree/master/demos/protect_mode_demo)
* [梳理linux0.12知识点系列 之 a20地址线](https://github.com/freelw/LanOS/tree/master/demos/a20_open)
* [梳理linux0.12知识点系列 之 8259A的初始化和时钟中断](https://github.com/freelw/LanOS/tree/master/demos/init_8259a)
* [8259A原理详细说明](https://blog.csdn.net/LiWang112358/article/details/103875658)
* [网上相关资料整理](https://github.com/freelw/LanOS/blob/master/doc/questions.md)

## 血与泪
* [跳转到用户态](https://github.com/freelw/LanOS/blob/master/demos/user_mode_demo/readme.md)
* [初步调试fork](https://github.com/freelw/LanOS/blob/master/demos/fork_demo/readme.md)
* [fork+写时复制](https://github.com/freelw/LanOS/blob/master/demos/cp_on_write/readme.md)
* [关于全局变量](https://github.com/freelw/LanOS/blob/master/demos/exit_demo/readme.md)

## Todo
1. ~~print_str 实现~~ done 20200224
2. ~~print_num 实现~~ done 20200224
3. ~~lan_fs 实现~~ done 20200224
4. ~~exec 实现~~ done 20200224
5. ~~exit 实现~~ done 20200304

## 最终效果展示

### 有简单的shell和文件系统，可以执行ls程序
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/ls.gif)
### 支持exec系统调用，可以执行应用程序
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/exec.gif)
### 在一个可执行程序运行的过程中，执行另一个程序，说明支持多任务调度
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/multitask.gif)
### loop程序的实现
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/loop.png)

[详见这里](https://github.com/freelw/LanOS/blob/master/demos/exit_demo/bin/loop/lan_main.c)
### 最终demo链接
[详见这里](https://github.com/freelw/LanOS/tree/master/demos/common_header_demo)

### 历史demo顺序
1. [asm_int_80](https://github.com/freelw/LanOS/tree/master/demos/asm_int_80)
2. [asm_call_c](https://github.com/freelw/LanOS/tree/master/demos/asm_call_c)
3. [lds_test](https://github.com/freelw/LanOS/tree/master/demos/lds_test)
4. [protect_mode_demo](https://github.com/freelw/LanOS/tree/master/demos/protect_mode_demo)
5. [protect_mode_cpp_demo](https://github.com/freelw/LanOS/tree/master/demos/protect_mode_cpp_demo)
6. [a20_not_open](https://github.com/freelw/LanOS/tree/master/demos/a20_not_open)
7. [a20_open](https://github.com/freelw/LanOS/tree/master/demos/a20_open)
8. [mm](https://github.com/freelw/LanOS/tree/master/demos/mm)
9. [user_mode_demo](https://github.com/freelw/LanOS/tree/master/demos/user_mode_demo)
10. [fork_demo](https://github.com/freelw/LanOS/tree/master/demos/fork_demo)
11. [cp_on_write](https://github.com/freelw/LanOS/tree/master/demos/cp_on_write)
12. [print_str_demo](https://github.com/freelw/LanOS/tree/master/demos/print_str_demo)
13. [fs_demo](https://github.com/freelw/LanOS/tree/master/demos/fs_demo)
14. [exec_demo](https://github.com/freelw/LanOS/tree/master/demos/exec_demo)
15. [shell_demo](https://github.com/freelw/LanOS/tree/master/demos/shell_demo)
16. [ls_demo](https://github.com/freelw/LanOS/tree/master/demos/ls_demo)
17. [exit_demo](https://github.com/freelw/LanOS/tree/master/demos/exit_demo)
18. [common_header_demo](https://github.com/freelw/LanOS/tree/master/demos/common_header_demo)

# b站文案

1. [01 目标 & 准备](https://github.com/freelw/LanOS/blob/master/doc/target_and_prepare.md)
2. [02 两个方向的对比](https://github.com/freelw/LanOS/blob/master/doc/to_direction_compare.md)
3. [03 控制二进制文件](https://github.com/freelw/LanOS/blob/master/doc/control_binary.md)
4. [04 保护模式速成](https://github.com/freelw/LanOS/blob/master/doc/quick_protect_mode.md)