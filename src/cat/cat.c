#include "cat.h"

int main(int argc, char *argv[]) {
  param params = {0};
  parse(argc, argv, &params);
  if (!params.error) {
    reader(argc, argv, &params);
  }
  return 0;
}

void parse(int argc, char *argv[], param *params) {
  int option;
  int param_id;
  static struct option longParams[] = {{"number-nonblank", 0, 0, 'b'},
                                       {"number", 0, 0, 'n'},
                                       {"squeeze-blank", 0, 0, 's'},
                                       {0, 0, 0, 0}};
  while ((option = getopt_long(argc, argv, "+benstvTE", longParams,
                               &param_id)) != -1) {
    switch (option) {
      case 'n':
        if (params->b != 1) {
          params->n = 1;
        }
        break;
      case 'b':
        params->b = 1;
        params->n = 0;
        break;
      case 'e':
        params->e = 1;
        params->v = 1;
        break;
      case 's':
        params->s = 1;
        break;
      case 't':
        params->t = 1;
        params->v = 1;
        break;
      case 'v':
        params->v = 1;
        break;
      case 'T':
        params->t = 1;
        break;
      case 'E':
        params->e = 1;
        break;
      default:
        fprintf(stderr, "Usage flags -b -e -n -s -t -v\n");
        option = -1;
        params->error = 1;
    }
  }
}

void reader(int argc, char *argv[], param *params) {
  for (int j = optind; j < argc; j++) {
    FILE *f = fopen(argv[j], "rb");
    if (f) {
      output(f, params);
    } else {
      fprintf(stderr, "No such file in this directory.\n");
    }
  }
}

void output(FILE *f, param *params) {
  char *line = NULL;
  size_t len = 0;
  int strCount = 0;
  int emptyLine = 0;
  int line_len;
  while ((line_len = myGetline(&line, &len, f)) != -1) {
    int flag = 0;
    int checkEmpty = (line_len == 1);
    if (params->s && checkEmpty) {
      if (emptyLine) {
        flag = 1;
      } else {
        emptyLine = 1;
      }
    } else {
      emptyLine = 0;
    }
    for (int i = 0; i < line_len && !flag; i++) {
      char current = line[i];
      if (params->b && i == 0 && line[0] != '\n') {
        strCount++;
        printf("%6d\t", strCount);
      } else if (params->n && i == 0) {
        strCount++;
        printf("%6d\t", strCount);
      }
      if (params->e && current == '\n') {
        if (params->b && line[0] == '\n') {
          printf("%6s\t", "");
        }
        printf("$\n");
      } else if (params->t && current == '\t') {
        printf("^I");
      } else if (params->v && current != '\n' && current != '\t') {
        if (current < 32) {
          printf("^%c", current + 64);
        } else if (current == 127) {
          printf("^?");
        } else {
          printf("%c", current);
        }
      } else {
        printf("%c", current);
      }
    }
  }
  free(line);
  fclose(f);
}

long myGetline(char **line, size_t *n, FILE *f) {
  int flag = 1;
  if (line == NULL || n == NULL || f == NULL) {
    flag = -1;
  }
  size_t pos = 0;
  int c;
  if (*line == NULL || *n == 0) {
    *n = 128;
    *line = malloc(*n);
    if (*line == NULL) {
      flag = -1;
    }
  }
  int stop = 0;
  while (!stop && ((c = fgetc(f)) != EOF)) {
    if (pos + 1 >= *n) {
      *n *= 2;
      char *ptr = realloc(*line, *n);
      if (ptr == NULL) {
        flag = -1;
      } else
        *line = ptr;
    }

    (*line)[pos++] = c;
    if (c == '\n') {
      stop = 1;
    }
  }
  if (pos == 0 && c == EOF) {
    flag = -1;
  }
  (*line)[pos] = '\0';
  if (flag == -1) {
    pos = -1;
  }
  return pos;
}