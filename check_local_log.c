// Filename:
//		check_local_log.c
// Program:
// 		Check if any files have been modified
// History:
//		20120602	Kuan-Hao		First release

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

char timestamp[15];

void show_modified_time(char *filename) {
	struct stat b;
	if (!stat(filename, &b)) {
		strftime(timestamp, 100, "%Y%m%d%H%M%S", localtime( &b.st_mtime));
	} else {
		printf("Cannot display the time.\n");
	}

	return;
}

void generate_log_file(const char *path) {
	int file_count = 0;
	struct dirent* dent;
	struct stat st;
	DIR* srcdir = opendir(path);
	FILE* fp;

	// Generate the local log file
	fp = fopen(".locallog", "w");

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
			fprintf(fp, "%s\t%s\t001\n", dent->d_name, timestamp);  
		}
	}

	closedir(srcdir);

	// Close the log file
	fclose(fp);

	return ;
}

void compare_log_file(const char *path) {
	int file_count = 0;
	struct dirent* dent;
	struct stat st;
	DIR* srcdir = opendir(path);
	FILE* fp;
	char filename[20];
	char timeTmp[15];
	char seq[4];
	char filepath[30];

	strcpy(filepath, path);
	strcat(filepath, "/.locallog");
	printf("path = %s\n", filepath);

	// Generate the local log file
	fp = fopen(filepath, "r");

	while((dent = readdir(srcdir)) != NULL) { 
		// Not the files we need
		if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) {
			continue;
		}

		lstat(dent->d_name, &st);

		// Check if it is a regular file
		if (S_ISREG(st.st_mode)) {
			show_modified_time(dent->d_name);
			rewind(fp);
			while ((fscanf(fp, "%s %s %s", filename, timeTmp, seq)) != EOF) {
				if (strcmp(filename, dent->d_name) != 0) {
					continue;
				}
				show_modified_time(dent->d_name);

				// Compare the timestamp
				// printf("file = %s\n", filename);
				// printf("log: %s, file: %s, strcmp: %d\n", timeTmp, timestamp,
								// strcmp(timeTmp, timestamp));

				break;
			}
		}
	}

	closedir(srcdir);

	// Close the log file
	fclose(fp);

	return;
}

int main(int argc, char const *argv[]) {
	generate_log_file(".");
	compare_log_file(".");
	
	return 0;
}