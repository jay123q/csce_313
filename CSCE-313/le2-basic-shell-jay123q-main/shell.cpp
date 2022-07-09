/****************
LE2: Basic Shell
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
#include <sys/wait.h> // wait
#include "Tokenizer.h"
#include <iostream>
using namespace std;

int main () {
    // TODO: insert le1 main here

    // lists all the files in the root directory in the long format
    // translates all input from lowercase to uppercase
    int dup_output = dup( 1 ); // save this and use later
    int dup_input =  dup( 0 ); // save this and use later
    string input;

    while(true)
    {
        cout << " Provide Commands: \n";
        getline(cin, input);
        if(input == "exit" || input == "Exit" || input == "exit " || input.size() == 0)
        {
            break;
        }


        Tokenizer token(input);
        //split does a loop for each command in token.commands


        for( size_t i = 0 ; i< token.commands.size() ; i++)
        {


            char** cmd1 = new char* [token.commands[i]->args.size()];
            
            // for ( const string* var = token.commands[i]->args.data() ; var != token.commands[i]->args.end() , var++ )
            // {
            //     cmd1[i] = (char*) var;  
            // }

            for( size_t j = 0 ;  j < token.commands[i]->args.size() ; j++ )
            {
                cmd1[j] = (char*) ( token.commands[i]->args.at(j).c_str() ) ;
            }
            // cout << "  comand line " << *cmd1[i] << "\n";
            //get all the info and at the last iteration add one extra data point of null ptr

            

            // TODO: add functionality
            // Create pipe
            int fd[ 2 ]; //declare a pipe to output
            pipe( fd );



            // Create child to run first command
            auto fork_1 = fork();

 
               // child process
            if ( fork_1 == 0 )
            {
                // not the last command allow for a graceful exit
                if( i < token.commands.size() -1 )
                {
                    // In child, redirect input to write end of pipe
                    dup2( fd[1] , 1 );
                    // Close the read end of the pipe on the child side.
                    close( fd[0] );

                }
                // In child, execute the command
                execvp( cmd1[0] , cmd1 );
            }
            else
            {

                // Redirect shell(Parent)'s input to the read end of the pipe
                dup2( fd[0] , 0 );
                // Close the write end of the pipe on the parent side.
                close( fd[1] );        

                // wait until the last command finishes.
                if (i == token.commands.size( ) -1 )
                {
                    wait(0);
                }
                
            }

        input = "exit";
        //  delete [] cmd1;
        }
    }
        dup2( 1, dup_output ); // save back
        dup2( 0 , dup_input ); // save back

}