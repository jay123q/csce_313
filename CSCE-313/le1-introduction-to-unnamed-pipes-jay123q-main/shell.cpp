/****************
LE1: Introduction to Unnamed Pipes
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
using namespace std;

int main () {
    // lists all the files in the root directory in the long format
    char* cmd1[] = {(char*) "ls", (char*) "-al", (char*) "/", nullptr};
    // translates all input from lowercase to uppercase
    char* cmd2[] = {(char*) "tr", (char*) "a-z", (char*) "A-Z", nullptr};

    auto dup_output = dup(1); // save this and use later
    auto dup_input =  dup(0); // save this and use later

    // TODO: add functionality
    // Create pipe
    int fd[ 2 ]; //declare a pipe to output
    pipe( fd );


    // Create child to run first command
    auto fork_1 = fork();
    
    // child process
    if ( fork_1 == 0 )
    {
        // In child, redirect input to write end of pipe
        dup2( fd[1] , 1 );
        // Close the read end of the pipe on the child side.
        close( fd[0] );
        // In child, execute the command
        execvp( cmd1[0] , cmd1 );
    }

    // Create another child to run second command+
    auto fork_2 = fork();
     // child process
    // Execute the second command.
    if ( fork_2 == 0 )
    { //ASKLD
        // In child, redirect input to the read end of the pipe
        dup2( fd[0] , 0 );
        // Close the write end of the pipe on the child side.
        close( fd[1] );
        //execute the second command
        execvp( cmd2[0] , cmd2 );
    }

    // Reset the input and output file descriptors of the parent.
    // save standard input and output and reset
    dup2(1,dup_output); // save back
    dup2(0,dup_input); // save back
}