
#ifndef S3_H
#define S3_H

#define CHILD_NUM 1 //number of child
#define BUF_SIZE 100 //length of a buf

extern int fd_toC[CHILD_NUM][2];
extern int fd_toP[CHILD_NUM][2];

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <regex.h>

// our headers
#include "parser.h"
#include "s3.h"


#endif