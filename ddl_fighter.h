#ifndef DDL_FIGHTER_H
#define DDL_FIGHTER_H

// my headers
#include "parser.h"

#define ACCEPTED 0
#define REJECTED 1

void fight_ddl();
bool is_error(struct Event e);

#endif