#ifndef GREP_H
#define GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int error;
  char *pattern;
} param;

void parse(int argc, char *argv[], param *params);
void reader(int argc, char *argv[], param *params);
void output(char *argv[], param *params, regex_t regex, regmatch_t matches[1],
            int j, FILE *f, int filesCount);

#endif