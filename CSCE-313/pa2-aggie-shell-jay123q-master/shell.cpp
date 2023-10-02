#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>  /* For O_RDWR */
#include <unistd.h> /* For open(), creat() */
#include <pwd.h>

#include <vector>
#include <string>
#include <string.h>

#include "Tokenizer.h"

// all the basic colours for a shell prompt
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define WHITE "\033[1;37m"
#define NC "\033[0m"

using namespace std;

string old_direct_working;
char new_direct_working[512];

void handle_directory_change(char** cmd1)
{
    bool empty = old_direct_working.empty();
    if (!strcmp(cmd1[1], "-"))
    {
        // cout << " ping33 \n ";
        if (empty)
        { // empty
         //   cout << " ping37 \n ";
            old_direct_working = getcwd(new_direct_working, 512);
        }
        // if it isnt empty change to that directory, then get the bufefer size
     //   cout << " the old directory is " << old_direct_working << std::endl;
        chdir(old_direct_working.c_str());
        old_direct_working = getcwd(new_direct_working, 512);
    }
    else
    {
        old_direct_working = new_direct_working;
        chdir(cmd1[1]); // maybe i+1 here
        // cout << " checking old working line 52 " << old_direct_working << std::endl;
    }
}

void combineProcess(int& in, int& out, Tokenizer& tknr)
{
    int fd[2]; // declare a pipe to output
    for (size_t i = 0; i < tknr.commands.size(); i++)
    {
        pipe(fd);

        // access command at i
        bool hasInput = tknr.commands[i]->hasInput();
        bool hasOutput = tknr.commands[i]->hasOutput();

        if (tknr.commands.size() - 1 == i && !hasOutput)
        {
            out = 1;
        }
        else
        {
            // last command
            out = fd[1];
        }

        char** cmd1 = new char* [tknr.commands[i]->args.size() + 1];
        for (size_t j = 0; j < tknr.commands[i]->args.size(); j++)
        {
            cmd1[j] = (char*)(tknr.commands[i]->args.at(j).c_str());
        }
        cmd1[tknr.commands[i]->args.size()] = nullptr;
        // close the write end as we are going to be re writing
        // close(in);
        // fork to create child
        // string file_name(one_command);
        if (!strcmp(cmd1[0], "cd"))
        {
            // do the work changing directories
            handle_directory_change(cmd1);
            continue;
        }

        pid_t forky = fork();

        if (forky == 0)
        {
            if (hasOutput)
            {
                // for ps aux 
                out = open(tknr.commands[i]->out_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                if (out == -1)
                { // no redirect
                    perror("cum_chalice 1");
                    exit(1);
                }
                int out_dup = dup2(out, STDOUT_FILENO);
                if (out_dup == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }
            else if (out != 1)
            {
                dup2(out, 1);
            }
            if (hasInput)
            {
                // for ps aux 
                in = open(tknr.commands[i]->in_file.c_str(), O_RDONLY);
                if (in == -1)
                { // no redirect
                    perror("congitive behavioral therapy 1");
                    exit(1);
                }
                int in_dup = dup2(in, STDIN_FILENO);
                if (in_dup == -1)
                {
                    perror("dup2");
                    exit(1);
                }
            }
            else if (in != 0)
            {
                dup2(in, 0);
            }
            execvp(cmd1[0], cmd1);
        }

        // if (out != 1)
        // { // prevent a stall after writing
        //     std::cout << " the process here at 247 " << counter << " is still running you incompetent coder" << std::endl;
        //     counter++;
        //     close(out);
        // }
        waitpid(forky, nullptr, 0);
        if (out != 1)
        {
            close(out);
        }
        delete[] cmd1;
        in = fd[0];
    }
}

// waits for the background processes to bie
void check_background_processes(vector<pid_t>& PID)
{
    int mom_are_you_back;
    for (size_t i = 0; i < PID.size(); i++)
    {
        mom_are_you_back = waitpid(PID[i], 0, WNOHANG);
        if (mom_are_you_back > 0)
        {
            std::cout << " younglings slain " << *(PID.begin() + i) << std::endl;
            auto index = PID.begin() + i;
            PID.erase(index);
            i--;
        }
    }
}


int main()
{
    int out = dup(1);
    int in = dup(0); // these will be place holder variables
    // vector of pids here follow minhs video
    vector<pid_t> PID;
    for (;;)
    {
        // need date/time, username, and absolute path to current dir
        cout << YELLOW << "Shell$" << NC << " ";
        char my_host[5012];
        my_host[5011] = '\0';
        gethostname(my_host, 5011);
        cout << YELLOW << getpwuid(geteuid())->pw_name << '@' << my_host << ":" << getcwd(new_direct_working, 512) << NC << "$";

        // get user inputted command
        string input;
        getline(cin, input);

        if (input == "exit")
        { // print exit message and break out of infinite loop
            cout << RED << "Now exiting shell..." << endl
                << "Goodbye" << NC << endl;
            break;
        }

        // wait for things to return
        check_background_processes(PID);

        // get tokenized commands from user input
        Tokenizer tknr(input);
        if (tknr.hasError())
        { // continue to next prompt if input had an error
            continue;
        }

        combineProcess(in, out, tknr);
    }

    dup2(1, out);
    dup2(0, in);
}
