
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


// shared variables
extern struct event events[1000]; // support at most 1000 events
extern int event_counter;
extern char command[1000][100];

// prototypes
void parse();
void print_event();
