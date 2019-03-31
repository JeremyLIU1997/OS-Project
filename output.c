#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <string.h>

#include <parser.h>

#define LINE_SIZE 50

void toDateFormat(char *newstr, char *oldstr);
void output(char *filename, char *algorithm);


void toDateFormat(char *newstr, char *oldstr) {
	strncpy(newstr, oldstr, 4);
    newstr[4]='-';
    strncpy(newstr+5, oldstr+4, 2);
    newstr[7]='-';
    strncpy(newstr+8, oldstr+6, 2);
    newstr[10]='\0';
}

void output(char *filename, char *algorithm) {
	int i, j, k;
	int date, time;
	char startStr1[9], endStr1[9], lastDate[9];
	char startStr2[11], endStr2[11], dateStr[11];
	char timeStr[6];
	char lineStr[LINE_SIZE];
	char *token;
	FILE *fp;
	
	int period_start_date=20190408; //to be deleted
	int period_end_date=20190421; //to be deleted
	
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open the file!\n"); 
		exit(1);
	}
	
	printf("Alice Timetable\n");
	sprintf(startStr1, "%d", period_start_date); 
	toDateFormat(startStr2, startStr1);
	sprintf(endStr1, "%d", period_end_date);
	toDateFormat(endStr2, endStr1);
	printf("Period: %s to %s\n", startStr2, endStr2);
	printf("Algorithm: %s\n\n", algorithm);
	printf("%-25s", "Date");
	for (i=DAY_START; i<DAY_START+HOUR_PER_DAY; i++) {
		sprintf(timeStr, "%d", i);
		strcpy(timeStr+2, ":00");
		printf("%-20s", timeStr);
	}
	
	time = DAY_START;
	printf("\n%-25s", startStr2);
	strcpy(lastDate, startStr1);
	while(fscanf(fp, "%[^\n]\n", lineStr) != EOF) {
		token = strtok(lineStr, " ");
		k=1;
		if (strcmp(token,lastDate)!=0) { // different date
			while (time < DAY_END) {
				printf("%-20s", "N/A");
				time++;
			}
			toDateFormat(dateStr, token);
			printf("\n%-25s", dateStr);
			time = DAY_START;
		}
		strcpy(lastDate, token);
		
		while( token != NULL ) {
			token = strtok(NULL, " ");
			k++;
			
			switch (k) {
				case 2:
					while (time < atoi(token)) { // different time
						printf("%-20s", "N/A"); 
						time++;
					}
					break;
				case 4:
					printf("%-20s", token);
					time++;
					break;
			}
		}
	}
	
	while (strcmp(endStr1,lastDate)!=0) { // until period end date
		while (time < DAY_END) {
			printf("%-20s", "N/A");
			time++;
		}
		date = atoi(lastDate)+1;
		sprintf(lastDate, "%d", date);
		toDateFormat(dateStr, lastDate);
		printf("\n%-25s", dateStr);
		time = DAY_START;
	}
	while (time < DAY_END) { // until DAY_END time
		printf("%-20s", "N/A");
		time++;
	}
	
}


int main() {
	output("RR_result", "RR");
	
	return 0;
}