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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

char timestamp[15];

void show_modified_time(char *filename) {
	struct stat b;
	if (!stat(filename, &b)) {
		strftime(timestamp, 100, "%Y%m%d%H%M%S", localtime(&b.st_mtime));
	} else {
		printf("Cannot display the time.\n");

		exit(1);
	}

	return;
}

void generate_log_file(const char *path) {
	int file_count = 0;
	struct dirent* dent;
	struct stat st;
	DIR* srcdir = opendir(path);
	FILE* fp;
	char filepath[50];
	char *username;

	// Get username
	username = getenv("USER");

	// Generate the local log file
	strcpy(filepath, path);
	strcat(filepath, "/.locallog");
	fp = fopen(filepath, "w");

	while((dent = readdir(srcdir)) != NULL) { 
		// Not the files we need
		if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) {
			continue;
		}

		lstat(dent->d_name, &st);

		// Check if it is a regular file
		if (S_ISREG(st.st_mode)) {
			show_modified_time(dent->d_name);
			// Save the filename, timestamp and seq # into the log file
			fprintf(fp, "%s\t%s\t%s\t001\n", dent->d_name, username, timestamp);  
		}
	}

	closedir(srcdir);

	// Close the log file
	fclose(fp);

	return ;
}

// Show the usage
void usage() {
	printf("./init [-d directory] [-s Slice_Prefix] [-k Group_Key]\n");

	exit(1);
}

int main(int argc, char **argv) {
	int opt;
	char shell[100];
	char slice_name[20];
	char dir_name[50];
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
	generate_log_file(dir_name);

	return 0;
}
