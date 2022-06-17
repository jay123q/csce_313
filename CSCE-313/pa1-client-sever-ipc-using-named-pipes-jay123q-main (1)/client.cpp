/*
	Original author of the starter code
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 2/8/20
	
	Please include your Name, UIN, and the date below
	Name: Joshua Clapp
	UIN: 930004089
	Date: 6/6/2022
*/
#include "common.h"
#include "FIFORequestChannel.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
double request_one_data_point(FIFORequestChannel *c, int p, double t, int e , char * buff)
{
// example data point request

    datamsg x(p, t, e);
	memcpy(buff, &x, sizeof(datamsg));
	c->cwrite(buff, sizeof(datamsg)); // question
	double reply;
	c->cread(&reply, sizeof(double)); //answer
	return reply;
}

void request_multiple(FIFORequestChannel *c, int person, char * buff)
{
	 int t = 0 ;
	auto kk = request_one_data_point( c,  person , t, 1 ,  buff);
	kk +=1;
	// double text_excell_double;
	int counter = 0;
	int name_counter = 1;

	string text_read,p_string,text_excell = "";
	p_string = to_string(person);
	cout <<" p string is " << p_string << endl;
	ifstream read_file("BIMDC/"+ p_string+".csv");
	ofstream create_excell("received/x"+ to_string(name_counter) +".csv");
	name_counter++;
	if(!read_file)
	{
		cout << " cant open " << endl;
		return;
	}
	//cout << text_read;
	while(counter < 1000)
	{
		getline(read_file,text_read);
		create_excell << text_read << '\n';
		// stringstream ss(text_read);

		// ss >> t;
		// // hard coding 1 as a pick the first ekg, then a 2 for the second
		// text_excell_double = request_one_data_point( c,  person , t, 1 ,  buff);
		// text_excell += to_string(text_excell_double);
		// text_excell_double = request_one_data_point( c,  person , t, 2 ,  buff);
		// text_excell += to_string(text_excell_double);
		// create_excell << text_excell + '\n';
		// text_excell = "";
		counter++;
	}
	create_excell.close();
	read_file.close();
}

__int64_t request_flen( FIFORequestChannel *c, const char * filename)
{
	filemsg fm(0, 0); // a challenge will be sending a file with over 256 characters, how do you control what happens?  How you send the rest?
	std::string file_string(filename);
	string fname = file_string;
	
	//contains entireity of file message and file name
	int len = sizeof(filemsg) + (fname.size() + 1);
	char* buf2 = new char[len];
	memcpy(buf2, &fm, sizeof(filemsg));
	strcpy(buf2 + sizeof(filemsg), fname.c_str());
	c->cwrite(buf2, len);  // I want the file length;
	__int64_t filesize = 0;
	c->cread(&filesize , sizeof(filesize));
	delete [] buf2;
	return filesize;
}

void file_transfer(FIFORequestChannel *c, __int64_t buffcap, char * buff, const char *  filename)
{
	std::string file_string(filename);
	string fname = file_string;
	// filemsg * file_msg; couldnt get it to allocate on the heap
	// char * buffer_request;
	// char * str_copy;
	__int64_t offset = 0;

	std::string str_buffer ="";
	auto total_file_length = request_flen(c, filename);

	ofstream create_excell("received/"+fname,ios::binary);
	//loop over segments
	while(total_file_length > offset)
	{
		if(total_file_length - offset < buffcap)
		{
			// stops 5 trailing bytes
			buffcap = total_file_length - offset;
		}
		// gimmi the size
		int len = sizeof( filemsg ) + ( fname.size() + 1);
		char * buffer_request = new char[ len ];
		// char * str_copy = new char[ len ];

		filemsg file_msg( offset , buffcap );
		
		// commecnt
		
		memcpy( buffer_request + sizeof(filemsg) , fname.c_str() + '\0', fname.size() + 1 );
		memcpy( buffer_request, &file_msg , sizeof( filemsg ));

		c->cwrite( buffer_request , len );

		c->cread( buff , buffcap );

		create_excell.write(buff, buffcap);
		
		offset += buffcap;

		delete [] buffer_request;
		// delete [] str_copy;
	}
	//offset, length


	create_excell.close();

}


