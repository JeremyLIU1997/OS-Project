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
#include "greedy.h"

// macros

// global variables
int schedule[1000];

// prototypes
int compareTo(const void* a, const void* b) {
	struct Event* structA = (struct Event*) a;
	struct Event* structB = (struct Event*) b;
	return (structA->unit_benefit >= structB->unit_benefit) ? -1 : 1;
}

void init() {
	/* Initialize Schedule */
	for (int i = 1; i <= event_counter; ++i) {
		schedule[i] = -1;
		events[i].rest_t = events[i].duration;
	}
}

void greedy() {
	printf("Running Greedy!\n");
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

		events[i].unit_benefit = benefit / events[i].duration;
	}

	qsort(events+1, event_counter, sizeof(events[0]), compareTo);
	printf("Sorting completed...\n");
	printf("Number of events: %d\n", event_counter);
	for (int i = 1; i <= event_counter; ++i)
		print_event(i);

	init();

	/* Start Scheduling */
	int current_time = 0;
	for (int i = 1; i <= event_counter; ++i)
	{	
		int ddl = (events[i].date - period_start_date) * HOUR_PER_DAY + events[i].time;
		while (current_time <= ddl && (events[i].rest_t)-- > 0)
			schedule[current_time++] = i;
	}

	for (int i = 1; i <= event_counter; ++i)
	{
		printf("%d ", schedule[i]);
	}

}
