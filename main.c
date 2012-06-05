// Filename:
//		main.c
// Program:
// 		Implement a Dropbox-Style Tool
// History:
//		20120602	Kuan-Hao		First release

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>

char timestamp[15];

void show_modified_time(char* filepath, char* filename) {
	struct stat b;
	char path[50];

	strcpy(path, filepath);
	strcat(path, "/");
	strcat(path, filename);

	if (!stat(path, &b)) {
		strftime(timestamp, 100, "%Y%m%s%H%M%S", localtime(&b.st_mtime));
	} else {
		printf("Cannot display the time.\n");

		exit(1);
	}

	// printf("Check time: %s -> %s\n", path, timestamp);

	return;
}

void update_local_log(const char *path) {
	int file_count = 0;
	struct dirent* dent;
	struct stat st;
	DIR* srcdir = opendir(path);
	FILE* fp_input;
	FILE* fp_tmp;
	FILE* fp_output;
	char filename[20];
	char filenameTmp[20];
	char user[20];
	char timeTmp[15];
	char seq[4];
	char filepath_01[100];
	char filepath_02[100];
	char filepath_03[100];
	char cmdTmp[200];
	int found = 0;
	char* username;
	char delFlag[2];
	char stringTmp[200];

	// Get username
	username = getenv("USER");

	strcpy(filepath_01, path);
	strcat(filepath_01, "/.locallog");

	strcpy(filepath_02, filepath_01);
	strcat(filepath_02, "Tmp");

	strcpy(filepath_03, filepath_01);
	strcat(filepath_03, "Output");

	printf("Check local log file:\n");
	printf("\tfilepath_01: %s\n", filepath_01);
	printf("\tfilepath_02: %s\n", filepath_02);
	printf("\tfilepath_03: %s\n", filepath_03);

	// Create the local log file
	while ((dent = readdir(srcdir)) != NULL) {
		found = 0;
		if (strcmp(dent->d_name, ".locallog") == 0) {
			found = 1;
			break;
		}
	}
	if (found == 0) {
		fopen(filepath_01, "w");
		printf("create locallog\n");
	}
	fp_input = fopen(filepath_01, "r");
	fp_tmp = fopen(filepath_02, "w");
	fp_output = fopen(filepath_03, "w");

	while ((dent = readdir(srcdir)) != NULL) {
		found = 0;

		// Not the files we need
		if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) {
			continue;
		}

		if (strcmp(dent->d_name, ".locallog") == 0
				|| strcmp(dent->d_name, ".locallogTmp") == 0
				|| strcmp(dent->d_name, ".locallogOutput") == 0
				|| strcmp(dent->d_name, ".repolog") == 0
				|| strcmp(dent->d_name, ".DS_Store") == 0) {
			continue;
		}
		lstat(dent->d_name, &st);

		// Check if it is a regular file
		// if (S_ISREG(st.st_mode)) {
		show_modified_time(path, dent->d_name);
		rewind(fp_input);
		while ((fscanf(fp_input, "%s %s %s %s %s", filename, user, timeTmp, seq,
				delFlag)) != EOF) {
			if (strcmp(filename, dent->d_name) != 0) {
				continue;
			}

			found = 1;

			// Compare the timestamp
			if (strcmp(timeTmp, timestamp) == 0) {
				fprintf(fp_tmp, "%s\t%s\t%s\t%s\t%s\n", filename, username,
						timeTmp, seq, delFlag);
			} else {
				// The file has been modified
				printf("\"%s\" has been modified\n", filename);
				fprintf(fp_tmp, "%s\t%s\t%s\t%03d\t%s\n", filename, username,
						timestamp, atoi(seq) + 1, delFlag);
			}
			break;
		}

		if (found == 0) {
			printf("Add a new file: %s \n", dent->d_name);
			fprintf(fp_tmp, "%s\t%s\t%s\t001\t0\n", dent->d_name, username,
					timestamp);
			fprintf(fp_output, "%s\t%s\t%s\t001\t0\n", dent->d_name, username,
					timestamp);
		}
	}

	closedir(srcdir);

	// Check if any file has been deleted
	fclose(fp_tmp);
	fp_tmp = fopen(filepath_02, "r");
	rewind(fp_input);
	while ((fscanf(fp_input, "%s %s %s %s %s", filename, user, timeTmp, seq,
			delFlag)) != EOF) {
		strcpy(stringTmp, filename);
		strcat(stringTmp, "\t");
		strcat(stringTmp, user);
		strcat(stringTmp, "\t");
		strcat(stringTmp, timeTmp);
		strcat(stringTmp, "\t");
		strcat(stringTmp, seq);
		strcat(stringTmp, "\t");
		strcat(stringTmp, "1");

		found = 0;
		while ((fscanf(fp_tmp, "%s %s %s %s %s", filenameTmp, user, timeTmp,
				seq, delFlag)) != EOF) {
			if (strcmp(filename, filenameTmp) == 0) {
				fprintf(fp_output, "%s\t%s\t%s\t%s\t%s\n", filename, username,
						timeTmp, seq, delFlag);
				found = 1;
				break;
			}
		}

		if (found == 0) {
			// Delete a file
			printf("Delete a file: %s\n", filename);
			printf("stringTmp = %s\n", stringTmp);
			fprintf(fp_output, "%s\n", stringTmp);
		}
	}
	// Close the log file
	fclose(fp_input);
	fclose(fp_tmp);
	fclose(fp_output);

	strcpy(cmdTmp, "rm ");
	strcat(cmdTmp, filepath_01);
	system(cmdTmp);

	strcpy(cmdTmp, "rm ");
	strcat(cmdTmp, filepath_02);
	system(cmdTmp);

	strcpy(cmdTmp, "mv ");
	strcat(cmdTmp, filepath_03);
	strcat(cmdTmp, " ");
	strcat(cmdTmp, filepath_01);
	system(cmdTmp);
	//test
	fp_output = fopen(filepath_01, "r");
	while ((fscanf(fp_output, "%s %s %s %s %s", filenameTmp, user, timeTmp, seq,
			delFlag)) != EOF) {
		printf("local log before comparing:  %s\t%s\t%s\t%s\t%s\n", filenameTmp,
				user, timeTmp, seq, delFlag);
	}
	fclose(fp_output);
	//
	return;
}

