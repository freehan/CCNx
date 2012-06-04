#include <stdio.h>

int main() {
	FILE* fp;
	fp = fopen("/Users/fyf513/Documents/111.txt", "r");
	if (fp == NULL) {
		printf("error\n");
	} else {
		printf("pass\n");
	}

	fclose(fp);
	return 0;
}
