#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	int		val;
	char	buffer[40]; // 0x3c - 0x14

	val = atoi(argv[1]);

	if (val > 9) {
		return 1;
	}

	memcpy(buffer, argv[2], val << 2);

	if (val == 0x574f4c46) {
		execl("/bin/sh", "sh", NULL);
	}
	return 0;
}