void download_repo_log(char * dir_name, char * slice_name) {
	get_file(dir_name, ".repolog", slice_name);
}

//@param dir: the path of the folder to be synchronized
void check_repo_log(char *dir, char * slice) {
	FILE * fp_repo;
	FILE * fp_local;
	FILE * fp_new_repo;
	FILE * fp_new_local;

	char repo_file_name[50];
	char repo_user_name[50];
	char repo_seq_no[50];
	char local_file_name[50];
	char local_user_name[50];
	char local_time_stamp[50];
	char local_seq_no[50];
	char repo_log[50];
	char local_log[50];
	int local_is_delete;
	int repo_is_delete; //if the file at the other side is deleted then set this flag as 1
						//Note: ccnr currently doesn't support delete operation
						//the deleted file is actually present in repo, the corresponding flag is set as 1
						//we acutally delete the file in the file system by reading the this flag

	char new_repo_log[50];
	char new_local_log[50];
	//strcpy(dir,"~/Documents/sync_folder"); //test

	strcpy(repo_log, dir);
	strcpy(local_log, dir);
	strcpy(new_repo_log, dir);
	strcpy(new_local_log, dir);

	strcat(repo_log, "/.repolog");
	strcat(local_log, "/.locallog");
	strcat(new_repo_log, "/.repologtmp");
	strcat(new_local_log, "/.locallogtmp");

	fp_repo = fopen(repo_log, "r");
	fp_local = fopen(local_log, "r");
	fp_new_repo = fopen(new_repo_log, "w");
	fp_new_local = fopen(new_local_log, "w");

	while (fscanf(fp_repo, "%s %s %s %d", repo_file_name, repo_user_name,
			repo_seq_no, &repo_is_delete) != EOF) {
		printf("repo log:  %s\t%s\t%s\t%d\n", repo_file_name, repo_user_name,
				repo_seq_no, repo_is_delete);
		//is_found=0 if a file in repo cannot be found in local
		//        =1 otherwise
		int is_found = 0;
		rewind(fp_local);
		while (fscanf(fp_local, "%s %s %s %s %d", local_file_name,
				local_user_name, local_time_stamp, local_seq_no,
				&local_is_delete) != EOF) {
			if (strcmp(local_file_name, repo_file_name) == 0) {
				is_found = 1;
				break;
			}
		}
		if (is_found == 0) {
			get_file(dir, repo_file_name, slice);

			show_modified_time(dir, repo_file_name);
			fprintf(fp_new_local, "%s\t%s\t%s\t%s\t%d\n", repo_file_name,
					repo_user_name, timestamp, repo_seq_no, repo_is_delete);
			fprintf(fp_new_repo, "%s\t%s\t%s\t%d\n", repo_file_name,
					repo_user_name, repo_seq_no, repo_is_delete);
		}
		//if repo_is_delete==1, it means that the file named repo_file_name is deleted on other file systems
		//, thus it should be deleted in the local file system
		else if (repo_is_delete == 1) {
			printf("file %s is found \n", repo_file_name);
			printf("delete %s/%s\n", dir, repo_file_name);
			delete_file(dir, repo_file_name);
			fprintf(fp_new_local, "%s\t%s\t%s\t%s\t%d\n", repo_file_name,
					repo_user_name, timestamp, repo_seq_no, repo_is_delete);
			fprintf(fp_new_repo, "%s\t%s\t%s\t%d\n", repo_file_name,
					repo_user_name, repo_seq_no, repo_is_delete);
		} else if (atoi(local_seq_no) > atoi(repo_seq_no)) {
			put_file(dir, local_file_name, slice);

			show_modified_time(dir, local_file_name);
			fprintf(fp_new_local, "%s\t%s\t%s\t%s\t%d\n", local_file_name,
					local_user_name, timestamp, local_seq_no, local_is_delete);
			fprintf(fp_new_repo, "%s\t%s\t%s\t%d\n", local_file_name,
					local_user_name, local_seq_no, local_is_delete);
		} else if (atoi(local_seq_no) < atoi(repo_seq_no)) {
			get_file(dir, repo_file_name, slice);

			show_modified_time(dir, repo_file_name);
			fprintf(fp_new_local, "%s\t%s\t%s\t%s\t%d\n", repo_file_name,
					repo_user_name, timestamp, repo_seq_no, repo_is_delete);
			fprintf(fp_new_repo, "%s\t%s\t%s\t%d\n", repo_file_name,
					repo_user_name, repo_seq_no, repo_is_delete);
		}
		else
		{
			fprintf(fp_new_local, "%s\t%s\t%s\t%s\t%d\n", local_file_name,
					local_user_name, timestamp, local_seq_no, local_is_delete);
			fprintf(fp_new_repo, "%s\t%s\t%s\t%d\n", local_file_name,
					local_user_name, local_seq_no, local_is_delete);
		}
	}

	rewind(fp_local);
	while (fscanf(fp_local, "%s %s %s %s %d", local_file_name, local_user_name,
			local_time_stamp, local_seq_no, &local_is_delete) != EOF) {
		printf("local log:  %s\t%s\t%s\t%s\t%d\n", local_file_name,
				local_user_name, local_time_stamp, local_seq_no,
				local_is_delete);
		//is_found=0 if a file in repo cannot be found in local
		//        =1 otherwise
		int is_found = 0;
		rewind(fp_repo);
		while (fscanf(fp_repo, "%s %s %s %d", repo_file_name, repo_user_name,
				repo_seq_no, &repo_is_delete) != EOF) {
			if (strcmp(local_file_name, repo_file_name) == 0) {
				is_found = 1;
				break;
			}
		}

		if (is_found == 0) {
			put_file(dir, local_file_name, slice);

			show_modified_time(dir, local_file_name);
			fprintf(fp_new_local, "%s\t%s\t%s\t%s\t%d\n", local_file_name,
					local_user_name, timestamp, local_seq_no, local_is_delete);
			fprintf(fp_new_repo, "%s\t%s\t%s\t%d\n", local_file_name,
					local_user_name, local_seq_no, local_is_delete);
		}
	}

	fclose(fp_repo);
	fclose(fp_local);
	fclose(fp_new_repo);
	fclose(fp_new_local);

	char cmdTmp[200];
	//remove the temp repolog and mv to locallog
	strcpy(cmdTmp, "rm ");
	strcat(cmdTmp, repo_log);
	system(cmdTmp);

	strcpy(cmdTmp, "mv ");
	strcat(cmdTmp, new_repo_log);
	strcat(cmdTmp, " ");
	strcat(cmdTmp, repo_log);
	system(cmdTmp);

	strcpy(cmdTmp, "rm ");
	strcat(cmdTmp, local_log);
	system(cmdTmp);

	strcpy(cmdTmp, "mv ");
	strcat(cmdTmp, new_local_log);
	strcat(cmdTmp, " ");
	strcat(cmdTmp, local_log);
	system(cmdTmp);

	//synchronize local repolog to repository repolog
	put_file(dir, ".repolog", slice);

	return;
}

