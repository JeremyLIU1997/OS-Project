#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <string.h>
#include "parser.h"

void analyzer();
float scoring(char *filename);


void analyzer() {
	char *pr_filename = "./output/PR_result";
	char *rr_filename = "./output/RR_result";
	char *ddl_filename = "./output/ddl_fighter_result";
	FILE *fp = fopen("./output/analyzer_summary.txt", "w");
	
	float pr_benefit = scoring(pr_filename);
	float rr_benefit = scoring(rr_filename);
	float ddl_benefit = scoring(ddl_filename);
	
	fprintf(fp, "%-35s%-35s\n-------------------------------------------\n", "Algorithm", "Benefit");
	if (pr_benefit >= 0)
		fprintf(fp, "%-35s%-35.2f\n", "Priority", pr_benefit);
	if (rr_benefit >= 0)
		fprintf(fp, "%-35s%-35.2f\n", "Round Robin", rr_benefit);
	if (ddl_benefit >= 0)
		fprintf(fp, "%-35s%-35.2f\n", "Deadline Fighter Algorithm", ddl_benefit);
	fprintf(fp, "-------------------------------------------\n");
	
	if (pr_benefit < 0 && rr_benefit < 0 && ddl_benefit < 0) {
		fclose(fp);
		return;
	}
	if (ddl_benefit >= pr_benefit && ddl_benefit >= rr_benefit)
		fprintf(fp, "Best Algorithm: Deadline Fighter Algorithm\n");
	else if (rr_benefit >= pr_benefit && rr_benefit >= ddl_benefit)
		fprintf(fp, "Best Algorithm: Round Robin\n");
	else 
		fprintf(fp, "Best Algorithm: Priority\n");
	
	fclose(fp);
}

float scoring(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
		return -1.0;
	
	int k;
	int type, duration, level;
	float unit_benefit;
	float benefit = 0;
	char name[30];
	char lineStr[80];
	char *token;
	while(fscanf(fp, "%[^\n]\n", lineStr) != EOF) {
		token = strtok(lineStr, " ");
		k=0;
		while( token != NULL ) {
			token = strtok(NULL, " ");
			k++;
			switch (k) {
				case 3: //name
					strcpy(name, token);
					break;
				case 4: //type
					type = atoi(token);
					break;
				case 5: //duration
					duration = atoi(token);
					break;
			}
		}
		level = parse_level(name);
		
		if (type == PROJECT_TYPE)
			unit_benefit = PROJECT_BASE + (level - 1) * LEVEL_UP_POINT;
		else if (type == ASSIGNMENT_TYPE)
			unit_benefit = ASSIGNMENT_BASE + (level - 1) * LEVEL_UP_POINT;
		else if (type == REVISION_TYPE)
			unit_benefit = REVISION_BASE + (level - 1) * LEVEL_UP_POINT;
		else 
			unit_benefit = ACTIVITY_BASE;
		unit_benefit /= duration;
		benefit += unit_benefit;
	}
	
	fclose(fp);
	return benefit;
}