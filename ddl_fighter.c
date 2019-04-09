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
#include <math.h>

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
void generate_summary();
void generate_log();
void generate_intermediate_timetable();
void generate_report();

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
	if (e.type == REVISION_TYPE || e.type == ACTIVITY_TYPE) {
		if (e.time < DAY_START || (e.time + e.duration) > DAY_END)
		return false;
		int ddl = get_ddl(e);
		if ( ddl >= total_hours || (ddl - e.duration) < 0)
			return false;
	}
	else {
		if (e.date > period_end_date || e.date < period_start_date)
			return false;
	}
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
		events[i].status = ACCEPTED;
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
	for (int i = 0; i < total_hours; ++i)
		schedule[i] = 0;
}

/* check if the revision/activity has overlaps with already scheduled events */
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

bool is_error(struct Event e) {
	if (e.date < period_start_date || e.date > period_end_date)
		return true;
	if (e.time > DAY_END || e.time < DAY_START)
		return true;
	if (e.type == ACTIVITY_TYPE || e.type == REVISION_TYPE)
		return !is_legal(e);
	return false;
}

void fight_ddl() {
	init();

	qsort(events+1, event_counter, sizeof(events[0]), compareTo);

	// printf("Deadline fighter scheduling...\n");
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
			schedule[ddl+1] = i;
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
					schedule[ddl--] = i;
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
				schedule[ddl--] = i;
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
					schedule[ddl--] = i;
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
				schedule[ddl--] = i;
		}
	}

	for (int i = 0; i < number_of_reject; ++i)
		events[rejected[i]].status = REJECTED;
	
	// print_result();
	generate_summary();
	generate_log();
	generate_intermediate_timetable();

	free(schedule);

	// printf("\nScheduling Complete!\n");
}

void generate_log() {
	char dict[2][20] = {"ACCEPTED","REJECTED"};
	FILE *log = fopen("./summary/S3_ddl_fighter.log", "w");
	fprintf(log,"***** Log - Deadline Fighter *****\n\n");
	fprintf(log, "ID    Name                                               Status\n");
	fprintf(log, "===============================================================\n");
	char temp[100];
	for (int i = 1; i <= event_counter; ++i) {
		strcpy(temp,command[events[i].id]);
		temp[strlen(temp)-1] = 0;
		if (events[i].rest_t < 0)
			events[i].rest_t = 0;
		fprintf(log, "%d\t%s\t\t\t%s\t%0.1f%%\n",events[i].id,temp,dict[events[i].status],(events[i].duration - events[i].rest_t) * 100 / (float)(events[i].duration));
	}
	fprintf(log, "\n===============================================================\n");
	fprintf(log,"Errors (if any):\n");
	for (int i = 1; i <= event_counter; ++i)
	{
		if (is_error(events[i]) == true)
			fprintf(log,"Event #%d contains error.\n", events[i].id);
	}
	fclose(log);
}

void generate_intermediate_timetable() {
	FILE *file = fopen("./summary/ddl_fighter_result", "w");
	int current_time = -1;
	int current_date;
	for (int i = 0; i < total_hours; ++i)
	{
		current_date = i / HOUR_PER_DAY + period_start_date;
		current_time = (current_time+1) % HOUR_PER_DAY;
		int index = schedule[i];
		if (index == 0) continue;
		fprintf(file, "%d %d %d %s %d %d\n", current_date, DAY_START + current_time,events[index].id,events[index].name,events[index].type,events[index].duration);
	}
	fclose(file);
}

void generate_summary() {
	FILE *summary = fopen("./summary/S3_report_ddl_fighter.dat", "w");
	fprintf(summary, "***** Summary Report *****\n");
	fprintf(summary, "\nAlgorithm: Deadline Fighter Algorithm\n");
	fprintf(summary, "\nNumber of requests: %d\n",event_counter);
	fprintf(summary, "Number of rejected: %d, [ ", number_of_reject);
	for (int i = 0; i < number_of_reject; ++i)
		fprintf(summary, "%d ", events[rejected[i]].id);
	fprintf(summary, "]\n\n");

	int hours_used = 0;
	for (int i = 0; i < total_hours; ++i)
		if (schedule[i] != 0) hours_used++;
	fprintf(summary, "Hours used: %d/%d\n",hours_used,total_hours);
	fclose(summary);
}

/*
int main() {
}
*/
