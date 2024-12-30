#ifndef S21_CAT_H
#define S21_CAT_H

#define DIRECTORY "Is a directory"
#define OTHER "Other type of file"
#define PATH "No such file or directory"
#define DENIED "Permission denied"
#define ILLEGAL "Illegal option --"

#define MAX_PATTERN 100

typedef struct {
  int flag;
  int output;
  int start_line;
  int visible;
  int empty_line;
  int double_hyphen;
  int flag_error;
  int path_error;
  int exit;
  int macOS;
  int file_number;
  int counter;
} Check;

typedef struct {
  int n;
  int b;
  int e;
  int t;
  int s;
  int v;
} Flag;

void detect_macOS(Check *status);

void parse_command_line_arguments(Check *status, Flag *flags, char *argv[],
                                  char *file_array[], int argc);

void parse_flag_arguments(Check *status, Flag *flags, char *file_array[],
                          char *flag);

void identify_flags(Check *status, Flag *flags, char *file_array[], char *flag);

void process_after_loop(Check *status, const Flag *flags, char *file_array[]);

void long_flags(Check *status, Flag *flags, const char *flag);

void short_flags(Check *status, Flag *flags, char *flag);

void check_path(Check *status, const Flag *flags, char *path);

void file_input(Check *status, const Flag *flags, const char *path);

void terminal_input(Check *status, const Flag *flags);

void output(Check *status, const Flag *flags, char c, int *counter);

void show_unprinted_chars(const Flag *flag, int c);

void show_char(const Flag *flag, char c);

void change_with_n(Check *status, const Flag *flags, char c, int *counter);

void change_with_b(Check *status, const Flag *flags, char c, int *counter);

void set_uppercase_flag(Check *status, Flag *flags, const char *flag);

void set_n_flag(const int *dependent_flag, int *main_flag, int *flag_status);

void set_b_flag(int *n_flag, int *main_flag, int *flag_status);

void set_flag(int *flag, int *flag_status);

void set_two_flags(int *main_flag, int *second_flag, int *flag_status);

void adding_files_to_array(Check *status, char *file_array[], char *file);

void add_newline_if_not(Check *status, const char *path);

void error_message(const Check *status, const char *path, const char *type);

#endif
