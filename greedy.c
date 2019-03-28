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
struct Event* sorted_ar[1000];

// prototypes
int compareTo(const void* a, const void* b) {
	struct Event* structA = (struct Event*) a;
	struct Event* structB = (struct Event*) b;
	return (structA->unit_benefit >= structB->unit_benefit) ? -1 : 1;
}



void greedy() {
	printf("Running Greedy!\n");
	for (int i = 0; i < event_counter; ++i)
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

	for (int i = 0; i < 1000; ++i)
		sorted_ar[i] = events + i;

	qsort(events, event_counter, sizeof(events[0]), compareTo);

	printf("------------------------\n");
	for (int i = 0; i < event_counter; ++i)
	{
		print_event(i);
	}
	printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	for (int i = 0; i < event_counter; ++i)
	{
		printf("%s\n", sorted_ar[i]->name);
	}


}
