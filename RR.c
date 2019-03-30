#include <stdio.h>
#include <stdlib.h>

#include "RR.h"

/*
Author: LIU Fengming
Student ID: 145104126D
Email: 15104126D@connect.polyu.hk

Notes:
	(1) cur_time is of form YYYYMMDDhh, which exactly records the time slot of concern of each iteration in the scheduling
		the valid valus of hh are (start_time, start_time+1, ..., end_time-1)
	(2) Method to deal with the deadline of Project and Activity: 
			<1> When the finishing time of the current time quantum exceeds the deadline, 
				the time slots of the day of deadline are all depleted and the current time is tuned to the starting time of the day next the day of deadline
			<2> the rest of the duration is also maintained to ensure consistency
			
*/

int print_slots_alloc(struct Event* head, int cur_time, int slots_elapsed, int start_time, int end_time, FILE* sch_result) {
	int i, temp=cur_time;
	for (i=0;i<slots_elapsed;i++) {
		if (temp%100>=end_time) { // will overflow
			temp = (temp/100 + 1)*100 + start_time;
		}
		fprintf(sch_result, "%d %d %d %s %d %.2f \n", temp/100, temp%100, head->id, head->name, head->type, head->percent);
		temp++;
	}
	return temp;
}

void Round_Robin(int q, struct Event* head, struct Event* tail, int start_date, int end_date, int start_time, int end_time, FILE* sch_result, FILE* log_file, FILE* summary, int total_requests) {
	int cur_time = start_date*100 + start_time, situation=0, slots_elapsed=0, accepted_events=0, total_slots=0;
	
	char* operations[] = {"addProject", "addAssignment", "addRevision", "addActivity"};
	
	while (tail!=NULL && cur_time<(end_date*100+end_time)) {	
		situation = 0;
		
		/* Revision or Activity */
		if (head->type==2 || head->type==3) {			
			if (cur_time == head->date*100 + head->time) { // it's the right time
				if (end_time - cur_time >= head->duration) { // the rest of the day is sufficient for the Event, Accept
					cur_time = print_slots_alloc(head, cur_time, head->duration, start_time, end_time, sch_result);
					accepted_events++;
					total_slots = total_slots + head->duration;
					fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    Accepted\n", head->id, operations[head->type], head->name, head->date/10000, (head->date/100)%100, head->date%100, head->time, head->duration);
				} else{ // the rest of the day is not sufficient of the Event, Reject
					fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    Rejected\n", head->id, operations[head->type], head->name, head->date/10000, (head->date/100)%100, head->date%100, head->time, head->duration);
				}
				situation = 1;
			} else if (cur_time > head->date*100 + head->time) { // the right time has passed, Reject
				fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    Rejected\n", head->id, operations[head->type], head->name, head->date/10000, (head->date/100)%100, head->date%100, head->time, head->duration);
				situation = 1;
			} else { // the right time is in the future, return the Event back to the queue for the future
				if (head->next==NULL) { // no more other events, arrange it now
					cur_time = print_slots_alloc(head, head->date*100 + head->time, head->duration, start_time, end_time, sch_result); 
					accepted_events++;
					total_slots = total_slots + head->duration;
					fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    Accepted\n", head->id, operations[head->type], head->name, head->date/10000, (head->date/100)%100, head->date%100, head->time, head->duration);
					situation = 1;
				} else {
					tail->next = head;
					tail = tail->next;
					head = head->next;
					tail->next = NULL;
				}
			}
		}
		
		/* Project or Assignment */
		else {
			head->rest_t = head->rest_t - q;
			cur_time = cur_time + q; // try to do
			
			if (cur_time/100 > head->date) { // the Event occupies time slots beyond the deadline		
				slots_elapsed = q - (cur_time%100 - start_time); // get the valid part of the quantum
				head->percent = head->percent + (float)slots_elapsed/head->duration; // maintain the percent
				head->rest_t = head->rest_t + q - slots_elapsed; // remove the excessive part of completion
				cur_time = print_slots_alloc(head, cur_time-q, slots_elapsed, start_time, end_time, sch_result);
				accepted_events++;
				total_slots = total_slots + slots_elapsed;
				fprintf(log_file, "%d %s %s %d-%d-%d %d          Accepted\n", head->id, operations[head->type], head->name, head->date/10000, (head->date/100)%100, head->date%100, head->duration);
				situation = 1;
			} else if (head->rest_t>0) { // the Event consumes the allocated quantum and the quantum is within the deadline, but the Event has not been completed yet
				cur_time = print_slots_alloc(head, cur_time-q, q, start_time, end_time, sch_result);
				total_slots = total_slots + slots_elapsed;
				head->percent = head->percent + (float)q/head->duration;
				tail->next = head;
				tail = tail->next;
				head = head->next;
				tail->next = NULL;
			} else { // the Event has been completed
				cur_time = print_slots_alloc(head, cur_time-q, q+head->rest_t, start_time, end_time, sch_result);
				accepted_events++;
				total_slots = total_slots + q + head->rest_t;
				fprintf(log_file, "%d %s %s %d-%d-%d %d          Accepted\n", head->id, operations[head->type], head->name, head->date/10000, (head->date/100)%100, head->date%100, head->duration);
				situation = 1;
			}
		}	
		
		/* Throw away the rejected or finished Event */
		if (situation==1) { // the current event should be removed from the queue
			if (head->next==NULL) { // the Event is the last one
				head = NULL;
				fprintf(summary, "\nNumber of requests accepted: %d\n", accepted_events);
				fprintf(summary, "Number of requests rejected: %d\n", total_requests-accepted_events);
				fprintf(summary, "Number of time slots used: %d\n", total_slots);
				return;
			} else {
				head = head->next;
			}
		}
	}
	
	/* Clear the remaining rejected events */
	while (head!=NULL) {
		fprintf(log_file, "%d %s %s %d-%d-%d", head->id, operations[head->type], head->name, head->date/10000, (head->date/100)%100, head->date%100);
		if (head->type==2 || head->type==3) {
			fprintf(log_file, " %d:00 %d    ", head->time, head->duration);
		} else {
			fprintf(log_file, " %d          ", head->time, head->duration);
		}
		fprintf(log_file, "Rejected\n");
		head = head->next;
	}
	
	fprintf(summary, "\nNumber of requests accepted: %d\n", accepted_events);
	fprintf(summary, "Number of requests rejected: %d\n", total_requests-accepted_events);
	fprintf(summary, "Number of time slots used: %d\n", total_slots);
}

