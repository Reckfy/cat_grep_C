#include "grep.h"

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

  while ((option = getopt(argc, argv, "+e:ivclnhs")) != -1) {
    switch (option) {
      case 'e':
        params->e = 1;
        params->pattern = optarg;
        break;
      case 'i':
        params->i = 1;
        break;
      case 'v':
        params->v = 1;
        break;
      case 'c':
        params->c = 1;
        break;
      case 'l':
        params->l = 1;
        break;
      case 'n':
        params->n = 1;
        break;
      case 's':
        params->s = 1;
        break;
      case 'h':
        params->h = 1;
        break;
      default:
        fprintf(stderr, "Usage flags -e -i -v -c -l -n -h\n");
        option = -1;
        params->error = 1;
    }
  }
  if (!params->e && optind < argc) {
    params->pattern = argv[optind++];
  }
}

void reader(int argc, char *argv[], param *params) {
  int filesCount = argc - optind;
  for (int j = optind; j < argc; j++) {
    int flag = 0;
    FILE *f = fopen(argv[j], "r");
    if (!f && params->s == 0) {
      fprintf(stderr, "Failed open file: %s\n", argv[j]);
      flag = 1;
    }
    if (!flag) {
      regex_t regex;
      regmatch_t matches[1];
      if ((regcomp(&regex, params->pattern, (params->i ? REG_ICASE : 0)) !=
           0)) {
        fprintf(stderr, "Fail compile regex\n");
        flag = 1;
      }
      if (!flag) {
        output(argv, params, regex, matches, j, f, filesCount);
      }
      regfree(&regex);
      fclose(f);
    }
  }
}

void output(char *argv[], param *params, regex_t regex, regmatch_t matches[1],
            int j, FILE *f, int filesCount) {
  char line[1024];
  int stop = 0, lineNumber = 0, matchCount = 0, lastLine = 1, concurrency = 0;
  while (!stop && fgets(line, sizeof(line), f)) {
    lineNumber++;
    int is_match = regexec(&regex, line, 1, matches, 0);
    if ((params->v && is_match == REG_NOMATCH) ||
        (!params->v && is_match == 0)) {
      matchCount++;
      if (matchCount > 0) concurrency = 1;
      if (filesCount > 1 && !params->l && !params->h && !params->c) {
        printf("%s:", argv[j]);
      }
      if (params->n && !params->l && !params->c) {
        printf("%d:", lineNumber);
      }
      if (params->l && !params->c) {
        printf("%s\n", argv[j]);
        stop = 1;
      }
      if (!stop) {
        if (!params->c) printf("%s", line);
        lastLine = line[strlen(line) - 1] == '\n' ? 1 : 0;
      }
    }
  }
  if (params->c && filesCount > 1 && !params->l) {
    if (!params->h) {
      printf("%s:", argv[j]);
    }
    printf("%d\n", matchCount);
  } else if (params->c && !params->l) {
    printf("%d\n", matchCount);
  }
  if (params->c && params->l && concurrency > 0 && filesCount > 1) {
    printf("%s:", argv[j]);
    printf("%d\n", concurrency);
    printf("%s\n", argv[j]);
  } else if (params->c && params->l && concurrency > 0) {
    printf("%d\n", concurrency);
    printf("%s\n", argv[j]);
  } else if (params->c && params->l) {
    printf("%d\n", concurrency);
  }
  if (!lastLine && !params->c) {
    printf("\n");
  }
}