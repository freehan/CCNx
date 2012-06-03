// Filename:
//		check_local_log.c
// Program:
// 		Check if any files have been modified
// History:
//		20120602	Kuan-Hao		First release

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#include "check_local_log.h"

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
	FILE* output;
	char filename[20];
	char timeTmp[15];
	char seq[4];
	char filepath_01[30];
	char filepath_02[30];
	char cmdTmp[60];

	strcpy(filepath_01, path);
	strcat(filepath_01, "/.locallog");

	strcpy(filepath_02, filepath_01);
	strcat(filepath_02, "Tmp");

	// Generate the local log file
	fp = fopen(filepath_01, "r");
	output = fopen(filepath_02, "w");

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
				if (strcmp(timeTmp, timestamp) == 0) {
					fprintf(output, "%s\t%s\t%s\n", filename, timeTmp, seq);
				} else {
					// The file has been modified
					fprintf(output, "%s\t%s\t%03d\n", filename, timestamp, atoi(seq) + 1);
				}

				break;
			}
		}
	}

	closedir(srcdir);

	// Close the log file
	fclose(fp);
	fclose(output);

	strcpy(cmdTmp, "rm ");
	strcat(cmdTmp, filepath_01);
	system(cmdTmp);
	strcpy(cmdTmp, "mv ");
	strcat(cmdTmp, filepath_02);
	strcat(cmdTmp, " ");
	strcat(cmdTmp, filepath_01);
	system(cmdTmp);

	return;
}

int main(int argc, char const *argv[]) {
	generate_log_file(".");
	compare_log_file(".");
	
	return 0;
}