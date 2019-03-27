/*
 * Author: LIU Le 15103617d
 * Date: 2019/3/27
 */

// header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>

// my headers
#include "main.h"

// macros

// prototypes

//global variables

// functions
void parse() {

}

int split(char* input, char* output, int* start) {
	char* ptr = output;
	while (*(input + *start) != ' ' && *(input + *start) != '\n')
		*ptr++ = *(input + (*start)++);
	*ptr = '\0';
	if (*(input + *start -1) == '\n')
		return -1;
	else 
		return 0;
}

void print_event() {

}

struct event events[1000]; // support at most 1000 events
int event_counter = 0;

int main(int argc, char* argv[]) {
	char buf[100];
	char command[6][100];

	strcpy(command[0],"addPeriod 2019-04-08 2019-04-21 19:00 23:00");
	strcpy(command[1],"addAssignment COMP2432A1 2019-04-18 12");
	strcpy(command[2],"addProject COMP2422P1 2019-04-20 26");
	strcpy(command[3],"addRevision COMP2000 2019-04-14 19:00 2");
	strcpy(command[4],"addActivity Meeting 2019-04-18 20:00 2");
	strcpy(command[5],"addBatch S3_tasks_00.dat");

	printf("Start!\n");
	for (int i = 0; i < 6; ++i)
	{
		int a = 0;
		int* start = &a;
		char temp[100];
		split(command[i],temp,start);
		(*start)++;
		if (strcmp(temp,"addPeriod") == 0) {

		}
		else if (strcmp(temp,"addAssignment") == 0 || strcmp(temp,"addProject") == 0) {
			if (strcmp(temp,"addAssignment") == 0)
				events[i].type = ASSIGNMENT_TYPE;
			else
				events[i].type = PROJECT_TYPE;
			// handle name
			split(command[i],temp,start);
			strcpy(events[i].name,temp);
			(*start)++;
			// handle date
			split(command[i],temp,start);
			char date_temp[8]; int end=0;
			for (int j = 0; j < strlen(temp); ++j)
			{
				if (temp[j] == '-')
					continue;
				date_temp[end++] = temp[j];
			}
			date_temp[end] = '\0';
			events[i].date = (int)atoi(date_temp);
			(*start)++;
			// handle time
			split(command[i],temp,start);
			events[i].duration = (int)atoi(temp);
		}
		else if (strcmp(temp,"addRevision") == 0 || strcmp(temp,"addActivity") == 0) {
			if (strcmp(temp,"addRevision") == 0)
				events[i].type = REVISION_TYPE;
			else
				events[i].type = ACTIVITY_TYPE;
			// handle name
			split(command[i],temp,start);
			strcpy(events[i].name,temp);
			(*start)++;
			// handle date
			split(command[i],temp,start);
			char date_temp[8]; int end=0;
			for (int j = 0; j < strlen(temp); ++j)
			{
				if (temp[j] == '-')
					continue;
				date_temp[end++] = temp[j];
			}
			date_temp[end] = '\0';
			events[i].date = (int)atoi(date_temp);
			(*start)++;
			// handle time
			split(command[i],temp,start);
			events[i].time = (temp[0] - '0') * 10 + (temp[1] - '0');
			(*start)++;
			// handle time
			split(command[i],temp,start);
			events[i].duration = (int)atoi(temp);
		}
	}
}