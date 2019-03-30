#ifndef RR_H
#define RR_H

#include "parser.h"
int print_slots_alloc(struct Event* head, int cur_time, int slots_elapsed, int start_time, int end_time, FILE* sch_result);
void Round_Robin(int q, struct Event* head, struct Event* tail, int start_date, int end_date, int start_time, int end_time, FILE* sch_result, FILE* log_file, FILE* summary, int total_requests);

#endif