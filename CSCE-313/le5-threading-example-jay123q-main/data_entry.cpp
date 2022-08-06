#include <vector> // vector, push_back, at
#include <string> // string
#include <iostream> // cin, getline
#include <fstream> // ofstream
#include <unistd.h> // getopt, exit, EXIT_FAILURE
#include <assert.h> // assert
#include <thread> // thread, join
#include <sstream> // stringstream

#include "BoundedBuffer.h" // BoundedBuffer class

#define MAX_MSG_LEN 256

using namespace std;

/************** Helper Function Declarations **************/

void parse_column_names(vector<string>& _colnames, const string& _opt_input);
void write_to_file(const string& _filename, const string& _text, bool _first_input=false);

/************** Thread Function Definitions **************/

// "primary thread will be a UI data entry point"
void ui_thread_function(BoundedBuffer* bb) {
    string input;

    while(true)
    {
        std::cout << "enter input \n";

        cin >> input;
        if(input == "Exit")
        {
            // std::cout << "break1 input \n";
            // push a quit dip, esacpe run!
            bb->push( (char * ) input.c_str() , input.size() );
            
            break;
            // std::cout << "break2 input \n";
        }
        // the line below is cursed code
        // I can see how this could potenially need to be 256
        bb->push( (char * ) input.c_str() , input.size() );


        
    }
}

// "second thread will be the data processing thread"
// "will open, write to, and close a csv file"
void data_thread_function(BoundedBuffer* bb, string filename, const vector<string>& colnames) {
    // TODO: implement Data Thread Function
    // (NOTE: use "write_to_file" function to write to file)
    
    char buffer[256];
    string temp = "";
    const string output = "";
    int add_things = 0;
    write_to_file( filename , colnames.at(0) + ',' +' ' , 1 );
    write_to_file( filename , colnames.at(1) + ',' +' ' , 0 );
    write_to_file( filename , colnames.at(2) + '\n' , 0 );

    while(true)
    {
        int msg_length = bb->pop( buffer , sizeof(buffer) );
        buffer[msg_length] = '\0';
        if( ! strcmp (buffer , "Exit"))
        {
            // std::cout <<  "breaking from data \n";
            break;

        }
        
        else
        {
            // cout << buffer << " is \n ";
            switch (add_things)
            {
            case 0:
                // std::cout << "bruh1 \n";
                temp = ( string ) buffer + ','+' ';
                add_things++;
                break;        
            case 1:
                // std::cout << "bruh2 \n";

                temp = ( string ) buffer + ','+' ';
                add_things++;
                break;        
            case 2:
                // std::cout << "bruh3 \n";
                temp = ( string ) buffer + '\n';

                add_things = 0;
                break;
        
            }
           // output = ( const string ) temp;
            write_to_file(filename,  temp , 0);

        }
    }

    






}

/************** Main Function **************/

int main(int argc, char* argv[]) {

    // variables to be used throughout the program
    vector<string> colnames; // column names
    string fname; // filename
    BoundedBuffer* bb = new BoundedBuffer(3); // BoundedBuffer with cap of 3

    // read flags from input
    int opt;
    while ((opt = getopt(argc, argv, "c:f:")) != -1) {
        switch (opt) {
            case 'c': // parse col names into vector "colnames"
                parse_column_names(colnames, optarg);
                break;
            case 'f':
                fname = optarg;
                break;
            default: // invalid input, based on https://linux.die.net/man/3/getopt
                fprintf(stderr, "Usage: %s [-c colnames] [-f filename]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // TODO: instantiate ui and data threads
   // thread * ui = new thread[1];
  //  thread * data = new thread[1];
    thread ui(ui_thread_function, bb);
   // thread data(ui_thread_function, bb);
    //ui[0] = thread(ui_thread_function, bb); 
    //data[0] = thread(data_thread_function, bb, fname, colnames);
    thread data (data_thread_function, bb, fname, colnames);
    // TODO: join ui_thread
   // thread ui(ui_thread_function, bb);
    ui.join();
   // ui[0].join();
    // TODO: "Once the user has entered 'Exit', the main thread will
    // "send a signal through the message queue to stop the data thread"

    // TODO: join data thread
    data.join();
    // CLEANUP: delete members on heap
    delete bb;

    // delete [] ui;
    // delete [] data;
}

/************** Helper Function Definitions **************/

// function to parse column names into vector
// input: _colnames (vector of column name strings), _opt_input(input from optarg for -c)
void parse_column_names(vector<string>& _colnames, const string& _opt_input) {
    stringstream sstream(_opt_input);
    string tmp;
    while (sstream >> tmp) {
        _colnames.push_back(tmp);
    }
}

// function to append "text" to end of file
// input: filename (name of file), text (text to add to file), first_input (whether or not this is the first input of the file)
void write_to_file(const string& _filename, const string& _text, bool _first_input) {
    // based on https://stackoverflow.com/questions/26084885/appending-to-a-file-with-ofstream
    // open file to either append or clear file
    ofstream ofile;
    if (_first_input)
        ofile.open(_filename);
    else
        ofile.open(_filename, ofstream::app);
    if (!ofile.is_open()) {
        perror("ofstream open");
        exit(-1);
    }

    // sleep for a random period up to 5 seconds
    usleep(rand() % 5000);

    // add data to csv
    ofile << _text;

    // close file
    ofile.close();
}