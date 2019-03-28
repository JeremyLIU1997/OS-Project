#include <stdio.h>
#include <stdlib.h>

/*
Author: LIU Fengming
*/

#include "parser.h"

void Round_Robin(int q, struct Event* head, struct Event* tail, int start_date, int end_date, int start_time, int end_time) {
	int cur_time = start_date*100 + start_time;
	int temp1, temp2;
	
	while (tail!=NULL && cur_time<(end_date*100+end_time)) {
		//printf("Date: %d, Time: %d, Event: (id: %s, name: %s, type: %d. percent: %.2f)\n", cur_time/100, cur_time%100, head->id, head->name, head->type, head->percent);
		
		/* Revision or Activity */
		if (head->type==2 || head->type==3) {
			if (cur_time == head->date*100 + head->time) { // it's the right date and time
				if (end_time - cur_time < head->duration) { // the Revision or Activity can not be finished in one go at the current day
					printf("Event (id: %s, name: %s, type: %d) has been rejected\n", head->id, head->name, head->type);
				} else {
					cur_time = cur_time + head->duration;
				}
			} else {
				printf("Event (id: %s, name: %s, type: %d) has been rejected\n", head->id, head->name, head->type);
			}
			if (head->next==NULL) { // the Event is the last one
				head = NULL;
				return;
			} else {
				head = head->next;
			}
		}
		
		/* Project or Assignment */
		else {
			head->rest_t = head->rest_t - q;
			cur_time = cur_time + q;
			
			if (head->rest_t>0) { // the Event has not been completed yet
				head->percent = head->percent + (float)q/head->duration;
				tail->next = head;
				tail = tail->next;
				head = head->next;
				tail->next = NULL;
			} else { // the Event has been completed
				printf("Event (id: %s, name: %s, type: %d) has been accepted\n", head->id, head->name, head->type);
				cur_time = cur_time + head->rest_t; // adjust the current time when rest_t becomes negative
				if (head->next==NULL) { // the Event is the last one
					head = NULL;
					return;
				} else {
					head = head->next;
				}
			}
		}	
		
		/* Time adjustment */
		temp1 = cur_time%100;
		if (temp1>=end_time) { // overflow to the following days
			cur_time = cur_time/100*100 + (temp1 - start_time)%(end_time - start_time) + start_time;
			if (cur_time%100 < temp1 - q) {
				cur_time = cur_time%100 + (cur_time/100 + q/(end_time - start_time) + 1)*100;
			} else {
				cur_time = cur_time%100 + (cur_time/100 + q/(end_time - start_time))*100;
			}
		}
	}
	
	/* Clear the remaining rejected events */
	while (head!=NULL) {
		printf("Event (id: %s, name: %s, type: %d) has been rejected\n", head->id, head->name, head->type);
		head = head->next;
	}
}

int main(int argc, char *argv[]) {
	struct Event* head = NULL;
	struct Event* tail = NULL;
	struct Event a1 = {.id="0001", .type=1, .name="COMP2432A1", .date=20190418, .time=-1, .duration=12, .rest_t=12, .percent=0.0, .next=NULL};
	struct Event a2 = {.id="0002", .type=0, .name="COMP2422P1", .date=20190420, .time=-1, .duration=26, .rest_t=26, .percent=0.0, .next=NULL};
	struct Event a3 = {.id="0003", .type=2, .name="COMP2000",   .date=20190414, .time=19, .duration=2,  .rest_t=2,  .percent=-1,  .next=NULL};
	struct Event a4 = {.id="0004", .type=3, .name="Meeting",    .date=20190418, .time=20, .duration=2,  .rest_t=2,  .percent=-1,  .next=NULL};

	head = &a1;
	a1.next = &a2;
	a2.next = &a3;
	a3.next = &a4;
	tail = &a4;
	Round_Robin(1, head, tail, 20190408, 20190421, 19, 23);
	printf("Round Robin has finished!\n");
}

