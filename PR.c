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
            //if (cur_time <= cur->date*100 + cur->time) { // the date and time > current time
                if (cur->time + cur->duration > end_time || cur->date > end_date || cur->date < start_date) { // the Revision or Activity can not be finished in one go at the current day
					// printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
                    fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    REJECTED 0.0%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->time, cur->duration);
				}
				else {
                    int ifreject = 0;
                    slot = cur->time - start_time + 4 * (cur->date - start_date);
                    for (int i = slot; i < slot+cur->duration; i++){
                        if (slots[i] == 1){
                            ifreject = 1;
                        }
                    }
                    if (ifreject == 1){
                        // printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
                        fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    REJECTED 0.0%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->time, cur->duration);
                    }
                    else{
                        cur_time = cur->date * 100 + cur->time;
                        slot = cur->time - start_time + 4 * (cur->date - start_date);
                        for (int i = slot; i < slot+cur->duration; i++){
                            slots[i] = 1;
                        }
                        // printf("Event (id: %d, name: %s, type: %d) has been accepted\n", cur->id, cur->name, cur->type);
                        fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    ACCEPTED 100.0%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->time, cur->duration);
                        accept++;
                        for (int i = 0; i < cur->duration; i++){
                            fprintf(sch_result, "%d %d %d %s %d %d\n", cur_time/100, cur_time%100, cur->id, cur->name, cur->type, cur->duration);
                            cur_time++;
                        }
                    }
                }
            /*}
            else {
				// printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
				fprintf(log_file, "%d %s %s %d-%d-%d %d:00 %d    REJECTED 0.0%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->time, cur->duration);
			}*/
        }
        else {
            // See if the remaining time is enough for it
            int cur_date = cur_time/100;
            int rem_date = end_date-cur_date;
            int today_end = cur_date*100 + 23;
            int rem_time = rem_date*(end_time - start_time) + today_end - cur_time;
            int time_to_ddl = (cur->date - cur_date)*(end_time - start_time) + today_end - cur_time;
            if (cur->date < cur_date) {
                fprintf(log_file, "%d %s %s %d-%d-%d %d          REJECTED 0.0%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
            }
            else {
                if (rem_time >= cur->duration){
                    if (time_to_ddl >= cur->duration) {
                        //cur_time = cur_time + 100*(cur->rest_t/(end_time-start_time)) + (start_time + cur->rest_t%(end_time-start_time));
                        // printf("Event (id: %d, name: %s, type: %d) has been accepted and has completed\n", cur->id, cur->name, cur->type);
                        fprintf(log_file, "%d %s %s %d-%d-%d %d          ACCEPTED 100.0%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration);
                        accept++;
                        // the Event has been completed
                        slot = cur_time % cur_date - start_time + 4 * (cur_date - start_date);
                        for (int i = 0; i < cur->duration; i++){
                            fprintf(sch_result, "%d %d %d %s %d %d\n", cur_time/100, cur_time%100, cur->id, cur->name, cur->type, cur->duration);
                            cur_time++;
                            if (cur_time%100 >= end_time){
                                cur_time += 100;
                                cur_time = cur_time/100 * 100 + start_time;
                            }
                        }
                        //cur_time += cur->duration;

                        for (int i = slot; i < slot+cur->duration; i++){
                            slots[i] = 1;
                        }
                        /*int overflow = (cur_time % cur_date) - end_time;
                        if (overflow > 0){
                            int day_spent = overflow / (end_time - start_time);
                            int remainder = overflow % (end_time - start_time);
                            cur_time = (cur_date + 1 + day_spent) * 100 + start_time + remainder;
                        }*/
                    }
                    else { // it fails to finish before the ddl
                        cur->percent = (float)time_to_ddl/(float)cur->duration * 100;
                        // printf("Event (id: %d, name: %s, type: %d) has been accepted and only finished %f%%\n", cur->id, cur->name, cur->type, cur->percent);
                        fprintf(log_file, "%d %s %s %d-%d-%d %d          ACCEPTED %.1f%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration, cur->percent);
                        accept++;
                        //cur_time = cur->date * 100 + 100 + start_time;
                        for (int i = 0; i < time_to_ddl; i++){
                            fprintf(sch_result, "%d %d %d %s %d %d\n", cur_time/100, cur_time%100, cur->id, cur->name, cur->type, cur->duration);
                            cur_time++;
                            if (cur_time%100 >= end_time){
                                cur_time += 100;
                                cur_time = cur_time/100 * 100 + start_time;
                            }
                        }
                    }
                }
                else
                { // it fails to finish before end date
                    cur->percent = (float)rem_time/(float)cur->duration * 100;
                    // printf("Event (id: %d, name: %s, type: %d) has been accepted but has not completed\n", cur->id, cur->name, cur->type);
                    fprintf(log_file, "%d %s %s %d-%d-%d %d          ACCEPTED %.1f%%\n", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100, cur->duration, cur->percent);
                    for (int i = 0; i < rem_time; i++){
                        fprintf(sch_result, "%d %d %d %s %d %d\n", cur_time/100, cur_time%100, cur->id, cur->name, cur->type, cur->duration);
                        cur_time++;
                        if (cur_time%100 >= end_time){
                            cur_time += 100;
                            cur_time = cur_time/100 * 100 + start_time;
                        }
                    }
                    accept++;
                    slot = cur_time % cur_date - start_time + 4 * (cur_date - start_date);
                }
            }
		}
        if (cur->next == NULL) {
            cur = NULL;
            break;
        }
        else {
            cur = cur->next;
        }
    }
    int slots_used = 0;
    for (int i = 0; i < total_slots; i++){
        slots_used += slots[i];
    }
    fprintf(summary, "\nNumber of requests accepted: %d\n", accept);
    fprintf(summary, "Number of requests rejected: %d\n", length-accept);
	fprintf(summary, "Number of time slots used: %d\n", slots_used);
     /* Clear the remaining rejected events */
	while (cur!=NULL) {
		// printf("Event (id: %d, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
		fprintf(log_file, "%d %s %s %d-%d-%d", cur->id, operations[cur->type], cur->name, cur->date/10000, (cur->date/100)%100, cur->date%100);
		if (cur->type == 2 || cur->type == 3){
            fprintf(log_file, " %d:00 %d    REJECTED 0.0%%\n", cur->time, cur->duration);
		}
		else {
			fprintf(log_file, " %d          REJECTED 0.0%%\n", cur->duration);
		}
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
                newNode->next = NULL;
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
	FILE *sch_result = fopen("./output/PR_result", "w"), *log_file = fopen("./output/S3_PR.log", "w"), *summary = fopen("./output/S3_report_PR.dat", "w");
	head = &events[1];
	for (int i = 1; i <= length; i++) {
		if (i < length) {
			events[i].next = &events[i+1];
		}
		else {
			events[length].next = NULL;
		}
		events[i].rest_t = events[i].duration;
	}
	fprintf(log_file, "***Log File - Priority***\n");
	fprintf(log_file, "ID    Name                                      Status\n");
	fprintf(log_file, "======================================================\n");
	fprintf(summary, "***Summary Report***\n");
	fprintf(summary, "\nAlgorithm used: Priority\n");
	fprintf(summary, "\nThere are %d requests\n", length);
	head = Sort_By_Priority(head, length);
	Priority(head, period_start_date, period_end_date, period_start_time, period_end_time, length, sch_result, log_file, summary);
	fprintf(log_file, "\n======================================================\n");
	fprintf(log_file,"Errors (if any):\n");
    int i;
	for (i = 1; i <= length; i++) {
		if (events[i].date < period_start_date || events[i].date > period_end_date) {
			fprintf(log_file, "Event #%d contains an error\n", events[i].id);
		} else {
			if (events[i].type == 2 || events[i].type == 3) {
				if (events[i].time < period_start_time || events[i].time >= period_end_time || events[i].duration > (period_end_time-events[i].time)) {
					fprintf(log_file, "Event #%d contains an error\n", events[i].id);
				}
			}
		}
	}
    fclose(sch_result);
	fclose(log_file);
	fclose(summary);
	return;
}

