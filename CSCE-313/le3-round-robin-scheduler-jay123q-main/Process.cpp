#include "Process.h"
#include <iostream>
using namespace std;
// Constructor
Process::Process() :pid(0), arrival_time(0), cpu_burst_time(0), remaining_time(0), isCompleted(false) {
}

// Constructor
Process::Process(int pid, int arrival_time, int cpu_burst_time) {
    this->pid = pid;
    this->arrival_time = arrival_time;
    this->cpu_burst_time = cpu_burst_time;
    this->remaining_time = cpu_burst_time; // timer hasn't started yet, so it's full time
    this->isCompleted = false;
}

// Copy Construcor
Process::Process(const Process &p){
    this->pid = p.pid;
    this->arrival_time = p.arrival_time;
    this->cpu_burst_time = p.cpu_burst_time;
    this->remaining_time = p.remaining_time;
    this->isCompleted = p.isCompleted;
}

// Every time, When your process is running, use this function to update 
// the remaining time and monitor if the process is done or not
void Process::Run(int run_time) {
    // update remaining_time
    this->remaining_time -= run_time;
    // std:: cout << remaining_time << "time left \n";
    // isCompleted updated based on remianing time or not
    if(remaining_time == 0)
    {
        this->isCompleted = true;
    }
    else{ this->isCompleted = false; }
}

// void Process::check_pid_exists(int a_new_pid)
// {
//     if (a_new_pid==-1) ? 
//     this->pid = a_new_pid;
// }

// getter functions
int Process::getPid() const {
    return this->pid;
}
int Process::get_arrival_time() const {
	return this->arrival_time;
}
int Process::get_cpu_burst_time() const {
    return this->cpu_burst_time;
}
int Process::get_remaining_time() const {
	return this->remaining_time;
}
bool Process::is_Completed() const {
    return this->isCompleted;
}

// Assignment Operator Overloading
Process & Process::operator=(const Process & p)
{
    this->pid = p.pid;
    this->arrival_time = p.arrival_time;
    this->cpu_burst_time = p.cpu_burst_time;
    this->remaining_time = p.remaining_time;
    this->isCompleted = p.isCompleted;

    return *this;
}

void Process::change_arrival_time(int new_time )
{
    this->arrival_time = new_time;
}

