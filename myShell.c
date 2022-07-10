#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


// This function reads the command user gives to the shell
char *readLine(){
    int bufferSize = 1024;
    int position = 0;
    char *inputCommand = malloc(sizeof(char) * bufferSize);
     int c;

     // Check if the memory is allotted to the string
    if (!inputCommand) {
        fprintf(stderr, "Memory Allocation error\n");
        exit(EXIT_FAILURE);
    }
     // Get the command char by char and store it in the above created string
    while (1) {
        // Read a character
        c = getchar();

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF || c == '\n') {
            inputCommand[position] = '\0';
            return inputCommand;
        } else {
            inputCommand[position] = c;
        }
        position++;

        // If we have exceeded the inputCommand, reallocate.
        if (position >= bufferSize) {
            bufferSize += 1024;
            inputCommand = realloc(inputCommand, bufferSize);
            if (!inputCommand) {
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

// This function splits the command into arguments
char** getArgs(char* line){
    int bufferSize = 64, position = 0;
    // This stores all the arguments in the command
    char **tokens = malloc(bufferSize * sizeof(char *));
    // This stores the current argument we are parsing
    char *token;
    // Check if the memory is alloted
    if(!tokens){
        printf("Memory Allocation Error");
        exit(EXIT_FAILURE);
    }

    // This gets the argument upto the next space or the other given delimiters
    token = strtok(line, " \t\r\n\a");
    while (token!=NULL){
        tokens[position++] = token;
        // If the number of tokens exceed reallocate memory
        if(position>=bufferSize){
            bufferSize+=64;
            tokens = realloc(tokens,bufferSize* sizeof(char *));
            if(!tokens){
                printf("Memory Allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}

// This takes args as input and launches the program
int launchProgram(char **args){
    pid_t pid, wpid;
    // This stores the status of the child process created
    int status;
    pid = fork();
    if(pid==0){
        // This executes the child process and if it returns a -1 that means there was an error in the process
        if(execvp(args[0],args)==-1){
            perror("Error");
        }
        exit(EXIT_FAILURE);
        // If pid<0 that means we forked the child process improperly
    }else if(pid<0){
        perror("Forked Improperly");
    }
    // Here the parent process waits for the child process to finish execution
    //
    else{
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}


// In shell most of the commands are programs
// But some are built into shell 1. Exit, 2. cd, 3. help
// so, we need to handle them differently
int execute(char** args){
    if(args[0]==NULL) return 1;

    printf("Command Entered:%s\n",args[0]);

    // See if the command entered is cd
    if(strcmp(args[0], "cd")==0){
        // If the directory to which we should change is not given
        if(args[1]==NULL){
            fprintf(stderr, "Expected argument to \"cd\"\n");
        }else{
            // Change to that directory
            if(chdir(args[1])!=0){
                perror("Error");
            }
        }
        return 1;
        // Handle the help command
    }else if(strcmp(args[0], "help")==0){
        printf("cd\n");
        printf("help\n");
        printf("exit\n");
        return 1;
        // Handle the exit command and return 0 to stop the program
    }else if(strcmp(args[0],"exit")==0){
        return 0;
    }
    // IF the commands are not built in commands then call the launchProgram function.
    else return launchProgram(args);

}

// This function calls the above defined functions appropriately
void loop(){
    // Initialize the required arguments
    char *line;
    char **args;
    int status;
    do{
        // Read the command entered
        printf("+ ");
        line = readLine();
        // Process the command
        args = getArgs(line);
        // Execute the command
        status = execute(args);

        // Free the space
        free(line);
        free(args);
        // Run the program until exit command is entered
    } while (status);
}

int main(){
    loop();
    return 0;
}
