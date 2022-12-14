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
  int pipefd1[2]; // Pipe for cat to grep
int pipefd2[2]; // Pipe for grep to sort
int pid1, pid2;

// Command and arguments for cat
char *cat_args[] = {"cat", "scores", NULL};

// Command and arguments for grep, with space for user input
char *grep_args[] = {"grep", NULL, NULL};

// Command and arguments for sort
char *sort_args[] = {"sort", NULL, NULL};

// User input for grep
grep_args[1] = argv[1];

// make pipes for cat to grep and grep to sort
pipe(pipefd1);
pipe(pipefd2);

// fork a new process for the first command
pid1 = fork();

  int pid1, pid2;

// Create pipes
int pipefd1[2];
int pipefd2[2];
pipe(pipefd1);
pipe(pipefd2);

pid1 = fork();

if (pid1 == 0) {
    // Create another child process
    pid2 = fork();

    if (pid2 == 0) {
        // This is the child process running grep

        // Close unused ends of pipes
        close(pipefd1[1]);
        close(pipefd2[0]);

        // Redirect standard input and output to pipes
        dup2(pipefd1[0], 0);
        dup2(pipefd2[1], 1);

        // Set grep argument and run grep
        grep_args[1] = argv[1];
        execvp("grep", grep_args);

    } else {
        // This is the original child process running cat

        // Close unused ends of pipes
        close(pipefd1[0]);
        close(pipefd2[0]);
        close(pipefd2[1]);

        // Redirect standard output to pipe
        dup2(pipefd1[1], 1);

        // Run cat
        execvp("cat", cat_args);
    }
} else {
    // This is the parent process running sort

    // Close unused ends of pipes
    close(pipefd2[1]);
    close(pipefd1[0]);
    close(pipefd1[1]);

    // Redirect standard input to pipe
    dup2(pipefd2[0], 0);

    // Run sort
    execvp("sort", sort_args);
}

}
