int main() {
	char buf1[67];
	char buf2[65];

	memset(buf1, 0, 67);
	memset(buf2, 0, 65);

	int fd = fopen("/home/user/end/.pass", "r");

	if (argc != 2 && fd == 0) {
		return -1;
	}

	fread(buf1, 1, 66, fd);
	buf1[67] = 0;

	int b = atoi(argv[1]);
	buf1[b] = 0;

	fread(buf2, 1, 65, fd);
	fclose(fd);

	if (strcmp(buf1, argv[1])) {
		execl("/bin/sh", "sh", NULL);
	}
}
