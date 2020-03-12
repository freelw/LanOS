all:
	gcc -m32 -g -fno-stack-protector -fno-pie -c lansys.c
	ar -r liblansys.a lansys.o
clean:
	rm lansys.o liblansys.a
	