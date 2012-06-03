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

void check_repo_log(char * dir) {
	FILE * fp_repo;
	FILE * fp_local;
	char file_name[20];
	char user_name[20];
	char seq_no[20];

	fp_repo=fopen("","r");
	fp_local=fopen("","r");
	while (scanf(fp_repo, "%s %s %s", file_name, user_name, seq_no) != EOF)
	{

	}
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

int main(int argc, char const *argv[]) {
	// while (1) {
		update_local_log();
		download_repo_log();
		check_repo_log();
	// }
	
	return 0;
}
