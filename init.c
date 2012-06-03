// Filename:
//		init.c
// Program:
// 		Create a directory and a slice. Generate a "binding.txt" which
//		contains the information of slice name, directory name and group key 
//		and .locallog which contains the file list. 
//		Basically it uses the "create.sh".
// History:
//		20120602	Kuan-Hao		First release

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "check_local_log.c"

// Show the usage
void usage() {
	printf("./init [-d directory] [-s Slice_Prefix] [-k Group_Key]\n");

	return;
}

int main(int argc, char **argv) {
	int opt;
	char shell[100];
	char slice_name[20];
	char dir_name[20];
	char key[20];
	FILE *fp;

	strcpy(shell, "sh create.sh ");

	if(argc != 7)
		usage();

	while((opt = getopt(argc, argv,"d:s:k:")) != -1) {
		switch(opt) {
			case 'd':
				// Directory name
				printf("directory = %s\n", optarg);
				strcpy(dir_name, optarg);
				break;
			case 's':
				// Slice name
				printf("slice_name = %s\n", optarg);
				strcpy(slice_name, optarg);
				break;
			case 'k':
				// Group key
				printf("group key = %s\n", optarg);
				strcpy(key, optarg);
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
	fprintf(fp, "%s\n\n", key);
	fclose(fp);

	strcat(shell, dir_name);
	strcat(shell, " ");
	strcat(shell, slice_name);

	// Execute the shell script
	// It creates a folder and a slice
	printf("%s\n", shell);
	system(shell);

	// Create the local log file
	check_local_log(dir_name);

	return 0;
}
