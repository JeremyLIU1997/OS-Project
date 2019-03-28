
// macros
#define ASSIGNMENT_TYPE 0
#define PROJECT_TYPE 1
#define REVISION_TYPE 2
#define ACTIVITY_TYPE 3

// structs
struct Event {
	char id[5];
	int type;
	char name[30];
	int date;
	int time;
	int duration;
	int rest_t; // the remaining hours
	float percent; // -1 represents in valid
	struct Event* next;
};

// shared variables
extern struct Event events[1000]; // support at most 1000 events
extern int event_counter;
extern char command[1000][100];
extern int period_start_date;
extern int period_end_date;
extern int period_start_time;
extern int period_end_time;

// prototypes
void parse();
void print_event();
