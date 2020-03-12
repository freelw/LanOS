# 梳理linux0.12知识点系列 
# 之
# A20地址线的打开

## 背景

关于A20地址线的历史问题维基百科上有[详细的描述](https://zh.wikipedia.org/wiki/A20%E6%80%BB%E7%BA%BF)
总体来说就是如果不开启A20地址线我们只能访问1M内存

也就是说这时能访问的最大内存地址是0xfffff

当我们想要访问0x100000的时候，cpu帮我们环绕到0x0处

## linux 0.12的打开和检查方案

1. linux 0.12在setup.S中进行了打开A20地址线的操作

		...
		!144行开始
		call	empty_8042
		mov	al,#0xD1		! command write
		out	#0x64,al
		call	empty_8042
		mov	al,#0xDF		! A20 on
		out	#0x60,al
		call	empty_8042
		...
		!204行开始
		empty_8042:
			.word	0x00eb,0x00eb
			in	al,#0x64	! 8042 status port
			test	al,#2		! is input buffer full?
			jnz	empty_8042	! yes - loop
			ret
		
2. 在head.s中进行了检查操作，如果不能通过检查，则进入死循环

		...
		!33行开始
		1:	incl %eax		# check that A20 really IS enabled
			movl %eax,0x000000	# loop forever if it isn't
			cmpl %eax,0x100000
			je 1b

思路很清晰
	
* 首先使用8042键盘控制器的功能打开A20地址线
* 然后不断的累加eax，放到0x0地址处，更0x100000地址处的值比较，如果相同就一致循环下去，如果不同说明不是环绕的地址，也就是可以访问1M以上的内存了，则退出循环
* 所以上面的这个检查的逻辑可以很好的用c语言来表示


		void check_a20_valid()
		{
		    unsigned char *p0 = 0x0;
		    unsigned char *p1 = 0x100000;
		    //这里指循环了10次
			for (unsigned char i = 0; i < 10; ++ i) {
				*p0 = i;   
				if (*p0 != *p1) {
					write_char('b');
					break;
				} else {
					write_char('e');
				}
			}
		}
	
	具体代码见[这里](https://github.com/freelw/LanOS/tree/master/demos/a20_open)

* 而在我们的head.s中，实现一个open_a20的函数，将书中的代码抄过来
		
		open_a20:
		    call empty_8042
		    mov al, 0xd1
		    out 0x64, al
		    call empty_8042
		    mov al, 0xdf
		    out 0x60, al
		    call empty_8042
		    ret
		
		empty_8042:
		    dw 0x00eb
		    dw 0x00eb
		    in al, 0x64
		    test al, 0x2
		    jnz empty_8042
		    ret
	
* 在lan_main.c中调用open_a20

		extern void write_char(char ch);
		extern void open_a20();
		
		void check_a20_valid()
		{
		    unsigned char *p0 = 0x0;
		    unsigned char *p1 = 0x100000;
			for (unsigned char i = 0; i < 10; ++ i) {
				*p0 = i;   
				if (*p0 != *p1) {
					write_char('b');
					break;
				} else {
					write_char('e');
				}
			}
		}
		
		void lan_main()
		{
			write_char('L');
			write_char('O');
			write_char('V');
			write_char('E');
			open_a20();
			check_a20_valid();
			while(1);
		}

## 效果

* 在A20被打开时
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/opena20.png)
* 如果我们注视掉`open_a20();`这句话
![](https://raw.githubusercontent.com/freelw/LanOS/master/demos/pic/a20notopen.png)

			
	
    