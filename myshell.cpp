//Anwi Gundavarapu
//Dr. Zhu
//CPSC 3500: myshell.cpp
//4/16/25



#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
using namespace std;


//global constants
//limited line, command and token length based on the assignment descr
const int MAX_LINE_LENGTH = 4026;
const int MAX_COMMANDS = 10;
const int MAX_TOKENS = 20;
const int MAX_TOKEN_LENGTH = 20;

int main()
{
    //declare variables to parse through input

    //commandLine holds input as a c string
    //commands tokenizes commands by |
    char commandLine[MAX_LINE_LENGTH];
    char* commandsArray[MAX_COMMANDS];

    int totalCommands = 0;

    cout << "myshell$";
    cin.getline(commandLine, MAX_LINE_LENGTH);

    //tokenizing by pipes using strtok()

    //using strtok to add first command before |
    char* command = strtok(commandLine, "|");
    while ( command != nullptr && totalCommands < MAX_COMMANDS)
    {
        //ignore leading whitespace
        while (*command == ' ')
        {
            command++;
        }


        //ignore trailing whitespaces
        int len = strlen(command);
        while (len > 0 && (command[len-1] == ' '
            || command[len-1] == '\t'
            || command[len-1] == '\n')) {
            command[len-1] = '\0';
            len--;
        }

        //add previously extracted command to commands array
        commandsArray[totalCommands++] = command;

        //tokenize next command
        command = strtok(nullptr, "|");
    }



    //create n-1 pipes for n commands dynamically
    int **pipes = new int*[totalCommands - 1];
    for (int i = 0; i < totalCommands - 1; i++) {
        pipes[i] = new int[2];
    }


    //create all pipes
    for (int i = 0; i < totalCommands - 1; i++)
    {
        //if pipe fails
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe failed!");
        }
    }

    //declare child process array of size max processes
    pid_t pids[MAX_COMMANDS];

    //loop to tokenize each command and create a child process for each command
    for (int i = 0; i < totalCommands; i++)
    {

        //tokenize into token pointer array
        char* tokensArray[MAX_TOKENS];
        int totalToks = 0;

        //use strtok to add first token seperated by space
        char* token = strtok(commandsArray[i], " ");


        //loop to add all tokens to tokens array
        while (token && totalToks < MAX_TOKENS - 1)
        {

            //previously tokenized token added to tokens array
            //use strtok to continue tokenizing
            tokensArray[totalToks++] = token;
            token = strtok(nullptr, " ");

        }
        //nullify last token in tokens array to be passed into execvp
        tokensArray[totalToks] = nullptr;

        //fork the first child process
        pids[i] = fork();

        //failed fork
        if (pids[i] == -1)
        {
            perror("fork failed!");
            exit(1);
        }

        //child process forked succesfully
        if (pids[i] == 0)
        {
            //if not the first process, establish pipeline
            if (i > 0)
            {
                //use dup2 to connect the previous process's
                //output to current process's stdin
                dup2(pipes[i-1][0], STDIN_FILENO);
            }

            //if not last process, establish pipeline connection
            if (i < totalCommands - 1)
            {
                //use dup2 to redirect standard output to current process's stdout
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            //close all file discriminators
            for (int i = 0; i < totalCommands -1; i++)
            {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }

            //pass in token and tokens array to rewrite process code
            //error thrown and exited upon failure
            execvp(tokensArray[0], tokensArray);
            perror("execvp failed");
            exit(1);
        }
    }

    //parent process closes all file discriminators after forking
    for (int i = 0; i < totalCommands -1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    
    //deallocate dynamic pipe memory
    for (int i = 0; i < totalCommands - 1; i++) {
        delete[] pipes[i];
    }
    delete[] pipes;

    //parent process waits for all child processes to finish exiting
    for (int i = 0; i < totalCommands; i++){
        //calling waitpid to wait on each child processes exit
        int status;
        pid_t pidEnd = waitpid(pids[i], &status, 0);

        //error and exit upon failure
        if (pidEnd == -1) {
            perror("waitpid failed");
            exit(1);
        }

        //completion message!
        cout << "Process " << pidEnd << " exits with " << status << endl;
    }



    return 0;
}
