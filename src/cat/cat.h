#ifndef CAT_H
#define CAT_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int error;
} param;

void parse(int argc, char *argv[], param *params);
long myGetline(char **line, size_t *n, FILE *f);
void reader(int argc, char *argv[], param *params);
void output(FILE *f, param *params);

#endif