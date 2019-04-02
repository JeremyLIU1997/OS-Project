#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
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
		if (temp%100>=end_time) { // deal with overflow
			temp = (temp/100 + 1)*100 + start_time;
		}
		fprintf(sch_result, "%d %d %d %s %d %.2f \n", temp/100, temp%100, head->id, head->name, head->type, head->percent);
		temp++;
	}
	return temp;
}

void print_queue(struct Event* head) {
	while (head!=NULL) {
		printf("%s-->", head->name);
		head = head->next;
	}
	printf("NULL\n");
}

void Round_Robin(int q, struct Event* head, struct Event* tail, int start_date, int end_date, int start_time, int end_time, FILE* sch_result, FILE* log_file, FILE* summary, int total_requests, int pro_ass_count) {
	int cur_time = start_date*100 + start_time, situation=0, slots_elapsed=0, accepted_events=0, total_slots=0, flag=0;
	struct Event* temp=NULL;
	char* operations[] = {"addProject", "addAssignment", "addRevision", "addActivity"};
	
	while (tail!=NULL && cur_time<(end_date*100+end_time)) {
		situation = 0;
		
		/* Revision or Activity */
		if (head->type==2 || head->type==3) {			
			if (cur_time == head->date*100 + head->time) { // it's the right time
				printf("%d   ", cur_time);
				print_queue(head);
				if (end_time - cur_time%100 >= head->duration) { // the rest of the day is sufficient for the Event, Accept
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
				if (pro_ass_count==0) { // only Revisions and Activities are left in the queue
					cur_time++; // push the time for one hour
					if (cur_time%100>=end_time) { // deal with overflow
						cur_time = (cur_time/100 + 1)*100 + start_time;
					}

				} else { // round the Event to the back of the queue and continue RR
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
				pro_ass_count--;
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
				pro_ass_count--;
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
			fprintf(log_file, " %d          ", head->duration);
		}
		fprintf(log_file, "Rejected\n");
		head = head->next;
	}
	
	fprintf(summary, "\nNumber of requests accepted: %d\n", accepted_events);
	fprintf(summary, "Number of requests rejected: %d\n", total_requests-accepted_events);
	fprintf(summary, "Number of time slots used: %d\n", total_slots);
}

void RR_invoker(struct Event events[1000], int event_counter, int q, int period_start_date, int period_end_date, int period_start_time, int period_end_time) {
	printf("Enter the invoker!\n");
	for (int i = 1; i <= event_counter; ++i)
		events[i].rest_t = events[i].duration;
	struct Event* head = NULL;
	struct Event* tail = NULL;
	int i = 0, pro_ass_count = 0;
	FILE *sch_result = fopen("./summary/RR_result.txt", "w"), *log_file = fopen("./summary/RR_log_file.txt", "w"), *summary = fopen("./summary/RR_summary.txt", "w");

	head = &events[1];
	for (i=1;i<=event_counter-1;i++) {
		events[i].next = &events[i+1];
		if (events[i].type==0 || events[i].type==1) {
			pro_ass_count++;
		}
	}
	events[event_counter].next = NULL;
	tail = &events[event_counter];
	
	fprintf(log_file, "***Log File - Round Robin***\n");
	fprintf(log_file, "ID Event                         Accepted/Rejected\n");
	fprintf(log_file, "==================================================\n");
	fprintf(summary, "***Summary Report***\n");
	fprintf(summary, "\nAlgorithm used: Round Robin\n");
	fprintf(summary, "\nThere are %d requests\n", event_counter);
		
	printf("Execute RR now!\n");
	Round_Robin(q, head, tail, period_start_date, period_end_date, period_start_time, period_end_time, sch_result, log_file, summary, event_counter, pro_ass_count);
	
	fclose(sch_result);
	fclose(log_file);
	fclose(summary);
	printf("Round Robin has finished!\n");
}

