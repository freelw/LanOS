#ifdef SCHED_H
#define SCHED_H

typedef struct desc_struct {
	unsigned long a,b;
} desc_table[256];

extern desc_table idt;


#endif