// Filename:
//		ccnx_create.c
// Program:
// 		Create a directory and a slice
// History:
//		20120601	Kuan-Hao		First release

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Show the usage
void usage() {
	printf("./create directory [-s Slice_Prefix] [-k Group_Key]\n");

	return;
}

int main(int argc, char **argv) {
	int opt;
	char shell[100];
	char slice_name[20];
	char dir_name[20];
	FILE *fp;

	strcpy(shell, "sh create.sh ");

	while((opt = getopt(argc, argv,"s:k:")) != -1) {
		switch(opt) {
			case 's':
				// Slice name
				printf("slice_name = %s\n", optarg);
				strcpy(slice_name, optarg);
				break;
			case 'k':
				// Directory name
				printf("dir_name = %s\n", optarg);
				strcpy(dir_name, optarg);
				break;
			default:
				// Show the usage
				usage();
				return 0;
				break;
		}
	}

	// Output the binding
	fp = fopen("binding.txt", "a");
	fprintf(fp, "%s\n", dir_name);
	fprintf(fp, "%s\n", slice_name);
	fprintf(fp, "%s\n\n", slice_name);
	fclose(fp);

	strcat(shell, dir_name);
	strcat(shell, " ");
	strcat(shell, slice_name);

	// Execute the shell script
	// It creates a folder and a slice
	printf("%s\n", shell);
	system(shell);

	return 0;
}
