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
		strftime(timestamp, 100, "%Y%m%d%H%M%S", localtime(&b.st_mtime));
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
	FILE* fp;
	FILE* output;
	char filename[20];
	char user[20];
	char timeTmp[15];
	char seq[4];
	char filepath_01[50];
	char filepath_02[50];
	char cmdTmp[100];
	int found = 0;
	char* username;

	// Get username
	username = getenv("USER");

	strcpy(filepath_01, path);
	strcat(filepath_01, "/.locallog");

	strcpy(filepath_02, filepath_01);
	strcat(filepath_02, "Tmp");

	printf("Check local log file:\n");
	printf("\tfilepath_01: %s\n", filepath_01);
	printf("\tfilepath_02: %s\n", filepath_02);

	// Generate the local log file
	fp = fopen(filepath_01, "r");
	output = fopen(filepath_02, "w");

	while((dent = readdir(srcdir)) != NULL) { 
		found = 0;

		// Not the files we need
		if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0) {
			continue;
		}

		if(strcmp(dent->d_name, ".locallog") == 0 || 
			strcmp(dent->d_name, ".locallogTmp") == 0 ||
			strcmp(dent->d_name, ".repolog") == 0 ||
			strcmp(dent->d_name, ".DS_Store") == 0) {
			continue;
		}

		// printf("Check file: %s\n", dent->d_name);

		lstat(dent->d_name, &st);

		// Check if it is a regular file
		// if (S_ISREG(st.st_mode)) {
			show_modified_time(path, dent->d_name);
			rewind(fp);
			while ((fscanf(fp, "%s %s %s %s", filename, user, timeTmp, seq)) != EOF) {
				if (strcmp(filename, dent->d_name) != 0) {
					continue;
				}

				found = 1;

				// Compare the timestamp
				if (strcmp(timeTmp, timestamp) == 0) {
					fprintf(output, "%s\t%s\t%s\t%s\n",
							filename, username, timeTmp, seq);
				} else {
					// The file has been modified
					printf("\"%s\" has been modified\n", filename);
					fprintf(output, "%s\t%s\t%s\t%03d\n",
							filename, username, timestamp, atoi(seq) + 1);
				}

				break;
			}

			if (found == 0) {
				printf("Add a new file: %s \n", dent->d_name);
				fprintf(output, "%s\t%s\t%s\t001\n",
						dent->d_name, username, timestamp);
			}
		// }
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

	char new_repo_log[50];
	char new_local_log[50];
	//strcpy(dir,"~/Documents/sync_folder"); //test

	strcpy(repo_log,dir);
	strcpy(local_log,dir);
	strcpy(new_repo_log,dir);
	strcpy(new_local_log,dir);

	strcat(repo_log,"/.repolog");
	strcat(local_log,"/.locallog");
	strcat(new_repo_log,"/.repologtmp");
	strcat(new_local_log,"/.locallogtmp");

	fp_repo=fopen(repo_log,"r");
	fp_local=fopen(local_log,"r");
	fp_new_repo=fopen(new_repo_log,"w");
	fp_new_local=fopen(new_local_log,"w");

	while (fscanf(fp_repo, "%s %s %s", repo_file_name, repo_user_name, repo_seq_no) != EOF)
	{
		printf("repolog:  %s\t%s\t%s\n",repo_file_name, repo_user_name, repo_seq_no);
		//is_found=0 if a file in repo cannot be found in local
		//        =1 otherwise
		int is_found=0;
		rewind(fp_local);
		while (fscanf(fp_local, "%s %s %s %s", local_file_name, local_user_name, local_time_stamp, local_seq_no) != EOF)
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

			show_modified_time(dir,repo_file_name);
			fprintf(fp_new_local,"%s\t%s\t%s\t%s\n",repo_file_name, repo_user_name, timestamp, repo_seq_no);
			fprintf(fp_new_repo, "%s\t%s\t%s\n", repo_file_name, repo_user_name, repo_seq_no);
		}
		else if (atoi(local_seq_no) > atoi(repo_seq_no))
		{
			put_file(dir, local_file_name, slice);

			show_modified_time(dir,local_file_name);
			fprintf(fp_new_local,"%s\t%s\t%s\t%s\n",local_file_name, local_user_name, timestamp, local_seq_no);
			fprintf(fp_new_repo, "%s\t%s\t%s\n",local_file_name,local_user_name,local_seq_no);
		}
		else if (atoi(local_seq_no) < atoi(repo_seq_no))
		{
			get_file(dir, repo_file_name, slice);

			show_modified_time(dir,repo_file_name);
			fprintf(fp_new_local,"%s\t%s\t%s\t%s\n",repo_file_name, repo_user_name, timestamp, repo_seq_no);
			fprintf(fp_new_repo, "%s\t%s\t%s\n",repo_file_name,repo_user_name,repo_seq_no);
		}
//		else  // local_seq_no == repo_seq_no ---> conflict resolving
//		{
//
//		}
	}

	rewind(fp_local);
	while (fscanf(fp_local, "%s %s %s %s", local_file_name, local_user_name, local_time_stamp, local_seq_no) != EOF)
	{
		printf("local log:  %s\t%s\t%s\t%s\n",local_file_name, local_user_name, local_time_stamp, local_seq_no);
		//is_found=0 if a file in repo cannot be found in local
		//        =1 otherwise
		int is_found=0;
		rewind(fp_repo);
		while (fscanf(fp_repo, "%s %s %s", repo_file_name, repo_user_name, repo_seq_no) != EOF)
		{
			if (strcmp(local_file_name, repo_file_name)==0)
			{
				is_found=1;
				break;
			}
		}

		if (is_found==0)
		{
			put_file(dir,local_file_name,slice);

			show_modified_time(dir,local_file_name);
			fprintf(fp_new_local,"%s\t%s\t%s\t%s\n",local_file_name, local_user_name, timestamp, local_seq_no);
			fprintf(fp_new_repo, "%s\t%s\t%s\n",local_file_name,local_user_name,local_seq_no);
		}
	}

	fclose(fp_repo);
	fclose(fp_local);
	fclose(fp_new_repo);
	fclose(fp_new_local);



	char cmdTmp[100];
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
	put_file(dir,".repolog",slice);

	return;
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
		printf("successful after update local log\n");
		download_repo_log(dir_name, slice_name);
		printf("successful after download repo log\n");
		check_repo_log(dir_name, slice_name);
		printf("successful after check repo log\n");
	// }

	printf("success!!!!!!!!!!\n");
	
	return 0;
}
