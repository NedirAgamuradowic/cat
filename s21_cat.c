#include "s21_cat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  Check status = {0};
  Flag flags = {0};
  char *file_array[MAX_PATTERN];
  detect_macOS(&status);
  status.counter = 1;
  if (argc == 1) {
    terminal_input(&status, &flags);
  } else if (argc == 2 &&
             (strcmp(argv[1], "-") == 0 || strcmp(argv[1], "--") == 0)) {
    terminal_input(&status, &flags);
  } else {
    parse_command_line_arguments(&status, &flags, argv, file_array, argc);
  }
  return 0;
}

// Using this function to know if this macOS system. Becuase cat in linux and
// macos works different
void detect_macOS(Check *status) {
#ifdef __APPLE__
  status->macOS = 1;
#else
  status->macOS = 0;
#endif
}

// With this function parse arguments by loop and identtify it(hyphen, flag,
// file)
void parse_command_line_arguments(Check *status, Flag *flags, char *argv[],
                                  char *file_array[], int argc) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-") == 0) {
      adding_files_to_array(status, file_array, argv[i]);
    } else if (argv[i][0] == '-' && strlen(argv[i]) >= 2) {
      parse_flag_arguments(status, flags, file_array, argv[i]);
    } else {
      adding_files_to_array(status, file_array, argv[i]);
    }
  }
  process_after_loop(status, flags, file_array);
}

// With this function if there is file or hyphen. Then add this elements to
// array.
void adding_files_to_array(Check *status, char *file_array[], char *file) {
  status->output = 1;
  if (status->file_number < MAX_PATTERN) {
    file_array[status->file_number++] = file;
  }
}

// With this function we will display the elements in the array on the
// terminal.(If it is hyphen then work with stdin)
void process_after_loop(Check *status, const Flag *flags, char *file_array[]) {
  if (status->file_number >= 1) {
    for (int i = 0; i < status->file_number; i++) {
      if (strcmp(file_array[i], "-") == 0) {
        if (status->exit == 0) terminal_input(status, flags);
      } else {
        if (status->exit == 0) check_path(status, flags, file_array[i]);
      }
    }
  } else {
    if (status->exit == 0) terminal_input(status, flags);
  }
}

// With this function we checking condition for macOS or linux systems for
// flags.
void parse_flag_arguments(Check *status, Flag *flags, char *file_array[],
                          char *flag) {
  if (status->macOS) {
    if (status->output == 0 && status->path_error == 0 &&
        status->double_hyphen == 0) {
      identify_flags(status, flags, file_array, flag);
    } else {
      adding_files_to_array(status, file_array, flag);
    }
  } else {
    if (status->double_hyphen == 0) {
      identify_flags(status, flags, file_array, flag);
    } else {
      adding_files_to_array(status, file_array, flag);
    }
  }
}

// With this function we identify what type of flag it is(Long, short, double
// hyphen or another)
void identify_flags(Check *status, Flag *flags, char *file_array[],
                    char *flag) {
  if (flag[1] == '-' && strlen(flag) > 2) {
    long_flags(status, flags, flag);
  } else if (flag[1] == '-' && strlen(flag) == 2) {
    status->double_hyphen = 1;
  } else if (flag[1] != '-' && strlen(flag) > 1) {
    short_flags(status, flags, flag);
  } else {
    adding_files_to_array(status, file_array, flag);
  }
}

// With this function we identify what type of long flag it is and change their
// status for 1.
void long_flags(Check *status, Flag *flags, const char *flag) {
  if (status->macOS == 0) {
    if (strcmp(flag, "--number") == 0) {
      set_n_flag(&flags->b, &flags->n, &status->flag);
    } else if (strcmp(flag, "--number-nonblank") == 0) {
      set_b_flag(&flags->n, &flags->b, &status->flag);
    } else if (strcmp(flag, "--squeeze-blank") == 0) {
      set_flag(&flags->s, &status->flag);
    } else {
      status->flag_error = 1;
      error_message(status, flag, ILLEGAL);
      status->exit = 1;
    }
  } else {
    error_message(status, flag, ILLEGAL);
    status->exit = 1;
  }
}

// With this function we identify what type of short flag it is and change their
// status for 1.
void short_flags(Check *status, Flag *flags, char *flag) {
  for (int i = 1; flag[i] != '\0'; i++) {
    switch (flag[i]) {
      case 'n':
        set_n_flag(&flags->b, &flags->n, &status->flag);
        break;
      case 'b':
        set_b_flag(&flags->n, &flags->b, &status->flag);
        break;
      case 's':
        set_flag(&flags->s, &status->flag);
        break;
      case 'e':
        set_two_flags(&flags->e, &flags->v, &status->flag);
        break;
      case 't':
        set_two_flags(&flags->t, &flags->v, &status->flag);
        break;
      case 'v':
        set_flag(&flags->v, &status->flag);
        break;
      case 'E':
        set_uppercase_flag(status, flags, &flag[i]);
        break;
      case 'T':
        set_uppercase_flag(status, flags, &flag[i]);
        break;
      default:
        status->flag_error = 1;
        error_message(status, flag, ILLEGAL);
        status->exit = 1;
    }
  }
}

// With this function if it is path we check existence of path, what type of
// path it is(file, directory, other type file)
void check_path(Check *status, const Flag *flags, char *path) {
  struct stat checkPath;

  if (stat(path, &checkPath) == 0) {
    if (S_ISREG(checkPath.st_mode)) {
      file_input(status, flags, path);
    } else if (S_ISDIR(checkPath.st_mode)) {
      status->path_error = 1;
      error_message(status, path, DIRECTORY);
    } else {
      status->path_error = 1;
      printf(OTHER "\n");
    }
  } else {
    status->path_error = 1;
    error_message(status, path, PATH);
  }
}

