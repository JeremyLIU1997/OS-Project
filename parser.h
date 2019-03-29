
#ifndef PARSER_H
#define PARSER_H

// macros
#define ASSIGNMENT_TYPE 0
#define PROJECT_TYPE 1
#define REVISION_TYPE 2
#define ACTIVITY_TYPE 3
#define GREEDY_ALG 0

#include "greedy.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>


/*
Project base point: 30
Assignment base point: 20
Revision base point: 10
Activity base point: 20
Each level up add 3
*/

#define PROJECT_BASE 30
#define ASSIGNMENT_BASE 20
#define REVISION_BASE 15
#define ACTIVITY_BASE 5
#define LEVEL_UP_POINT 3
#define HOUR_PER_DAY 8

// structs
struct Event {
	char id[5];
	int type;
	char name[30];
	int date;
	int time;
	int duration;
	int rest_t; // the remaining hours
	float percent; // -1 represents in valid
	float unit_benefit;
	struct Event* next;
};

// shared variables
extern struct Event events[1000]; // support at most 1000 events
extern int event_counter;
extern char command[1000][100];
extern int period_start_date;
extern int period_end_date;
extern int period_start_time;
extern int period_end_time;

// prototypes
void parse();
void print_event();
void create_scheduler(int option);
int parse_level(char* name);
bool is_digit(char a);
#endif