int main(int argc, char *argv[]) {
	struct Event* head = NULL;
	struct Event* tail = NULL;
	struct Event a1 = {.id=1, .type=1, .name="COMP2432A1", .date=20190418, .time=-1, .duration=12, .rest_t=12, .percent=0.0, .next=NULL};
	struct Event a2 = {.id=2, .type=0, .name="COMP2422P1", .date=20190420, .time=-1, .duration=26, .rest_t=26, .percent=0.0, .next=NULL};
	struct Event a3 = {.id=3, .type=2, .name="COMP2000",   .date=20190414, .time=19, .duration=2,  .rest_t=2,  .percent=-1,  .next=NULL};
	struct Event a4 = {.id=4, .type=3, .name="Meeting",    .date=20190418, .time=20, .duration=2,  .rest_t=2,  .percent=-1,  .next=NULL};
	FILE *sch_result = fopen("./RR_result", "w"), *log_file = fopen("./RR_log_file", "w"), *summary = fopen("./RR_summary", "w");
	long int result = 0;
	int total_requests = 4;
	
	fprintf(log_file, "***Log File - Round Robin***\n");
	fprintf(log_file, "ID Event                         Accepted/Rejected\n");
	fprintf(log_file, "==================================================\n");
	fprintf(summary, "***Summary Report***\n");
	fprintf(summary, "\nAlgorithm used: Round Robin\n");
	fprintf(summary, "\nThere are %d requests\n", total_requests);
	
	head = &a1;
	a1.next = &a2;
	a2.next = &a3;
	a3.next = &a4;
	tail = &a4;
	
	Round_Robin(1, head, tail, 20190408, 20190421, 19, 23, sch_result, log_file, summary, total_requests);
	
	fclose(sch_result);
	fclose(log_file);
	fclose(summary);
	printf("Round Robin has finished!\n");
}

