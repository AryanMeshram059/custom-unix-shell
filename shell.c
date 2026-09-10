#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <errno.h>
#include <signal.h>

#define ROLL_NO "24110054"
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 100

pid_t active_child_pid = -1;

struct termios original_terminal;

volatile sig_atomic_t ctrl_q_pressed = 0;



void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal);
}


void ctrl_q_handler(int sig) {
    (void)sig;

    ctrl_q_pressed = 1;
}


void cleanup_children() {
    if (active_child_pid > 0) {

        kill(active_child_pid, SIGTERM);

       
        while (waitpid(active_child_pid, NULL, 0) == -1 && errno == EINTR) {
        }

        active_child_pid = -1;
    }
}

void exit_shell() {
    cleanup_children();

    restore_terminal();

    printf("\nExiting shell...\n");

    exit(0);
}


void builtin_kill(char *args[]) {
    if (args[1] == NULL) {
        printf("kill: missing PID\n");
        return;
    }

    pid_t pid = (pid_t)atoi(args[1]);

    if (kill(pid, SIGTERM) != 0) {
        perror("kill");
    }
}

void execute_external_command(char *args[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
      

        execvp(args[0], args);

        perror("execvp");

        exit(1);
    }
    else {
   
        active_child_pid = pid;


        while (waitpid(pid, NULL, 0) == -1) {

       
            if (errno == EINTR && ctrl_q_pressed) {
                exit_shell();
            }

            if (errno != EINTR) {
                perror("waitpid");
                break;
            }
        }

        active_child_pid = -1;


        if (ctrl_q_pressed) {
            exit_shell();
        }
    }
}


void builtin_exit() {
    exit_shell();
}


void builtin_cd(char *args[]) {
    if (args[1] == NULL) {
        printf("cd: missing argument\n");
        return;
    }

    if (chdir(args[1]) != 0) {
        perror("cd");
    }
}


void builtin_echo(char *args[]) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s", args[i]);

        if (args[i + 1] != NULL) {
            printf(" ");
        }
    }

    printf("\n");
}


void builtin_pwd() {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    }
    else {
        perror("pwd");
    }
}


int main() {

    char cwd[PATH_MAX];
    char input[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS];


    if (tcgetattr(STDIN_FILENO, &original_terminal) == -1) {
        perror("tcgetattr");
        exit(1);
    }


    atexit(restore_terminal);


    struct termios new_terminal = original_terminal;

    new_terminal.c_iflag &= ~IXON;

    new_terminal.c_cc[VINTR] = 17;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal) == -1) {
        perror("tcsetattr");
        exit(1);
    }

    struct sigaction sa;

    sa.sa_handler = ctrl_q_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }


    while (1) {

        if (ctrl_q_pressed) {
            exit_shell();
        }

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s:%s$ ", ROLL_NO, cwd);
        }
        else {
            perror("getcwd");
        }

        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {

            // Ctrl+Q interrupted fgets()
            if (ctrl_q_pressed) {
                exit_shell();
            }

            break;
        }


        input[strcspn(input, "\n")] = '\0';


        // Parse command and arguments
        int i = 0;

        char *token = strtok(input, " \t");

        while (token != NULL && i < MAX_ARGS - 1) {
            args[i] = token;
            i++;

            token = strtok(NULL, " \t");
        }


        args[i] = NULL;

        if (args[0] == NULL) {
            continue;
        }

        if (strcmp(args[0], "pwd") == 0) {
            builtin_pwd();
        }

        else if (strcmp(args[0], "echo") == 0) {
            builtin_echo(args);
        }

        else if (strcmp(args[0], "cd") == 0) {
            builtin_cd(args);
        }

        else if (strcmp(args[0], "exit") == 0) {
            builtin_exit();
        }

        else if (strcmp(args[0], "kill") == 0) {
            builtin_kill(args);
        }

        else {
            execute_external_command(args);
        }
    }


    exit_shell();

    return 0;
}
