/*
 * Author: LIU Le 15103617d
 * Date: 2019/3/27
 */

// my headers
#include "parser.h"
#include "RR.h"
#include "pr.h"
// macros

// prototypes

//global variables
struct Event events[1000]; // support at most 1000 events
int event_counter = 0;
char command[1000][100];
int period_start_date;
int period_end_date;
int period_start_time;
int period_end_time;

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

bool is_digit(char a) {
	return (a >= '0' && a <= '9') ? true : false;
}

int parse_level(char* name) {
	int n = strlen(name); int i = 0;
	while (!is_digit(name[i++]));
	return name[i-1]-'0';
}

void print_event(int i) {
	printf("------------------\n");
	printf("Event #%d\n", i);
	printf("Name: %s\n",events[i].name);
	printf("Date: %d\n",events[i].date);
	printf("Time: %d\n",events[i].time);
	printf("Duration: %d\n",events[i].duration);
	printf("Unit_Benefit: %f\n",events[i].unit_benefit);
	printf("------------------\n");
}


void parse_date(char* temp, int* dest) {
	char date_temp[9]; int end=0;
	for (int j = 0; j < strlen(temp); ++j)
		{
			if (temp[j] == '-')
				continue;
			date_temp[end++] = temp[j];
		}
	date_temp[end] = 0;
	*dest = (int)atoi(date_temp);
} 



// functions
void parse() {

	printf("Start!\n");
	event_counter--;
	for (int i = 1; i <= event_counter; ++i)
		events[i].id = i;
	for (int i = 0; i <= event_counter; ++i)
	{
		int a = 0;
		int* start = &a;
		char temp[100];
		split(command[i],temp,start);
		(*start)++;
		if (i == 0) {
			if (!(strcmp(temp,"addPeriod") == 0)) {
				printf("Must add period first. Exit.\n");
				exit(1);
			}
			split(command[i],temp,start);
			parse_date(temp,&(period_start_date));
			(*start)++;
			split(command[i],temp,start);
			parse_date(temp,&(period_end_date));
			(*start)++;
			// handle time
			split(command[i],temp,start);
			period_start_time = (temp[0] - '0') * 10 + (temp[1] - '0');
			(*start)++;
			// handle time
			split(command[i],temp,start);
			period_end_time = (temp[0] - '0') * 10 + (temp[1] - '0');
			(*start)++;
		}
		
		if (strcmp(temp,"addAssignment") == 0 || strcmp(temp,"addProject") == 0) {
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
			parse_date(temp,&(events[i].date));
			(*start)++;
			// handle duration
			split(command[i],temp,start);
			events[i].duration = (int)atoi(temp);
			events[i].time = DAY_END;
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
			parse_date(temp,&(events[i].date));
			(*start)++;
			// handle time
			split(command[i],temp,start);
			events[i].time = (temp[0] - '0') * 10 + (temp[1] - '0');
			(*start)++;
			// handle duration
			split(command[i],temp,start);
			events[i].duration = (int)atoi(temp);
		}
	}

}

/* parser is responsible for creating scheduler upon command */
void create_scheduler(int option) {

	if (option == DDL_FIGHTER) {
		fight_ddl();
	}
	else if (option == RR) {
		Round_Robin();
	}
	else if (option == PR) {

	}

	wait(NULL);

}

