#ifndef PR_H
#define PR_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "parser.h"
void Priority(struct Event* head,  int start_date, int end_date, int start_time, int end_time,int length, FILE* sch_result, FILE* log_file, FILE* summary);
void PR_invoker(struct Event events[1000], int length, int period_start_date, int period_end_date, int period_start_time, int period_end_time);

#endif
