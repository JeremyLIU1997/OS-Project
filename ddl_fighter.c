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
#define REJECT 0
#define OVERLAP 1
#define NO_OVERLAP 2

// global variables
int* schedule;
int total_hours;
int rejected[1000];
int number_of_reject = 0;

// prototypes
int get_ddl(struct Event e);

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

/* return the number of hours between two time points */
int get_hour_diff(int date1, int date2, int time1, int time2) {
	int hours = (get_date_difference(date1, date2) - 1) * HOUR_PER_DAY;
	hours = hours - (time1- DAY_START) + time2 - DAY_START;
	return hours;
}

/* Test if an event is within the time bound */
bool is_legal(struct Event e) {
	if (e.time < DAY_START || (e.time + e.duration) > DAY_END)
		return false;
	int ddl = get_ddl(e);
	if ( ddl >= total_hours || (ddl - e.duration) < 0)
		return false;
	return true;
}

/* Get the corresponding hour in schedule[] the event ends */
int get_ddl(struct Event e) {
	if (e.type == ASSIGNMENT_TYPE || e.type == PROJECT_TYPE)
		return get_hour_diff(period_start_date,e.date,period_start_time,DAY_END) - 1;
	else
		return get_hour_diff(period_start_date,e.date,period_start_time,e.time + e.duration) - 1;
}

/* initialize stuff */
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
	total_hours = get_hour_diff(period_start_date,period_end_date,period_start_time,period_end_time);
	schedule = (int*) malloc(sizeof(int) * total_hours);
	printf("%d\n", get_hour_diff(period_start_date,period_end_date,period_start_time,period_end_time));
	for (int i = 0; i < total_hours; ++i)
		schedule[i] = 0;
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

int has_overlap(struct Event e) {
	int ddl = get_ddl(e);
	bool overlap = false;
	for (int i = 0; i < e.duration; ++i) {
		if (schedule[ddl-i] == 0)
			continue;
		if (events[schedule[ddl-i]].type == ACTIVITY_TYPE || events[schedule[ddl-i]].type == REVISION_TYPE)
			return REJECT;
		overlap = true;
	}
	if (overlap == true)
		return OVERLAP;
	return NO_OVERLAP;
}

void swap(int* ar, int a, int b) {
	int temp = *(ar+a);
	*(ar+a) = *(ar+b);
	*(ar+b) = temp;
}

bool has_enough_slot(int index, int duration) {
	int count = 0;
	while (index-- >= 0) {
		if (schedule[index+1] != 0)
			continue;
		/* slots occupied by activities/revision is excluded */
		if (events[schedule[index+1]].type == ACTIVITY_TYPE || events[schedule[index+1]].type == REVISION_TYPE)
			continue;
		if (++count == duration)
			return true;
	}
	return false;
}

void print_schedule() {
	int first_day_hours = period_start_time - DAY_START;
	for (int i = 0; i < total_hours; ++i) {
		if ((i + first_day_hours) % HOUR_PER_DAY == 0)
			printf("|<%d> ", (i + first_day_hours) / HOUR_PER_DAY + 1);
		printf("%d ", schedule[i]);
	}
	printf("\n");
}

void print_result() {
	print_schedule();
	printf("Rejected: ");
	for (int i = 0; i < number_of_reject; ++i)
		printf("%d ", rejected[i]);
}

void fight_ddl() {
	init();

	qsort(events+1, event_counter, sizeof(events[0]), compareTo);

	printf("Deadline fighter scheduling...\n");
	/* handle project and assignment first */
	for (int i = 1; i <= event_counter; ++i)
	{
		if (events[i].type == ACTIVITY_TYPE || events[i].type == REVISION_TYPE)
			continue;

		int ddl = get_ddl(events[i]);
		/* search forward from deadline for empty spot */
		while(ddl >= 0 && events[i].rest_t > 0) {
			if (schedule[ddl--] != 0)
				continue;
			schedule[ddl+1] = events[i].id;
			(events[i].rest_t)--;
		}
	}

	/* then handle revision */
	for (int i = 1; i <= event_counter; ++i)
	{
		if (events[i].type != REVISION_TYPE)
			continue;
		/* if does not fit in time period, reject directly */
		if (!is_legal(events[i])) {
			rejected[number_of_reject++] = i;
			continue;
		}
		/* test if overlap with other event occurs */
		int ddl = get_ddl(events[i]);
		int result = has_overlap(events[i]);
		if (result == OVERLAP) {
			if (!has_enough_slot(ddl, events[i].duration))
				rejected[number_of_reject++] = i;
			else
			{
				int index = ddl;
				while (index-- >= 0) {
					if (events[i].rest_t == 0)
						break;
					if (schedule[index+1] != 0)
						continue;
					if (events[schedule[index+1]].type == REVISION_TYPE || events[schedule[index+1]].type == ACTIVITY_TYPE)
						continue;
					swap(schedule,index+1,ddl);
					schedule[ddl--] = events[i].id;
					(events[i].rest_t)--;
				}
			}
		}
		else if (result == REJECT) {
			rejected[number_of_reject++] = i;
		}
		/* no overlap, just put the activity there */
		else {
			while ((events[i].rest_t)-- > 0)
				schedule[ddl--] = events[i].id;
		}
	}

	/* then handle activity */
	for (int i = 1; i <= event_counter; ++i)
	{
		if (events[i].type != ACTIVITY_TYPE)
			continue;
		/* if does not fit in time period, reject directly */
		if (!is_legal(events[i])) {
			rejected[number_of_reject++] = i;
			continue;
		}
		/* test if overlap with other event occurs */
		int ddl = get_ddl(events[i]);
		int result = has_overlap(events[i]);
		if (result == OVERLAP) {
			if (!has_enough_slot(ddl, events[i].duration))
				rejected[number_of_reject++] = i;
			else
			{
				int index = ddl;
				while (index-- >= 0) {
					if (events[i].rest_t == 0)
						break;
					if (schedule[index+1] != 0)
						continue;
					if (events[schedule[index+1]].type == REVISION_TYPE || events[schedule[index+1]].type == ACTIVITY_TYPE)
						continue;
					swap(schedule,index+1,ddl);
					schedule[ddl--] = events[i].id;
					(events[i].rest_t)--;
				}
			}
		}
		else if (result == REJECT) {
			rejected[number_of_reject++] = i;
		}
		/* no overlap, just put the activity there */
		else {
			while ((events[i].rest_t)-- > 0)
				schedule[ddl--] = events[i].id;
		}
	}
	
	print_result();
	free(schedule);
	printf("\nScheduling Complete!\n");
}

/*
int main() {
}
*/
