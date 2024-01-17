#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glob.h>
#include <libgen.h>
#include <fcntl.h>

#define MAX_LINE 80
#define MAX_TOKENS 20

int last_command_status = 0;
char **expanded_tokens;
char **tokens;
char initial[124] = "/bin/";
char initialcpy[124] = "/bin/";
int max;
int malc1 = 0;
int free1 = 0;

void free_tokens(char **tokens) {
    //printf("free is executed\n");
    for (int i = 0; i < max; i++) 
    {
        free(tokens[i]);
    }
    max = 0;
    free(tokens);
}

char **tokenize(char *command) {
    tokens = malloc(MAX_TOKENS * sizeof(char*));
    char *token = strtok(command, " \t\r\n");
    int position = 0;

    while (token != NULL) {
        tokens[position] = token;  // Copy each token
        position++;
        token = strtok(NULL, " \t\r\n");
    }
    tokens[position] = NULL;

    return tokens;
}

char **expand_tokens(char **tokens) {
    expanded_tokens = malloc(MAX_TOKENS * sizeof(char*));
    int position = 0;

    for (int i = 0; tokens[i] != NULL; i++) {
        if (strchr(tokens[i], '*') != NULL) {  // Check for wildcard
            glob_t glob_result;
            memset(&glob_result, 0, sizeof(glob_result));
            glob(tokens[i], GLOB_TILDE | GLOB_NOCHECK, NULL, &glob_result);

            if(glob_result.gl_pathc == 0) 
            {
                // No matches found, retain the original token
                expanded_tokens[position] = malloc(sizeof(char*)*sizeof(tokens[i]));
                strcpy(expanded_tokens[position], tokens[i]);
                position++;
            }
            else{
                for (unsigned j = 0; j < glob_result.gl_pathc; j++) {
                    expanded_tokens[position] = malloc(sizeof(char*)*sizeof(glob_result.gl_pathv[j]));
                    strcpy(expanded_tokens[position], glob_result.gl_pathv[j]);
                    position++;
                    if (position >= MAX_TOKENS - 1) break;  // Prevent overflow
                }
            }
            globfree(&glob_result);
        } else {
            expanded_tokens[position] = malloc(sizeof(char*)*sizeof(tokens[i]));
            strcpy(expanded_tokens[position], tokens[i]);
            position++;
        }
        max = position;
        if (position >= MAX_TOKENS - 1) break;  // Prevent overflow
    }
    expanded_tokens[position] = NULL;

    //free_tokens(tokens);  // Free the original tokens
    return expanded_tokens;
}

void execute_command(char **args) {
    pid_t pid;
    int status;
    int in_redirect = -1, out_redirect = -1, pipe_pos = -1;
    int pipefd[2];
    if (args[0] == NULL)
    {
        return;
    }
    if (strcmp(args[0], "cd") == 0) 
    {
        if (args[1] == NULL) {
            fprintf(stderr, "mysh: cd: missing argument\n");
        } else {
            if (chdir(args[1]) != 0) 
            {
                perror("mysh");
            }
        }
        return;
    }
    // Check for redirection and pipes in args
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            free(args[i]);
            args[i] = NULL;
            in_redirect = i + 1;
        } else if (strcmp(args[i], ">") == 0) {
            free(args[i]);
            args[i] = NULL;
            out_redirect = i + 1;
        } else if (strcmp(args[i], "|") == 0) {
            free(args[i]);
            args[i] = NULL;
            pipe_pos = i + 1;
            break;
        }
    }

    if (pipe_pos != -1) {
        pipe(pipefd);
    }

    pid = fork();
    if (pid == 0) {
        // Child process
        if (in_redirect != -1) {
            int fd0 = open(args[in_redirect], O_RDONLY);
            dup2(fd0, STDIN_FILENO);
            close(fd0);
        }
        if (out_redirect != -1) {
            int fd1 = open(args[out_redirect], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd1, STDOUT_FILENO);
            close(fd1);
        }

        if (pipe_pos != -1) {
            // First part of the pipe
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            strcat(initial, args[0]);
            execv(initial, args);
            strcpy(initial, initialcpy);
            exit(EXIT_FAILURE);
        } else {
            strcat(initial, args[0]);
            execv(initial, args);
            strcpy(initial, initialcpy);
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Error forking
        perror("mysh");
    } else {
        if (pipe_pos != -1) {
            // Second part of the pipe
            if (fork() == 0) {
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[1]);
                strcat(initial, args[pipe_pos]);
                execv(initial, &args[pipe_pos]);
                strcpy(initial, initialcpy);
                exit(0);
            }
            close(pipefd[0]);
            close(pipefd[1]);
        }
        // Wait for the child process or both parts of the pipe
        waitpid(pid, &status, WUNTRACED);
        last_command_status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;

        if (pipe_pos != -1) {
            wait(NULL); // Wait for the second part of the pipe
        }
    }
}

void parse_command(char *command) {

    char **tokens = tokenize(command);
    
    // Expand the tokens
    tokens = expand_tokens(tokens);
    //Handling 'then' and 'else' conditionals

    while (tokens[0] != NULL && (strcmp(tokens[0], "then") == 0 || strcmp(tokens[0], "else") == 0)) {
        int should_execute = 0;
        if (strcmp(tokens[0], "then") == 0 && last_command_status == 0) {
            should_execute = 1; // Execute if the last command succeeded
        } else if (strcmp(tokens[0], "else") == 0 && last_command_status != 0) {
            should_execute = 1; // Execute if the last command failed
        }
        if (!should_execute) {
            free_tokens(tokens); // Skip execution if condition not met
            return;
        }

        // Remove 'then' or 'else' from the tokens
        int i = 0;
        free(tokens[0]);
        for (; tokens[i + 1] != NULL; i++) {
            tokens[i] = tokens[i + 1];
        }
        tokens[i] = NULL;
    }

    // Execute the command
    execute_command(tokens);

    // Free the allocated memory
    free_tokens(tokens);
}
void passTheCommand(char *command)
{
    // printf("command: %s\n", command);
    if (strncmp(command, "exit", 4) == 0) 
    {
        printf("mysh: exiting\n");
        // free(expanded_tokens);
        exit(0);
    }
    parse_command(command);
    free(tokens);
}
int main(int argc, char **argv) 
{
    char command[MAX_LINE];
    if (argc > 1)
    {
        const char *filename = argv[1];

        // Open the file for reading
        FILE *file = fopen(filename, "r");

        // Check if the file was successfully opened
        if (file == NULL) 
        {
            fprintf(stderr, "Error opening file: %s\n", filename);
            return 1; // Return an error code
        }

        // Read and print each line of the file
        char line[256]; // Adjust the buffer size as needed
        while (fgets(line, sizeof(line), file) != NULL) 
        {
            // You can process the content of each line as needed
            if (strncmp(line, "exit", 4) == 0) {
            printf("mysh: exiting\n");
            break;
        }
        passTheCommand(line);
        }

        // Close the file
        fclose(file);
        return(0);
    }
    printf("Welcome to my shell!\n");
    while (1) 
    {
        printf("mysh> ");
        if (!fgets(command, MAX_LINE, stdin)) {
            // Handle error or EOF
            if (feof(stdin)) break;  // Exit on EOF
            perror("fgets error");
            continue;
        }
        passTheCommand(command);
    }
    return 0;
}