// With this function file if it is file we open this file and starts read file
// with getchar() function
void file_input(Check *status, const Flag *flags, const char *path) {
  add_newline_if_not(status, path);
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    char ch;
    if (status->macOS) status->counter = 1;
    while ((ch = fgetc(file)) != EOF) {
      output(status, flags, ch, &status->counter);
    }
    fclose(file);
  } else {
    status->path_error = 1;
    error_message(status, path, DENIED);
  }
}

// With this function if there is hyphen or only flags we works with stdin
void terminal_input(Check *status, const Flag *flags) {
  char ch;
  if (status->macOS) status->counter = 1;
  while ((ch = getchar()) != EOF) {
    output(status, flags, ch, &status->counter);
  }
}

// With this function we output results based on flags  status
void output(Check *status, const Flag *flags, char c, int *counter) {
  status->output = 1;
  if (flags->s == 1) {
    if (c != '\n') {
      status->visible++;
    }
    if (c == '\n') {
      if (status->visible == 0) {
        status->empty_line++;
      } else {
        status->empty_line = 0;
      }
      if (status->empty_line > 1) {
        return;
      }
      status->visible = 0;
    }
  }
  change_with_n(status, flags, c, counter);
  change_with_b(status, flags, c, counter);
  if (flags->v || flags->t || flags->e) {
    show_unprinted_chars(flags, c);
  } else {
    show_char(flags, c);
  }
}

// With this function we print unprinatble characters based on ASCII value
void show_unprinted_chars(const Flag *flag, int c) {
  if (c < 32 && c != 9 && c != 10) {
    printf("^%c", c + 64);
  } else if (c == 127) {
    printf("^?");
  } else if (c > 127 && c < 160) {
    printf("M-^%c", c - 64);
  } else if (c >= 160 && c != 255) {
    printf("M-%c", c - 128);
  } else if (c == 255) {
    printf("M-^?");
  } else {
    show_char(flag, c);
  }
}

// With this function if there is e print $ sign based on newline element, if
// there is t print  ^I based on tab
void show_char(const Flag *flag, char c) {
  if (c == '\n' && flag->e == 1) {
    printf("$\n");
  } else if (flag->t == 1 && (int)c == 9) {
    printf("^I");
  } else {
    putchar(c);
    fflush(stdout);
  }
}

// With this function we numbering lines based on n flag status
void change_with_n(Check *status, const Flag *flags, char c, int *counter) {
  if (flags->n == 1 && flags->b == 0) {
    if (status->start_line == 0) {
      printf("%6d\t", (*counter));
      status->start_line = 1;
    }
    if (c == '\n') {
      status->start_line = 0;
      (*counter)++;
    }
  }
}

// With this function we numbering line where has text based on b flag
void change_with_b(Check *status, const Flag *flags, char c, int *counter) {
  if (flags->b == 1) {
    if (status->start_line == 0 && c != '\n') {
      printf("%6d\t", (*counter));
      (*counter)++;
      status->start_line = 1;
    }
    if (status->start_line == 0 && flags->e == 1) {
      char space = ' ';
      if (status->macOS) printf("%6c\t", space);
    }
    if (c == '\n') {
      status->start_line = 0;
    }
  }
}

// With this function if there is uppercase shor flags check system and then
// change their status
void set_uppercase_flag(Check *status, Flag *flags, const char *flag) {
  if (status->macOS == 0) {
    if (*flag == 'E') {
      set_flag(&flags->e, &status->flag);
    } else {
      set_flag(&flags->t, &status->flag);
    }
  } else {
    status->flag_error = 1;
    error_message(status, flag, ILLEGAL);
    status->exit = 1;
  }
}

// With this function if there is n flag check b flag status then chnage n flag
// status(b flag change n flag status to zero)
void set_n_flag(const int *dependent_flag, int *main_flag, int *flag_status) {
  if (*dependent_flag == 0 && *main_flag == 0) {
    *main_flag = 1;
    *flag_status = 1;
  }
}

// With this function if there is b flag change n flag status to zero and then
// change b flag status to 1
void set_b_flag(int *n_flag, int *main_flag, int *flag_status) {
  if (*main_flag == 0) {
    *n_flag = 0;
    *main_flag = 1;
    *flag_status = 1;
  }
}

// With this function we change flag status for 1
void set_flag(int *flag, int *flag_status) {
  if (*flag == 0) {
    *flag = 1;
    *flag_status = 1;
  }
}

// With this function we change two flag status for 1. (If it is e and t flags
// they activite v flag too)
void set_two_flags(int *main_flag, int *second_flag, int *flag_status) {
  if (*main_flag == 0) {
    *main_flag = 1;
    *second_flag = 1;
    *flag_status = 1;
  }
}

// With this function we checking file end for newline element. if there is not
// newline element we add newline element
void add_newline_if_not(Check *status, const char *path) {
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    char ch;
    char last_char = '\0';
    while ((ch = fgetc(file)) != EOF) {
      last_char = ch;
    }
    fclose(file);
    if (last_char != '\n') {
      file = fopen(path, "a");
      if (file != NULL) {
        fputc('\n', file);
        fclose(file);
      } else {
        status->path_error = 1;
        error_message(status, path, DENIED);
      }
    }
  } else {
    status->path_error = 1;
    error_message(status, path, DENIED);
  }
}

// With this function we gives error messages when error happens
void error_message(const Check *status, const char *path, const char *type) {
  if (status->flag_error) {
    fprintf(stderr, "cat: %s %s\n", type, path);
  } else {
    fprintf(stderr, "cat: %s: %s\n", path, type);
  }
}
