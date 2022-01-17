#include <stdio.h>
#include <stdlib.h>

int m = 0;

void v() {
	char buffer[512];
	fgets(buffer, 512, stdin);
	printf(buffer);
	if (m == 64) {
		fwrite("Wait what?!\n", 12, 1, stdout);
		system("/bin/sh");
	}
}

int main() {
	v();
}
