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

void handle_directory_change(char **cmd1)
{
    bool empty = old_direct_working.empty();
    if (!strcmp(cmd1[1], "-"))
    {
        cout << " ping33 \n ";
        if (empty)
        { // empty
            cout << " ping37 \n ";
            old_direct_working = getcwd(new_direct_working, 512);
        }
        // if it isnt empty change to that directory, then get the bufefer size
        cout << " the old directory is " << old_direct_working << std::endl;
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
// void process_single_command(char *one_command)
// {
//     bool pwd_cd_check = !strcmp(one_command, "cd");
//     std::cout << "cd check " << pwd_cd_check << std::endl;
//     if (!pwd_cd_check)
//     {
//         char *args[] = {one_command, nullptr};
//         std::cout << " running a single command args" << *args << std::endl;

//         if (execvp(args[0], args) < 0)
//         { // error check
//             std::cout << " running a single command 81" << std::endl;

//             perror("execvp");
//             exit(2);
//         }
//     }
//     else
//     {
//         std::cout << " running a single command 86" << std::endl;

//         handle_directory_change(one_command);
//     }
// }
// void create_process_mult(int in, int out, char **one_command, bool hasInput, bool hasOutput, vector<pid_t> &PID)
// {

//     // now close the write end
//     close(in);
//     // fork to create child
//     // string file_name(one_command);

//     pid_t pid = fork();
//     PID.push_back(pid);
//     if (pid < 0)
//     { // error check
//         perror("fork");
//         exit(2);
//     }
//     if (pid == 0)
//     { // if child, exec to run command
//         // is there a input, if so redirect
//         if (hasInput)
//         {
//             int check_open = open(*one_command, ios::in);
//             if (check_open == -1)
//             { // no redirect
//                 perror("open");
//                 exit(1);
//             }
//             if (dup2(in, STDIN_FILENO) == -1)
//             {
//                 perror("dup2");
//                 exit(1);
//             }
//             // close( out );
//         }
//         // is there a input, no, now check if we are chaining
//         else if (in != 0)
//         {
//             dup2(in, 0);
//         }
//         // is there a output, if so redirect
//         if (hasOutput)
//         {
//             int check_open = open(*one_command, ios::out);
//             if (check_open == -1)
//             { // no redirect
//                 perror("open");
//                 exit(1);
//             }
//             if (dup2(out, STDOUT_FILENO) == -1)
//             {
//                 perror("dup2");
//                 exit(1);
//             }
//         }
//         // is there a output, no, now check if we are chaining
//         else if (out != 1)
//         {
//             dup2(out, 1);
//         }
//         // run single commands with no arguments
//         char *args[] = {*one_command, nullptr};
//         if (execvp(args[0], args) < 0)
//         { // error check
//             perror("execvp");
//             exit(2);
//         }
//     }
//     else
//     {
//         // parent
//         int status = 0;
//         waitpid(pid, &status, 0);
//         if (status > 1)
//         { // exit if child didn't exec properly
//             exit(status);
//         }
//     }
// }

// waits for the background processes to bie
void check_background_processes(vector<pid_t> &PID)
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
void create_process(int &in, int &out, Command *current_command, bool hasInput, bool hasOutput)
{
    char **cmd1 = new char *[current_command->args.size() + 1];
    for (size_t j = 0; j < current_command->args.size(); j++)
    {
        cmd1[j] = (char *)(current_command->args.at(j).c_str());
    }
    cmd1[current_command->args.size()] = nullptr;
    // close the write end as we are going to be re writing
    // close(in);
    // fork to create child
    // string file_name(one_command);

    pid_t forky = fork();

    if (forky == 0)
    {
        if (hasOutput)
        {
            out = open(current_command->out_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
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
            in = open(current_command->in_file.c_str(), O_RDONLY);
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
    delete [] cmd1;

}

void process_command(int &in, int &out, Command *current_command, char **cmd1, vector<pid_t> &PID)
{
    if (!strcmp(cmd1[0], "cd"))
    {
        // do the work changing directories
        handle_directory_change(cmd1);
        return;
    }
    pid_t forky = fork();
    // access command at i
    bool hasInput = current_command->hasInput();
    bool hasOutput = current_command->hasOutput();

    if (current_command->isBackground())
    {
        PID.push_back(forky);
    }
    if (forky == 0)
    {
        if (hasOutput)
        {
            out = open(current_command->out_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (out == -1)
            { // no redirect
                perror("cbt2");
                exit(1);
            }
            int check_dup = dup2(out, STDOUT_FILENO);
            if (check_dup == -1)
            {
                perror("dup2");
                exit(1);
            }
            // else works
        }
        if (hasInput)
        {
            // I used to use IO::IN and this failed
            in = open(current_command->in_file.c_str(), O_RDONLY);
            if (in == -1)
            { // no redirect
                perror("cbt3");
                exit(1);
            }
            int check_dup = dup2(in, STDIN_FILENO);

            if (check_dup == -1)
            {
                perror("dup2");
                exit(1);
            }
        }
        execvp(cmd1[0], cmd1);

        // if (out != 1)
        // { // prevent a stall after writing
        //     cout << " closing 290 \n";
        //     close(out);
        // }
    }
    else
    {

        int status = 0;
        waitpid(forky, &status, 0);
        if (status > 1)
        { // exit if child didn't exec properly
            exit(status);
        }
    }
}
void process_chunk(int &in, int &out, Tokenizer &token, vector<pid_t> &PID)
{
    Command *current_command = token.commands.back();
    char **cmd1 = new char *[current_command->args.size() + 1];
    for (size_t j = 0; j < current_command->args.size(); j++)
    {
        cmd1[j] = (char *)(current_command->args.at(j).c_str());
    }
    cmd1[current_command->args.size()] = nullptr;


    process_command(in, out, current_command, cmd1, PID);

    
    delete[] cmd1;
}
void process_single(int &in, int &out, Tokenizer &tknr)
{
    int fd[2]; // declare a pipe to output
    for (size_t j = 0; j < tknr.commands.size(); j++)
    {
        pipe(fd);

        // access command at i
        bool hasInput = tknr.commands[j]->hasInput();
        bool hasOutput = tknr.commands[j]->hasOutput();

        if (tknr.commands.size() - 1 == j && !hasOutput)
        {
            out = 1;
        }
        else
        {
            // last command
            out = fd[1];
        }

        create_process(in, out, tknr.commands[j], hasInput, hasOutput);
        in = fd[0];
     
        // read is now write
        // chdir()
        // if dir (cd  <dir> ) is "-", then go to previous working directory

        // // print out every command token-by-token on individual lines
        // // prints to cerr to avoid influencing autograder
        // for (auto cmd : tknr.commands) {
        //     for (auto str : cmd->args) {
        //         cerr << "|" << str << "| ";
        //     }
        //     if (cmd->hasInput()) {
        //         cerr << "in< " << cmd->in_file << " ";
        //     }
        //     if (cmd->hasOutput()) {
        //         cerr << "out> " << cmd->out_file << " ";
        //     }
        //     cerr << endl;
        // }
        // cmd1[i] = nullptr;
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
        // // see if the out isnot 1. if it is end
        // if (tknr.commands.size() > 1)
        // {
        //     process_single(in, out, tknr);
        // }
        // else
        // {
        //     // cout << " is the correc things running \n";
        //     process_chunk(in, out, tknr, PID);
        // }        


            // cout << " is the correc things running \n";
            process_chunk(in, out, tknr, PID);
    
    }

    dup2(1, out);
    dup2(0, in);
}
