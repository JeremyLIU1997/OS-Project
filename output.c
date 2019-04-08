#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <string.h>

#include "parser.h"

/* prototypes */
void toDateFormat(char *newstr, char *oldstr);
void output(char *summary_file, char *algorithm, char *timetable_file);

/* convert date format, such as "20190408" to "2019-04-08" */
void toDateFormat(char *newstr, char *oldstr) {
	strncpy(newstr, oldstr, 4);
    newstr[4]='-';
    strncpy(newstr+5, oldstr+4, 2);
    newstr[7]='-';
    strncpy(newstr+8, oldstr+6, 2);
    newstr[10]='\0';
}

/* Print formatted timetable */
void output(char *summary_file, char *algorithm, char *timetable_file) {
	int i, j, k;
	int date, time;
	char startStr1[9], endStr1[9], lastDate[9];
	char startStr2[11], endStr2[11], dateStr[11];
	char timeStr[6];
	char lineStr[80], path[50];
	char *token;
	FILE *timetable = fopen(timetable_file, "w");
	FILE *fp = fopen(summary_file, "r");
	if (fp == NULL) {
		printf("Cannot open the file!\n"); 
		exit(1);
	}
	
	fprintf(timetable, "Alice Timetable\n");
	sprintf(startStr1, "%d", period_start_date); 
	toDateFormat(startStr2, startStr1);
	sprintf(endStr1, "%d", period_end_date);
	toDateFormat(endStr2, endStr1);
	fprintf(timetable, "Period: %s to %s\nAlgorithm: %s\n\n%-25s", startStr2, endStr2, algorithm, "Date");
	for (i=DAY_START; i<DAY_START+HOUR_PER_DAY; i++) {
		sprintf(timeStr, "%d", i);
		strcpy(timeStr+2, ":00");
		fprintf(timetable, "%-20s", timeStr);
	}
	
	time = DAY_START;
	fprintf(timetable, "\n%-25s", startStr2);
	strcpy(lastDate, startStr1);
	while(fscanf(fp, "%[^\n]\n", lineStr) != EOF) {
		token = strtok(lineStr, " ");
		k=0;
		if (strcmp(token,lastDate)!=0) { // different date
			while (time < DAY_END) {
				fprintf(timetable, "%-20s", "N/A");
				time++;
			}
			while ((lastDate[6]-'0')*10+(lastDate[7]-'0')+1 < (token[6]-'0')*10+(token[7]-'0')) {
				date = atoi(lastDate)+1;
				sprintf(lastDate, "%d", date);
				toDateFormat(dateStr, lastDate);
				fprintf(timetable, "\n%-25s%-20s%-20s%-20s%-20s", dateStr,"N/A","N/A","N/A","N/A");
			}
			toDateFormat(dateStr, token);
			fprintf(timetable, "\n%-25s", dateStr);
			time = DAY_START;
		}
		strcpy(lastDate, token);
		while( token != NULL ) {
			token = strtok(NULL, " ");
			k++;
			switch (k) {
				case 1:
					while (time < atoi(token)) { // different time
						fprintf(timetable, "%-20s", "N/A"); 
						time++;
					}
					break;
				case 3:
					fprintf(timetable, "%-20s", token);
					time++;
					break;
			}
		}
	}
	
	while (strcmp(endStr1,lastDate)!=0) { // until period end date
		while (time < DAY_END) {
			fprintf(timetable, "%-20s", "N/A");
			time++;
		}
		date = atoi(lastDate)+1;
		sprintf(lastDate, "%d", date);
		toDateFormat(dateStr, lastDate);
		fprintf(timetable, "\n%-25s", dateStr);
		time = DAY_START;
	}
	while (time < DAY_END) { // until DAY_END time
		fprintf(timetable, "%-20s", "N/A");
		time++;
	}
	fprintf(timetable, "\n");
	
	fclose(fp);
	fclose(timetable);
}