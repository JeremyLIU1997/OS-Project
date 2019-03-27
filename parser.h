
// macros
#define ASSIGNMENT_TYPE 0
#define PROJECT_TYPE 1
#define REVISION_TYPE 2
#define ACTIVITY_TYPE 3

// structs
struct event {
	int type;
	char name[30];
	int date;
	int time;
	int duration;
};

// functions
void print_event();
