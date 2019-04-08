
#ifndef PARSER_H
#define PARSER_H

// macros
#define PROJECT_TYPE 0
#define ASSIGNMENT_TYPE 1
#define REVISION_TYPE 2
#define ACTIVITY_TYPE 3
#define DDL_FIGHTER 0
#define RR 1
#define PR 2
#define ALL 3

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
#define DAY_START 19
#define DAY_END 23
#define HOUR_PER_DAY (DAY_END - DAY_START)

// structs
struct Event {
	int id;
	int type; // 0: Project, 1: Assignment, 2: Revision, 3: Activity
	char name[30];
	int date; // format: YYYYMMDD
	int time; // format: hh (0<=hh<=23), -1 represents invalid
	int ddl;
	int duration;
	int rest_t; // the remaining hours
	float percent; // -1 represents in valid
	float unit_benefit;
	int status;
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
extern char report_filename[100];

// prototypes
void parse();
void print_event();
void create_scheduler(int option);
int parse_level(char* name);
bool is_digit(char a);
void output(char *filename, char *algorithm);

#include "ddl_fighter.h"
#include "RR.h"
#include "PR.h"

#endif