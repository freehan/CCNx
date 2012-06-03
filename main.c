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

void update_local_log(const char *path) {
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

void download_repo_log(char * dir_name, char * slice_name) {
	get_file(dir_name, ".repolog", slice_name);
}

//@param dir: the path of the folder to be synchronized
void check_repo_log(char *dir, char * slice)
{
	FILE * fp_repo;
	FILE * fp_local;
	char repo_file_name[50];
	char repo_user_name[50];
	char repo_seq_no[50];
	char local_file_name[50];
	char local_user_name[50];
	char local_seq_no[50];
	char repo_log[50];
	char local_log[50];


	strcpy(dir,"~/Documents/sync_folder"); //test

	strcpy(repo_log,dir);
	strcpy(local_log,dir);

	strcat(repo_log,"/.repolog");
	strcat(local_log,"/.locallog");


	fp_repo=fopen(repo_log,"r");
	fp_local=fopen(local_log,"r");
	while (fscanf(fp_repo, "%s %s %s", repo_file_name, repo_user_name, repo_seq_no) != EOF)
	{
		//is_found=0 if a file in repo cannot be found in local
		//        =1 otherwise
		int is_found=0;
		while (fscanf(fp_local, "%s %s %s", local_file_name, local_user_name, local_seq_no) != EOF)
		{
			if (strcmp(local_file_name, repo_file_name)==0)
			{
				is_found=1;
				break;
			}
		}
		if (is_found==0)
		{
			get_file(dir, repo_file_name, slice);
		}
		else if (atoi(local_seq_no) > atoi(repo_seq_no))
		{
			put_file(dir, local_file_name, slice);
		}
		else if (atoi(local_seq_no) < atoi(repo_seq_no))
		{
			get_file(dir, repo_file_name, slice);
		}
//		else  // local_seq_no == repo_seq_no ---> conflict resolving
//		{
//
//		}
	}
	fclose(fp_repo);
	fclose(fp_local);
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

void put_file(char * dir_name, char * file_name, char * slice_name) {
	// put_log;
	// put_file;
	char shell[100];
	strcpy(shell,"sh put_file.sh");
	strcat(shell," ");
	strcat(shell,slice_name);
	strcat(shell," ");
	strcat(shell,file_name);
	strcat(shell," ");
	strcat(shell,dir_name);
	printf("%s\n",shell);
	system(shell);

}

void get_file(char * dir_name, char * file_name, char * slice_name) {
	// get_file;
	char shell[100];
	strcpy(shell,"sh get_file.sh");
	strcat(shell," ");
	strcat(shell,slice_name);
	strcat(shell," ");
	strcat(shell,file_name);
	strcat(shell," ");
	strcat(shell,dir_name);
	printf("%s\n",shell);
	system(shell);
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
	char dir_name[50];
	char group_key[20];
	FILE *fp;
	int found = 0;

	fp = fopen("binding.txt", "r");
	if(fp == NULL)
	{
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
