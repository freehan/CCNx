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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


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
}

int main(int argc, char const *argv[]) {
	if(argc != 2)
		usage();

	char *dir = malloc((strlen(argv[1])+1)*sizeof(char));
	strcpy(dir, argv[1]);

	// while (1) {
		update_local_log();
		download_repo_log();
		check_repo_log();
	// }
	
	return 0;
}
