// Filename:
//		ccnx_check.c
// Program:
// 		Check the repository
// History:
//		20120601	Kuan-Hao		First release

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void usage() {
	printf("./check directory\n");

	return;
}

int main(int argc, char **argv) {
	char shell[100];
	char slice_name[20];
	char dir_name[20];
	char group_key[20];
	FILE *fp;
	int found = 0;

	// Wrong command
	if (argc < 2) {
		usage();

		return 0;
	}

	fp = fopen("binding.txt", "r");

	while (fscanf(fp, "%s %s %s", dir_name, slice_name, group_key) != EOF) {
		if (strcmp(argv[1], dir_name) == 0) {
			printf("dir_name = %s\n", dir_name);
			printf("slice_name = %s\n", slice_name);
			printf("group_key = %s\n\n", group_key);

			found = 1;

			break;
		}
	}

	// Cannot find the directory
	if (found == 0) {
		printf("Directory not found\n");

		return 0;
	}

	// Get the file list from the repository
	strcpy(shell, "ccnls /");
	strcat(shell, argv[1]);
	strcat(shell, " >> ");
	strcat(shell, dir_name);
	strcat(shell, ".log.txt");

	printf("%s\n", shell);

	system(shell);

	return 0;
}
