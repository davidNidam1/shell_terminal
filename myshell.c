#include <stdio.h>      // For standard input/output functions
#include <stdlib.h>     // For general utility functions like malloc and free
#include <string.h>     // For string manipulation functions
#include <unistd.h>     // For POSIX operating system API functions like fork and exec
#include <sys/types.h>  // For data types used in system calls
#include <sys/wait.h>   // For waitpid function to wait for child process
#include <errno.h>      // For error number macros and perror function

// Constants to define maximum limits
#define MAX_PATHS 100
#define MAX_COMMAND_LENGTH 100
#define MAX_HISTORY 100

// Function Declarations
void store_paths(int argc, char *argv[]);
char* find_command(char *cmd);
void handle_cd(char *args[]);
void handle_pwd();
void show_history();
void add_to_history(const char *cmd);

// Arrays to store custom paths and command history
char *custom_paths[MAX_PATHS+1];
int path_count = 0;
char *history[MAX_HISTORY];
int history_count = 0;

int main(int argc, char *argv[]) {
    // Store paths provided as command-line arguments and from $PATH variable
    store_paths(argc - 1, &argv[1]);

    char command[MAX_COMMAND_LENGTH];

    while (1) {
        // Print the shell prompt
        printf("$ ");
        // Flush the output buffer to ensure the prompt is displayed immediately
        fflush(stdout);

        // Read the command input from the user
        if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        // Remove the newline character at the end
        command[strcspn(command, "\n")] = '\0';

        // Add the command to the history
        add_to_history(command);

        // Tokenize the user's command input into individual words (tokens)
        char *args[MAX_COMMAND_LENGTH / 2 + 1];
        int i = 0;
        args[i] = strtok(command, " ");
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " ");
        }

        // If no command is entered (atfer pressing Enter), continue to the next loop iteration
        if (args[0] == NULL) {
            continue;
        }

        // Handle built-in commands
        if (strcmp(args[0], "exit") == 0) {
            // Exit the shell
            break;
        } else if (strcmp(args[0], "cd") == 0) {
            // Change directory
            handle_cd(args);
        } else if (strcmp(args[0], "pwd") == 0) {
            // Print working directory
            handle_pwd();
        } else if (strcmp(args[0], "history") == 0) {
            // Show command history
            show_history();
        } else {
            // Find and execute the command
            char *cmd_path = find_command(args[0]);
            if (cmd_path != NULL) {
                pid_t pid = fork();
                if (pid == 0) {
                    execv(cmd_path, args);
                    perror("exec failed");
                    exit(EXIT_FAILURE);
                } else if (pid < 0) {
                    perror("fork failed");
                } else {
                    waitpid(pid, NULL, 0);
                }
            } else {
                printf("%s: command not found\n", args[0]);
            }
        }
    }
    for (int i = 0; i < history_count; i++) {
            free(history[i]);
        }
        for (int i = 0; i < path_count; i++) {
            free(custom_paths[i]);
        }
    return 0;
}

void store_paths(int argc, char *argv[]) {
    // Store custom paths from command-line arguments
    for (int i = 0; i < argc && path_count < MAX_PATHS; i++) {
        custom_paths[path_count++] = strdup(argv[i]);
    }

    // Store paths from the PATH environment variable
    char *path_env = getenv("PATH");
    if (path_env != NULL) {
        char *path_copy = strdup(path_env);  // Make a copy of PATH environment variable
        char *token = strtok(path_copy, ":");  // Tokenize the PATH variable by colon
        while (token != NULL && path_count < MAX_PATHS) {
            custom_paths[path_count++] = strdup(token);
            token = strtok(NULL, ":");
        }
        free(path_copy);
    }
    custom_paths[path_count] = NULL;

}

char* find_command(char *cmd) {
    static char path[1024];  // Buffer to store the full path of the command
    for (int i = 0; i < path_count; i++) {
        snprintf(path, sizeof(path), "%s/%s", custom_paths[i], cmd);  // Construct the full path
        if (access(path, X_OK) == 0) {  // Check if the command is executable
            return path;  // Return the full path if the command is found
        }
    }
    return NULL;  // Return NULL if the command is not found
}

void handle_cd(char *args[]) {
    if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");  // Print error if no directory is specified
    } else if (chdir(args[1]) != 0) {
        perror("cd failed");  // Print error if changing directory fails
    }
}

void handle_pwd() {
    char cwd[1024];  // Buffer to store the current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);  // Print the current working directory
    } else {
        perror("pwd failed");  // Print error if getting current working directory fails
    }
}

void add_to_history(const char *cmd) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(cmd);  // Add the command to history
    } else {
        // If history is full, remove the oldest command and add the new one
        free(history[0]);  // Free the memory of the oldest command
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];  // Shift commands to the left
        }
        history[MAX_HISTORY - 1] = strdup(cmd);  // Add the new command at the end
    }
}

void show_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%s\n", history[i]);  // Print each command in history
    }
}
