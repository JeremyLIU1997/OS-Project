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
#include "ddl_fighter.h"

// macros

// global variables
int* schedule;

// prototypes
int compareTo(const void* a, const void* b) {
	struct Event* structA = (struct Event*) a;
	struct Event* structB = (struct Event*) b;
	return (structA->unit_benefit >= structB->unit_benefit) ? -1 : 1;
}

int get_date_difference(int date1, int date2) {
	int month1 = (date1 % 10000) / 100;
	int month2 = (date2 % 10000) / 100;
	int day1 = date1 % 100;
	int day2 = date2 % 100;
	return (month2 - month1 - 1) * 30 + (30 - day1 + 1) + day2;
}

int get_total_hours(int date1, int date2, int time1, int time2) {
	int hours = (get_date_difference(date1, date2) - 1) * HOUR_PER_DAY;
	hours = hours - (time1- DAY_START) + time2 - DAY_START;
	return hours;
}

/* Test if an event is within the time bound */
int is_legal(struct Event e) {
	return true;
}

/* Get the corresponding hour in schedule[] the event ends */
int get_ddl(struct Event e) {
	if (e.type == ASSIGNMENT_TYPE || e.type == PROJECT_TYPE)
		return get_total_hours(period_start_date,e.date,period_start_time,DAY_END);
	else
		return get_total_hours(period_start_date,e.date,period_start_time,e.time + e.duration);
}

void init() {
	for (int i = 1; i <= event_counter; ++i)
	{
		int level = parse_level(events[i].name);
		float benefit = 0;
		if (events[i].type == PROJECT_TYPE)
			benefit = PROJECT_BASE + (level - 1) * LEVEL_UP_POINT;
		else if (events[i].type == ASSIGNMENT_TYPE)
			benefit = ASSIGNMENT_BASE + (level - 1) * LEVEL_UP_POINT;
		else if (events[i].type == REVISION_TYPE)
			benefit = REVISION_BASE + (level - 1) * LEVEL_UP_POINT;
		else
			benefit = ACTIVITY_BASE;
		events[i].rest_t = events[i].duration;
		events[i].unit_benefit = benefit / events[i].duration;
	}

	schedule = (int*) malloc(sizeof(int) * get_total_hours(period_start_date,period_end_date,period_start_time,period_end_time));
	printf("%d\n", get_total_hours(period_start_date,period_end_date,period_start_time,period_end_time));
	for (int i = 1; i <= event_counter; ++i) {
		schedule[i] = -1;
	}
}

/*
We ensure the highest unit_benefit
task be done as much as possible, by
ensuring enough time is set aside (its
duration number of hours) just before 
its deadline.
Activity and revision is scheduled at
after project and assignments are 
scheduled, if the time slot happes
to be available.
*/


void fight_ddl() {
	printf("Fighting Deadlines !!!\n");
	init();
	qsort(events+1, event_counter, sizeof(events[0]), compareTo);

	/* to delete */
	for (int i = 1; i <= event_counter; ++i)
		print_event(i);

	printf("Deadline fighter scheduling...\n");

	/* handle project and assignment first */
	for (int i = 1; i <= event_counter; ++i)
	{
		if (events[i].type == ACTIVITY_TYPE || events[i].type == REVISION_TYPE)
			continue;
		int ddl = get_ddl(e);
		while(ddl)
	}

	/* then handle revision */

	/* then handle assignment */
	
	printf("\nScheduling Complete!\n");

}
/*

int main() {
	get_total_hours(0,0,0,0);
}
*/