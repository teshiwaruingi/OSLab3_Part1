#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv) {
  int pipefd1[2];
  int pipefd2[2];
  int pid1, pid2;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", "Lakers", NULL};
  char *sort_args[] = {"sort", NULL, NULL};

  // grep_args[1] = argv[1];
  // execvp("grep", grep_args);

  // make a pipe (fds go in pipefd[0] and pipefd[1])
  pipe(pipefd1); // pipe for cat to grep
  pipe(pipefd2); // pipe for grep and sort

  pid1 = fork();

  if (pid1 == 0) {
    pid2 = fork();
    if (pid2 == 0) {
      // run grep
      close(pipefd1[1]);
      close(pipefd2[0]);
      dup2(pipefd1[0], 0);
      dup2(pipefd2[1], 1);
      grep_args[1] = argv[1];
      execvp("grep", grep_args);

    } else {
      // run cat
      close(pipefd1[0]);
      close(pipefd2[0]);
      close(pipefd2[1]);
      dup2(pipefd1[1], 1);
      execvp("cat", cat_args);
    }
  } else {
    // run sort
    close(pipefd2[1]);
    close(pipefd1[0]);
    close(pipefd1[1]);
    dup2(pipefd2[0], 0);
    execvp("sort", sort_args);
  }
}