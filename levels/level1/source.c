#include <stdio.h>
#include <stdlib.h>

void run(void) {
	fwrite("Good... Wait what?\n", 19, 1, stdout);
	system("/bin/sh");
}

int	main(void) {
	char buffer[64];   //0x50 - 0x10 = 64
	gets(buffer);
}
