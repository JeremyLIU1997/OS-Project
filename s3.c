#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <regex.h>

// our headers
#include "parser.h"
#include "s3.h"

/* prototype */
void getInput(char *instr);
void toChild(int fd_toC[][2], char *instr);
void test(int fd_toC[][2], int i);//to be deleted

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
			while ((n = read(fd_toC[i][0],command[event_counter],BUF_SIZE)) > 0) {
				command[event_counter][n] = '\n';
				command[event_counter][n+1] = 0;
				if (strcmp(command[event_counter],"RUN\n") == 0) {
					printf("RUN!!!\n");
					parse();
					break;
				}
				event_counter++;
			}
			
			for (int i = 0; i < event_counter; ++i)
			{

				print_event(i);
			}

			//sleep(3);
			/* call schedulers... */

			// just for testing: 
			// test(fd_toC, i);//to be deleted




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
			toChild(fd_toC, instr);

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

/* toChild function: pass input command to all children */
void toChild(int fd_toC[][2], char *instr) {
	for (int i = 0; i < CHILD_NUM; i++) {
		write(fd_toC[i][1], instr, strlen(instr));
	}
	//printf("<Parent> message passed to all child\n");//to be deleted
}


//to be deleted
void test(int fd_toC[][2], int i) {
	int n;
	char buf[BUF_SIZE];

	while ((n = read(fd_toC[i][0], buf, BUF_SIZE)) > 0) { // read from pipe
		buf[n] = 0;
		printf("number ofBytes read: %d\n", n);
		printf("<Child %d> message [%s] received\n", getpid(), buf);
		if (buf[0] == 'e') break;
	}
}