void put_file(char * dir_name, char * file_name, char * slice_name) {
	// put_log;
	// put_file;
	char shell[100];
	strcpy(shell, "sh put_file.sh");
	strcat(shell, " ");
	strcat(shell, slice_name);
	strcat(shell, " ");
	strcat(shell, file_name);
	strcat(shell, " ");
	strcat(shell, dir_name);
	printf("%s\n", shell);
	system(shell);

}

void get_file(char * dir_name, char * file_name, char * slice_name) {
	// get_file;
	char shell[100];
	strcpy(shell, "sh get_file.sh");
	strcat(shell, " ");
	strcat(shell, slice_name);
	strcat(shell, " ");
	strcat(shell, file_name);
	strcat(shell, " ");
	strcat(shell, dir_name);
	printf("%s\n", shell);
	system(shell);
}

void delete_file(char *dir_name, char * file_name) {
	char shell[100];
	strcpy(shell, "sh delete_file.sh");
	strcat(shell, " ");
	strcat(shell, dir_name);
	strcat(shell, " ");
	strcat(shell, file_name);
	printf("%s\n", shell);
	system(shell);
}

void usage() {
	printf("cmd directory\n");
	exit(1);
}

int main(int argc, char const *argv[]) {
	if (argc != 2) {
		usage();
	}

	char shell[100];
	char slice_name[20];
	char dir_name[50];
	char group_key[20];
	FILE *fp;
	int found = 0;

	fp = fopen(".binding", "r");
	if (fp == NULL) {
		printf("cannot find binding.txt\n");
		exit(1);
	}
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
	update_local_log(dir_name);
	download_repo_log(dir_name, slice_name);
	check_repo_log(dir_name, slice_name);
	// }

	printf("success!!!!!!!!!!\n");

	return 0;
}
