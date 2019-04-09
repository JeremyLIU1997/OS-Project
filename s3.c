#include "s3.h"

/* prototype */
void getInput(char *instr);
void cmdToChild(int fd_toC[][2], char *instr);
void toChild(int fd_toC[][2], char *instr);
void test(int fd_toC[][2], int i);//to be deleted
char report_filename[100];
void analyzer();
float scoring(char *filename);

/* global variable */
int fd_toC[CHILD_NUM][2], fd_toP[CHILD_NUM][2];

/* main */
int main(int argc, char *argv[]) {
	int pid, i;
	
	// create pipes
	for (i = 0; i < CHILD_NUM; i++) {
		if (pipe(fd_toC[i]) < 0 || pipe(fd_toP[i]) < 0) {
			printf("Pipe creation error\n");
			exit(1);
		}
	}

	// create child processes
	for (i = 0; i < CHILD_NUM; i++) {
		pid = fork();
		if (pid < 0) {
			printf("Fork failed\n");
			exit(1);
		}

		if (pid == 0) { // child process
			close(fd_toC[i][1]);
			close(fd_toP[i][0]);
			
			int n=0;
			char str[100];
			bool parsed = false;
			while ((n = read(fd_toC[i][0],str,BUF_SIZE)) > 0) {
				str[n] = '\n'; str[n+1] = 0;
				write(fd_toP[i][1],"O",1); /* ACK message */
				if (strncmp(str,"run",3) == 0 && parsed == false) {
					parsed = true;
					parse();
				}
				if (strcmp(str,"run ddl\n") == 0) {
					create_scheduler(DDL_FIGHTER);
					continue;
				}
				else if (strcmp(str,"run rr\n") == 0) {
					create_scheduler(RR);
					continue;
				}
				else if (strcmp(str,"run pr\n") == 0) {
					create_scheduler(PR);
					continue;
				}
				else if (strcmp(str,"run all\n") == 0) {
					create_scheduler(ALL);
					continue;
				} 
				else if (strcmp(str, "analyze\n") == 0) {
					analyzer();
					continue;
				}
				else if (strcmp(str,"exitS3\n") == 0) {
					printf("Parser Exited!\n");
					exit(0);
				}
				/* Increment event_counter only if NOT run command */
				strcpy(command[event_counter++],str);
			}

			close(fd_toC[i][0]);
			close(fd_toP[i][1]);
			exit(0);
		}
	}

	if (pid > 0) { // parent process
		for (i = 0; i < CHILD_NUM; i++) {
			close(fd_toC[i][0]);
			close(fd_toP[i][1]);
		}
		printf("\t~~WELCOME TO S3~~\n");
		char instr[BUF_SIZE];

		while (1) {
			getInput(instr);
			cmdToChild(fd_toC, instr);
			if (strcmp(instr, "exitS3") == 0) break;
		}

		printf("Bye-bye!\n");
		for (i = 0; i < CHILD_NUM; i++) {
			close(fd_toC[i][1]);
			close(fd_toP[i][0]);
		}
	}

	// wait for all child processes
	for (i = 0; i < CHILD_NUM; i++)
		wait(NULL);

	return 0;
}

/* input function: scan input command */
void getInput(char *instr) {
	printf("Please enter:\n> ");
	scanf("%[^\n]", instr); // scan the whole input line
	getchar();
}

void sync() {
	char temp[10];
	for (int i = 0; i < CHILD_NUM; ++i)
		read(fd_toP[i][0],temp,1);
}

/* cmdToChild function: pass all inputed command to children */
void cmdToChild(int fd_toC[][2], char *instr) {
	if (strncmp(instr, "addBatch", 8) != 0) {
		toChild(fd_toC, instr);
		sync();
	}

	else { // if the command is "addBatch ...", read file
		FILE *fp;
		char *filename = (char*) malloc(strlen(instr)-9+1);
		strcpy(filename, instr+9);
		fp = fopen(filename, "r");
		if (fp == NULL) {
			printf("Cannot open the file!\n"); 
			exit(1);
		}

		while(fscanf(fp, "%[^\n]\n", instr) != EOF) {
		//while( fgets (instr, BUF_SIZE, fp) != NULL ) {
			toChild(fd_toC, instr);
			int i = 0;
			sync();
		}
		fclose(fp);
		free(filename);
	}
}

/* toChild function: pass a command to all children */
void toChild(int fd_toC[][2], char *instr) {
	for (int i = 0; i < CHILD_NUM; i++)
		write(fd_toC[i][1], instr, strlen(instr));
}


//to be deleted
void test(int fd_toC[][2], int i) {
	int n;
	char buf[BUF_SIZE];

	while ((n = read(fd_toC[i][0], buf, BUF_SIZE)) > 0) { // read from pipe
		buf[n] = 0;
		printf("<Child %d> message [%s] received of %d bytes\n", getpid(), buf, n);
		if (buf[0] == 'e') break;
	}
}

