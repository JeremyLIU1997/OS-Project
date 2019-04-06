/*
 * Author: DING Dashan 17082316d
 * Date: 2019/3/30
 */
#include "PR.h"

// Priority: Project > Assignment > Revision > Activity

struct Event * Sort_By_Priority(struct Event* head, int length);

void Priority(struct Event* head, int start_date, int end_date, int start_time, int end_time,int length, FILE* sch_result, FILE* log_file, FILE* summary){
    /*No exemption for this version*/
    int cur_time = start_date*100 + start_time;
    int total_slots = (end_time-start_time)*(end_date-start_date+1);
    int slot, accept = 0;
    int *slots = (int *)malloc(total_slots * (sizeof(int)));
    for (int i = 0; i < total_slots; i++){
        slots[i] = 0;
    }
    char* operations[] = {"addProject", "addAssignment", "addRevision", "addActivity"};
    struct Event* cur = head;
    while (cur_time<(end_date*100+end_time) && cur!=NULL){
        /* Revision or Activity */
        if (cur->type == 2 || cur->type == 3){
            if (cur_time <= cur->date*100 + cur->time) { // it's the right date and time
                if (cur->date*100 + end_time - cur_time < cur->duration) { // the Revision or Activity can not be finished in one go at the current day
					// printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
                    fprintf(log_file, "%d %s %s %d-%d-%d %d          Rejected\n", cur->id, operations[head->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
				}
				else {
                    int ifreject = 0;
                    int cur_date = cur_time/100;
                    slot = cur_time % cur_date - start_time + 4 * (cur_date - start_date);
                    for (int i = slot; i < slot+cur->duration; i++){
                        if (slots[i] == 1){
                            ifreject = 1;
                        }
                    }
                    if (ifreject == 1){
                        // printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
                        fprintf(log_file, "%d %s %s %d-%d-%d %d          Rejected\n", cur->id, operations[head->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
                    }
                    else{
                        int cur_date = cur_time/100;
                        slot = cur_time % cur_date - start_time + 4 * (cur_date - start_date);
                        cur_time = cur_time + cur->duration;
                        for (int i = slot; i < slot+cur->duration; i++){
                            slots[i] = 1;
                        }
                        // printf("Event (id: %d, name: %s, type: %d) has been accepted\n", cur->id, cur->name, cur->type);
                        fprintf(log_file, "%d %s %s %d-%d-%d %d          Accepted\n", cur->id, operations[head->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
                        accept++;
                        for (int i = 0; i < cur->duration; i++){
                            fprintf(sch_result, "%d %d %d %s %d \n", cur_time/100, cur_time%100, cur->id, cur->name, cur->type);
                        }
                    }
                }
            }
            else {
				// printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
				fprintf(log_file, "%d %s %s %d-%d-%d %d          Rejected\n", cur->id, operations[head->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
			}
			if (cur->next == NULL) {
                cur = NULL;
                break;
			}
			else {
			    cur = cur->next;
			}
        }
        else {
            // See if the remaining time is enough for it
            int cur_date = cur_time/100;
            int rem_date = end_date-cur_date;
            int today_end = cur_date*100 + 23;
            int rem_time = rem_date*(end_time - start_time) + today_end - cur_time;
            int time_to_ddl = (cur->date - cur_date)*(end_time - start_time) + today_end - cur_time;
            if (rem_time >= cur->rest_t){
                if (time_to_ddl >= cur->rest_t) {
                    //cur_time = cur_time + 100*(cur->rest_t/(end_time-start_time)) + (start_time + cur->rest_t%(end_time-start_time));
                    // printf("Event (id: %d, name: %s, type: %d) has been accepted and has completed\n", cur->id, cur->name, cur->type);
                    fprintf(log_file, "%d %s %s %d-%d-%d %d          Accepted\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
                    accept++;
                    // the Event has been completed
                    slot = cur_time % cur_date - start_time + 4 * (cur_date - start_date);
                    for (int i = 0; i < cur->duration; i++){
                        fprintf(sch_result, "%d %d %d %s %d \n", cur_time/100, cur_time%100, cur->id, cur->name, cur->type);
                    }
                    cur_time += cur->duration;

                    for (int i = slot; i < slot+cur->duration; i++){
                        slots[i] = 1;
                    }
                    int overflow = (cur_time % cur_date) - end_time;
                    if (overflow > 0){
                        int day_spent = overflow / (end_time - start_time);
                        int remainder = overflow % (end_time - start_time);
                        cur_time = (cur_date + 1 + day_spent) * 100 + start_time + remainder;
                    }
                }
                else {
                    cur->percent = (float)time_to_ddl/(float)cur->duration * 100;
                    // printf("Event (id: %d, name: %s, type: %d) has been accepted and only finished %f%%\n", cur->id, cur->name, cur->type, cur->percent);
                    fprintf(log_file, "%d %s %s %d-%d-%d %d          Accepted\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
                    accept++;
                    cur_time = cur->date * 100 + 100 + start_time;
                    for (int i = 0; i < time_to_ddl; i++){
                        fprintf(sch_result, "%d %d %d %s %d \n", cur_time/100, cur_time%100, cur->id, cur->name, cur->type);
                    }
                }
                if (cur->next==NULL) { // the Event is the last one
					cur = NULL;
					break;
				} else {
					cur = cur->next;
				}
            }
			else { // it fails to finish
                // printf("Event (id: %d, name: %s, type: %d) has been accepted but has not completed\n", cur->id, cur->name, cur->type);
                fprintf(log_file, "%d %s %s %d-%d-%d %d          Accepted\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
                for (int i = 1; ;)
                accept++;
                slot = cur_time % cur_date - start_time + 4 * (cur_date - start_date);
				if (cur->next==NULL) { // the Event is the last one
					cur = NULL;
					break;
				} else {
					cur = cur->next;
				}
			}
		}
    }
    int slots_used = 0;
    for (int i = 0; i < total_slots; i++){
        slots_used += slots[i];
    }
    fprintf(summary, "\nNumber of requests accepted: %d\n", accept);
    fprintf(summary, "Number of requests rejected: %d\n", length-accept);
	fprintf(summary, "Number of time slots used: %d (%f%%)\n", slots_used, (100*(float)slots_used/(float)total_slots));
     /* Clear the remaining rejected events */
	while (cur!=NULL) {
		// printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
		cur = cur->next;
	}
    return;
}

struct Event * Sort_By_Priority(struct Event* head, int length){
    // We duplicate the linked list, or else the original will change
    if (length == 0 || length == 1 || head == NULL) return head;
    struct Event* current, *lastNode=NULL, *newHead;
    for (int i = 0; i < 4; i++){
        current = head;
        for (int j = 0; j < length; j++){
            if (current->type == i){
                struct Event *newNode = (struct Event *)malloc(sizeof(struct Event));
                memcpy(newNode, current, sizeof(struct Event));
                if (lastNode != NULL){
                    lastNode->next = newNode;
                }
                else{
                    newHead = newNode;
                }
                lastNode = newNode;
            }
            current = current->next;
        }
    }
    return newHead;
}

void PR_invoker(struct Event events[1000], int length, int period_start_date, int period_end_date, int period_start_time, int period_end_time){
    struct Event* head = NULL;
	FILE *sch_result = fopen("./summary/PR_result", "w"), *log_file = fopen("./summary/PR_log_file", "w"), *summary = fopen("./summary/PR_summary", "w");
	head = &events[1];
	for (int i=1;i<=length;i++) {
		if (i<length) {
			events[i].next = &events[i+1];
		} else {
			events[length].next = NULL;
		}
		events[i].rest_t = events[i].duration;
	}
	head = Sort_By_Priority(head, length);
	Priority(head, period_start_date, period_end_date, period_start_time, period_end_time, length, sch_result, log_file, summary);
	fprintf(log_file, "\n==================================================\n");
    fclose(sch_result);
	fclose(log_file);
	fclose(summary);
}