FIFORequestChannel * new_chan(FIFORequestChannel *c)
{
	MESSAGE_TYPE temp = NEWCHANNEL_MSG;
	c->cwrite(&temp, sizeof(MESSAGE_TYPE));
	char name[100];
	// create a variable to hold the name
	// cread response from server
	c->cread(name,sizeof(name));
	// call the FIFORequestChannel constructor with the name from the server
	FIFORequestChannel * new_channel = new FIFORequestChannel(name,FIFORequestChannel::CLIENT_SIDE );
	// call the functions in here
	return new_channel;
}

void delete_chan(FIFORequestChannel *c)
{

	MESSAGE_TYPE temp = QUIT_MSG;
	c->cwrite(&temp, sizeof(MESSAGE_TYPE));
	delete c;
	// delete [] *c;

}
// int execvp( const char* filename , char const * argv[] );



int main (int argc, char *argv[]) {
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	int m = MAX_MESSAGE;
	char * buffer = new char[m];
	bool new_channel = false;
	bool file_transfer_gate = false;
	FIFORequestChannel * main_channel;
	//vector<FIFORequestChannel*> channels;
	
	string filename = "";

	// FIFORequestChannel * chan =  new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE); 
	// this is how you join /  create a new channel
		// the idea was to declare the channel here but in reality it stopped the code form running further

	while ((opt = getopt(argc, argv, "p:t:e:f:m:c")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				// req_one_data_pnt[0] = 1;
				break;
			case 't':
				t = atof (optarg);
				// req_one_data_pnt[1] = 1;
				break;
			case 'e':
				e = atoi (optarg);
					// auto double_holder = request_one_data_point(chan,p,t,e,buffer);
				break;
			case 'f':
				filename = optarg;
				file_transfer_gate = true;
				break;
			case 'm':
				m = atoi(optarg);
				delete [] buffer;
				buffer = new char [m];
				break;
			case 'c':
				 new_channel = true;
				 break;
		}
	}

	//give arguements for the server
	//server needs path . /server , '-m' , '<val for -m arg> <- -m is buffer capacity ', 'NULL' <- done giving arguements to server
	//m fork
	// in the child, run execvp using the server arguments.

	int doIFork = fork() ; 
	if( doIFork < 0)
	{ // client run
		cout << " no fork " << endl;
		cout << "  ending thread "; 
		return 69;
	}
	else if( doIFork == 0)
	{
	//  server
		char* args[] = {const_cast< char* >("./server"),const_cast< char* >("-m"),const_cast< char* >(to_string(m).c_str()),nullptr};
		auto fail = execvp(args[0],args);
		if (fail < 0)
		{
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	// FIFORequestChannel * main_channel = new FIFORequestChannel("main_channel",FIFORequestChannel::CLIENT_SIDE);

	FIFORequestChannel * chan =  new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE); 
	main_channel = chan;
	if (new_channel)
	{
		// allocating inside of newchan
		main_channel = new_chan(chan);
		// FIFORequestChannel * new_channel = new_chan(chan);
		// idea of making a vector with all things inside then commenting out main body function and then in here I can throw the function that parses the requests in like 258
	}

	auto this_is_stupid = request_one_data_point( chan ,  p , t , e , buffer );
	cout << "check" <<this_is_stupid << "\n"; 
	request_multiple(chan,p,buffer);
	
	const char * file_name_to_c_str = filename.c_str(); 
	if(file_transfer_gate)
	{
		file_transfer(chan,MAX_MESSAGE,buffer,file_name_to_c_str);
	}
	if(new_channel)
	{
		delete_chan(main_channel);
		delete_chan(chan);
	}
	else
	{
		delete_chan(chan);
	}
	waitpid(doIFork,0 ,0);
	delete [] buffer;
	// delete chan;
	// delete main_channel;
	// delete[] new_channel;
	
}
