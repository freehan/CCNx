// Filename:
//		main.c
// Program:
// 		Implement a Dropbox-Style Tool
// History:
//		20120602	Kuan-Hao		First release


// ------------------------------------------------------------
// Log Filename Format

// .locallog
//		Filename	Username	Time	Seq#
//		file01.txt	yifei		10001	001
//		file02.txt	minhan		10020	002

// .repolog
//		Filename	Username	Seq#
//		file01.txt	yifei		001
//		file02.txt	minhan		002
// ------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void update_local_log() {
	/*
	if (modified) {
		// Update time_stamp
		// Seq # + 1
	}
	*/
}

void download_repo_log() {
	

}

void check_repo_log() {

	/*
	if (local_new) {
		// Local Seq # > Repo Seq #
		put_file();
	} else if (repo_new) {
		// Repo Seq # > Local Seq #
		get_file();
	} else {
		// Repo Seq # = Local Seq #
		// Check user
		if (user is the same as me) {
			// Do nothing
		} else {
			// Conflict

		}
	}
	*/

}

void put_file() {
	// put_log;
	// put_file;
}

void get_file() {
	// get_file;
}
void usage()
{
	printf("cmd directory\n");
	exit(1);
}

int main(int argc, char const *argv[]) {
	if(argc != 2)
		usage();

	char shell[100];
	char slice_name[20];
	char dir_name[20];
	char group_key[20];
	FILE *fp;
	int found = 0;

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
		printf("Sync Directory not found\n");
		return 0;
	}
	fclose(fp);

	// while (1) {
		update_local_log();
		download_repo_log();
		check_repo_log();
	// }
	
	return 0;
}
