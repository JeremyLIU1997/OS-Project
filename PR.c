#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Priority: Project > Assignment > Revision > Activity
struct Event {
	char id[5];
	int type; // 0: Project, 1: Assignment, 2: Revision, 3: Activity
	char name[20];
	int date; // format: YYYYMMDD
	int time; // format: hh (0<=hh<=23), -1 represents invalid
	int duration;
	int rest_t; // the remaining hours
	float percent; // -1 represents in valid
	struct Event* next;
};
struct Event * Sort_By_Priority(struct Event* head, int length);
void Priority(struct Event* head,  int start_date, int end_date, int start_time, int end_time){
    /*No exemption for this version*/
    int cur_time = start_date*100 + start_time;
    struct Event* cur = head;
    while (cur_time<(end_date*100+end_time)){
        /* Revision or Activity */
        if (cur->type == 2 || cur->type == 3){
            if (cur_time == cur->date*100 + cur->time) { // it's the right date and time
                if (end_time - cur_time < cur->duration) { // the Revision or Activity can not be finished in one go at the current day
					printf("Event (id: %s, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
				} else {
					cur_time = cur_time + cur->duration;
				}
            } else {
				printf("Event (id: %s, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
			}
			if (cur->next == NULL) {
                cur = NULL;
                return;
			}
			else {
			    cur = cur->next;
			}
        }
        else {
            // See if the remaining time is enough for it
            int cur_date = cur_time/100;
            int rem_date = end_date-cur_date;
            int rem_time = rem_date*(start_time - end_time) + (cur_date+1)*100 - cur_time;
            int time_to_ddl = (cur->date - cur_date)*(start_time - end_time) + (cur_date+1)*100 - cur_time;
            printf("curdate: %d, remdate: %d\n", cur_date, rem_date);
            if (rem_time >= cur->rest_t){
                if (time_to_ddl >= cur->rest_t) {
                    cur_time = cur_time + 100*(cur->rest_t/(end_time-start_time)) + (start_time + cur->rest_t%(end_time-start_time));
                    printf("Event (id: %s, name: %s, type: %d) has been accepted and has completed\n", cur->id, cur->name, cur->type);
                    // the Event has been completed
                }
                else{
                    cur->percent = (float)time_to_ddl/(float)cur->duration * 100;
                    printf("Event (id: %s, name: %s, type: %d) has been accepted and only finished %f%%\n", cur->id, cur->name, cur->type, cur->percent);
                    cur_time = (cur->date+1) * 100;
                }
                if (cur->next==NULL) { // the Event is the last one
					cur = NULL;
					return;
				} else {
					cur = cur->next;
				}
            }
			else { // it fails to finish
                printf("Event (id: %s, name: %s, type: %d) has been accepted but has not completed\n", cur->id, cur->name, cur->type);
				if (cur->next==NULL) { // the Event is the last one
					cur = NULL;
					return;
				} else {
					cur = cur->next;
				}
			}
		}
    }
     /* Clear the remaining rejected events */
	while (cur!=NULL) {
		printf("Event (id: %s, name: %s, type: %d) has been rejected\n", cur->id, cur->name, cur->type);
		cur = cur->next;
	}

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
                    lastNode->next=newNode;
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
int main(){
    struct Event* head = NULL;
	struct Event a1 = {.id="0001", .type=1, .name="COMP2432A1", .date=20190418, .time=-1, .duration=12, .rest_t=12, .percent=0.0, .next=NULL};
	struct Event a2 = {.id="0002", .type=0, .name="COMP2422P1", .date=20190420, .time=-1, .duration=26, .rest_t=26, .percent=0.0, .next=NULL};
	struct Event a3 = {.id="0003", .type=2, .name="COMP2000",   .date=20190414, .time=19, .duration=2,  .rest_t=2,  .percent=-1,  .next=NULL};
	struct Event a4 = {.id="0004", .type=3, .name="Meeting",    .date=20190418, .time=20, .duration=2,  .rest_t=2,  .percent=-1,  .next=NULL};

    head = &a1;
	a1.next = &a2;
	a2.next = &a3;
	a3.next = &a4;
	int length = 4;
	head = Sort_By_Priority(head, length);
	struct Event* cur = head;
    /*for (int i = 0; i < 4; i++){
        printf("Event (id: %s, name: %s, type: %d)\n",cur->id, cur->name, cur->type);
        cur = cur->next;
    }*/
    Priority(head, 20190408, 20190421, 19, 23);
    return 0;
}

