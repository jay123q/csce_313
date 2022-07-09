#include "RoundRobin.h"
#include <iostream>
using namespace std;
/*
This is a constructor for RoundRobin Scheduler, you should use the extractProcessInfo function first
to load process information to process_info and then sort process by arrival time;

Also initialize time_quantum
*/
RoundRobin::RoundRobin(string file, int the_time_quantum) {
	// generate file process
	extractProcessInfo(file);
	set_time_quantum(the_time_quantum);
	
}


// Schedule tasks based on RoundRobin Rule
// the jobs are put in the order the arrived
// Make sure you print out the information like we put in the document
void RoundRobin::schedule_tasks() {
	int system_time = 0;
	// int debugger_counter=0;
	vector<shared_ptr<Process>> Ready_Process; 
	bool Gate = true;
	while( !processVec.empty() || !Ready_Process.empty() )
	{
		if(!processVec.empty())
		{ //  this is a protections statemnent that allows us to print all from ready_process
			auto v_front = processVec.front();

			// if arrival time is the same as system time then put into running processes
			// <= because we are incrementing system time in various spots
			while( v_front->get_arrival_time() <= system_time && Gate)
			{
				
				Ready_Process.push_back(v_front);
				processVec.pop();
				// check if vfront null
				if(processVec.empty())
				{
					//  this is a janky break since it broke outta all while loops
					Gate = false;
					continue;
				}
				v_front = processVec.front();

			}
		}
		// if(debugger_counter > 40)
		// {
		// 	cout << "DEBUGGER TRIGGERED \n";
		// 	break;
		// }debugger_counter++;

		if(Ready_Process.size() == 0)
		{
			// print no pids
			print(system_time,-1,0);
			system_time++;
			continue;

		}


		auto Process_at_running = Ready_Process.front();
		int run_time = time_quantum;

		if( Process_at_running->get_remaining_time() < time_quantum)
			{
				// cout << "asdunasdhjuiow \n";
			run_time = Process_at_running->get_remaining_time();
			}
			Process_at_running->Run(run_time);
		// this prints all the finish processes
		for (int i = 0; i < run_time ; i++)
		{
			print(system_time,Process_at_running->getPid(),0);
			system_time++; //  increment before the next step
			
		}
		if(Process_at_running->is_Completed())
	    // if last iteration and we are compelete
		{
		print(system_time,Process_at_running->getPid(),Process_at_running->is_Completed());
		}

		// throw to back
		if(Process_at_running->get_remaining_time() > 0)
		{
			// should be push to ready process to fix bug in tests
			processVec.push(Process_at_running);
			Gate = true; // reenable while
		}
		Ready_Process.erase(Ready_Process.begin());		

	}

}


/*************************** 
ALL FUNCTIONS UNDER THIS LINE ARE COMPLETED FOR YOU
You can modify them if you'd like, though :)
***************************/


// Default constructor
RoundRobin::RoundRobin() {
	time_quantum = 0;
}

// Time quantum setter
void RoundRobin::set_time_quantum(int quantum) {
	this->time_quantum = quantum;
}

// Time quantum getter
int RoundRobin::get_time_quantum() {
	return time_quantum;
}

// Print function for outputting system time as part of the schedule tasks function
void RoundRobin::print(int system_time, int pid, bool isComplete){
	string s_pid = pid == -1 ? "NOP" : to_string(pid);
	cout << "System Time [" << system_time << "].........Process[PID=" << s_pid << "] ";
	if (isComplete)
		cout << "finished its job!" << endl;
	else
		cout << "is Running" << endl;
}

// Read a process file to extract process information
// All content goes to proces_info vector
void RoundRobin::extractProcessInfo(string file){
	// open file
	ifstream processFile (file);
	if (!processFile.is_open()) {
		perror("could not open file");
		exit(1);
	}

	// read contents and populate process_info vector
	string curr_line, temp_num;
	int curr_pid, curr_arrival_time, curr_burst_time;
	while (getline(processFile, curr_line)) {
		// use string stream to seperate by comma
		stringstream ss(curr_line);
		getline(ss, temp_num, ',');
		curr_pid = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_arrival_time = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_burst_time = stoi(temp_num);
		shared_ptr<Process> p(new Process(curr_pid, curr_arrival_time, curr_burst_time));

		processVec.push(p);
	}

	// close file
	processFile.close();
}