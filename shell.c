#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096
#define PATH_MAX 4096
#define MAX_ARGS 64

char** parse_command(char *line) {
    char **tokens = calloc(MAX_ARGS, sizeof(char*));
    if (!tokens) {
        perror("calloc");
        return NULL;
    }
    char *buffer = strtok(line, " ");

    int counter = 0;

    while (buffer) {
        tokens[counter] =  strdup(buffer);
        counter++;
        buffer = strtok(NULL, " ");
    }

    tokens[counter] = NULL; // for the execvp
    return tokens;
}

void execute_command (char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();

  switch (pid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);
    case 0:
      execvp(args[0], args);

      // All statements are ignored after execvp() call as this whole 
      // process is replaced by another process
      perror("execvp");
      exit(EXIT_FAILURE);
    default:
      printf("Child is PID %d\n", pid); // This can be commented to prioritize a more realistic experience

    // puts the process to sleep and waits for the child to terminate
    if (wait(&status) == -1) {
      perror("waitpid");
      // exit(EXIT_FAILURE);
      return;
    }

    if (WIFEXITED(status)) printf("Child exited with code %d\n", WEXITSTATUS(status)); // This can be commented to prioritize a more realistic experience
    else if (WIFSIGNALED(status)) printf("Child terminated by signal %d\n", WTERMSIG(status)); // This can be commented to prioritize a more realistic experience
  }
}
// A static variable inside a function keeps its value between invocations.
void bultin_cd(char **args) {
  char target_dir[PATH_MAX];
  char curr_dir[PATH_MAX];
  static char prev_dir[PATH_MAX];

  if (!args[1]) { // wants to go to home dir
    char *value = getenv("HOME");

    if (!value) {
      perror("HOME directory is not set in environment variables list");
      return;
    }

    strncpy(target_dir, value, sizeof(target_dir));
  }
  else if (strcmp(args[1], "-") == 0) {

    if(prev_dir[0] == '\0') {
      perror("nash: cd: OLDPWD not set\n");
      return;
    }
    strncpy(target_dir, prev_dir, sizeof(target_dir));
    printf("%s\n", target_dir);
  } else {
    strncpy(target_dir, args[1], sizeof(target_dir));
  }

  char *curr_path = getcwd(curr_dir, sizeof(curr_dir));
  if (!curr_path) {
    perror("nash: cd");
    return;
  }

  if (chdir(target_dir) == -1) {
    perror("cd");
    return;
  }
  strncpy(prev_dir, curr_dir, sizeof(prev_dir));
  free(curr_path);
}

void builtin_pwd() {
  char curr_path[PATH_MAX];
  char *path = getcwd(curr_path, sizeof(curr_path));
  if (!path) {
    perror("getcwd");
  }
  printf("%s\n", path);
  free(path);
}

int main(int argc, char *argv[]) {
  int is_shell_running = 1;
  char buffer[BUFFER_SIZE];

  // REPL (read-eval-print-loop)
  while(is_shell_running) {
    printf("nash> ");
    fflush(stdout);
    char *read_line = fgets(buffer, BUFFER_SIZE, stdin);
    if (!read_line) {
      break;
    }
    read_line[strcspn(read_line, "\n")] = 0;
    if (strcmp(read_line, "exit") == 0) {
      break;
    }

    char **args = parse_command(read_line);
    if (args[0] != NULL) {
      if (strcmp(args[0], "cd") == 0) bultin_cd(args);
      else if (strcmp(args[0], "pwd") == 0) builtin_pwd();
      else execute_command(args);
    }

    free(args);
  }

  printf("\ngoodbye, sailor...\n");
  exit(EXIT_SUCCESS);
}
