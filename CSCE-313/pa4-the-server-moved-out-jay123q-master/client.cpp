// your PA3 client code here
#include <fstream>
#include <iostream>
#include <thread>
#include <sys/time.h>
#include <sys/wait.h>
#include <string>
#include "BoundedBuffer.h"
#include "common.h"
#include "Histogram.h"
#include "HistogramCollection.h"
#include "TCPRequestChannel.h"

// ecgno to use for datamsgs
#define EGCNO 1

using namespace std;
    struct Patient
    {
        int id_number;
        double info;
    };
const string ToString(char* b)
{
    string a = b;
    return a;
}

void patient_thread_function ( BoundedBuffer *  req_buff, int p , int n ){
    // functionality of the patient threads
    datamsg data( p , 0.0 , EGCNO );
    for( int i = 0; i < n ; i++ )
    {
        req_buff->push((char*) (&data), sizeof(datamsg));
        data.seconds += 0.004;
    }
}

void file_thread_function (TCPRequestChannel *c, BoundedBuffer *  bound_buff, int buff_cap , string name) {
    // functionality of the file thread
    // referencing pa 1
    __int64_t file_size = 0;   
    string path_name = "received/"+name;
    int packet_size = sizeof(filemsg)+path_name.size()+1;
    char  buf [1024];
    // get the filemsg

    filemsg fm( 0 , 0 );
	memcpy(buf, &fm, sizeof(filemsg));
    strcpy(buf+sizeof(filemsg), name.c_str()); // put file name
    
    c->cwrite(buf, packet_size);

    c->cread(&file_size, sizeof(file_size));
    // file length aquired
    int length_left = file_size;


    FILE * fp = fopen(path_name.c_str(),"w"); 
    fseek( fp , file_size , SEEK_SET );
    fclose(fp);

    filemsg * new_file = (filemsg *) buf;

    while(length_left > 0 )
    {
        // find min
        if(buff_cap > length_left )
        {
            new_file->length = length_left;
        }
        else
        {
            new_file->length = buff_cap;
        }
      
        bound_buff->push((char*) new_file , packet_size );     
        new_file-> offset += new_file->length;
        length_left -= new_file->length;
    }

}

void worker_thread_function ( TCPRequestChannel *c, BoundedBuffer * reply_b, BoundedBuffer * request_b , int m) {
    // functionality of the worker threads
    double receive_data = 0.0;
    char * buffer = new char [m];
    while(true)
    {
        int length_server = request_b->pop(buffer,m); // buffer is the request
        MESSAGE_TYPE * message = (MESSAGE_TYPE * ) buffer;


        if(*message == FILE_MSG)
        {

            filemsg * fm = (filemsg *) message;
            string file_name = (char*)(fm + 1);
            string file_path = "received/"+file_name;
             char * r_buf =  new char [fm->length]; //size of the request 
            c->cwrite( buffer , sizeof(filemsg) + file_name.size() +1 ) ; // name of request
            c->cread( r_buf , fm->length ) ; // buffer2 stores response

            // add recieved as a directory
            FILE * fp = fopen(file_path.c_str(), "r+");
        //asdf
            // position cursor correctly and use the filemsg offset
            fseek(fp , fm->offset , SEEK_SET );
            fwrite( r_buf , 1 , fm->length  , fp );
            fclose(fp);

            delete [] r_buf;


        }
        else if(*message == DATA_MSG)
        {

            // a is using acutal request datamsg is not
            datamsg * a = (datamsg * )buffer;


            c->cwrite(a,sizeof(datamsg));
            c->cread(&receive_data, sizeof(double)); // size aquired
          
            Patient data { a->person , receive_data };

            reply_b->push( ( char * ) &data , sizeof(data) );
            

        }
        else if(*message == QUIT_MSG)
        {
            // write the buffer with quit into the vector given by pop of data
            c->cwrite( buffer, length_server );
            delete c;
            break;
        }
        else{
            std::cout << " else case 90 \n ";
        }

    }
    delete [] buffer;
}

void histogram_thread_function (BoundedBuffer * rep_buff, HistogramCollection * hc) {
    // functionality of the histogram threads


    while(true)
    {

        Patient data_point; // apply the struct to each area of the data
        rep_buff->pop( (char*) &data_point , sizeof(data_point) );

        if(data_point.id_number <= 0 )
        {
            break;
        }
        // cout << "asdjnkfgjnka "<< data_point.id_number << " ,  " << data_point.info << std::endl;
        hc->update( data_point.id_number , data_point.info );

    }

    
    // push a negative -100 -100 pair at the end
}


int main (int argc, char* argv[]) {
    int n = 1000;	// default number of requests per "patient"
    int p = 10;		// number of patients [1,15]
    int w = 100;	// default number of worker threads
	int h = 20;		// default number of histogram threads
    int b = 20;		// default capacity of the request buffer (should be changed)
	int m = MAX_MESSAGE;	// default capacity of the message buffer
    string  a = "127.0.0.1"; // ip address
    string r = "8080"; // number
	string f = "";	// name of file to be transferred
    // read arguments
    int opt;
	while ((opt = getopt(argc, argv, "n:p:w:h:b:m:f:a:r:")) != -1) {
		switch (opt) {
			case 'n':
				n = atoi(optarg);
                break;
			case 'p':
				p = atoi(optarg);
                break;
			case 'w':
				w = atoi(optarg);
                break;
			case 'h':
				h = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
                break;
			case 'm':
				m = atoi(optarg);
                break;
			case 'f':
				f = optarg;
                break;
			case 'a':
				 a = optarg; // check lines below used to assign ip_addy
                break;
			case 'r':
				 r = optarg;// check lines below used to assign port
                break;
		}
	}

	// fork and exec the server
    int pid = fork();
    if (pid == 0) {
        execl("./server", "./server", "-m", (char*) to_string(m).c_str(), nullptr);
    }

    



	// initialize overhead (including the control channel)
	TCPRequestChannel* chan = new TCPRequestChannel(a,r);
    BoundedBuffer request_buffer(b);
    BoundedBuffer response_buffer(b);
	HistogramCollection hc;



    // making histograms and adding to collection
    for (int i = 0; i < p; i++) {
        Histogram* h = new Histogram(10, -2.0, 2.0);
        hc.add(h);
    }
	
	// record start time
    struct timeval start, end;
    gettimeofday(&start, 0);

    /* create all threads here */
    thread * pat_thread = new thread[p];
    thread * worker_thread = new thread[w];
    thread * hist_thread = new thread[h];
    thread * buffer_thread = new thread[b];
    thread file_ripper;

    // create channels
    vector <TCPRequestChannel*> worker_channels;
    for (int i = 0; i < w; i++)
    {
        // MESSAGE_TYPE temp = NEWCHANNEL_MSG;
        // chan->cwrite(&temp,sizeof(MESSAGE_TYPE));
        // char name[256];
        // chan->cread(name,sizeof(name));
        TCPRequestChannel * new_channel = new TCPRequestChannel( a , r );
        // send new channel request, call name and constructoer
        worker_channels.push_back(new_channel);


    }


    if ( f.empty() )
    { 
        // file name so we need to process data
        for (int i = 0; i < p; i++)
        {
           int thread_place  = i + 1;
           pat_thread[i] = thread(patient_thread_function, &request_buffer , thread_place , n);
        }
        // now run historgram creation
        for (int i = 0; i < h ; i++)
        {
           hist_thread[i] = thread(histogram_thread_function, &response_buffer , &hc);
        }        
    }
    else{
        // if no file exists 
        file_ripper = thread(file_thread_function , chan ,  &request_buffer , m , f);
    }


    


    for (int i = 0; i < w; i++)
    {
        worker_thread[i] = thread(worker_thread_function, worker_channels[i] , &response_buffer,&request_buffer,m);

    }
    

	/* join all threads here */
    if ( f.empty() )
    { 
        // file name so we need to process data
        for (int i = 0; i < p; i++)
        {
           pat_thread[i].join();
        }
        // DONT HVAE HISTOGRAHM HERE when the patient thread joins we know we are out of requests


    }
    else{
        // if no file exists 
        file_ripper.join();
    }



    for( int i = 0 ; i < w; ++i)
    {
        // tell EVERY SINGLE WORKER to quit
        MESSAGE_TYPE quit = QUIT_MSG;
        request_buffer.push( (char *) &quit ,  sizeof(quit) );
    }
    for( int i = 0 ; i < w; ++i)
    {
        worker_thread[i].join();
    }


    if(f.empty() )
    {
        for (int i = 0; i < h; i++)
        {
            // tell each one to quit
            Patient quit = {0,0};
            response_buffer.push((char*)&quit, sizeof(quit));
        }
        for (int i = 0; i < h; i++)
        {
            hist_thread[i].join();
        }
        

    }




	// record end time
    gettimeofday(&end, 0);

    // print the results
	if (f == "") {
		hc.print();
	}
    int secs = ((1e6*end.tv_sec - 1e6*start.tv_sec) + (end.tv_usec - start.tv_usec)) / ((int) 1e6);
    int usecs = (int) ((1e6*end.tv_sec - 1e6*start.tv_sec) + (end.tv_usec - start.tv_usec)) % ((int) 1e6);
    std::cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;

	// quit and close control channel
    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    std::cout << "All Done!" << endl;
    delete chan;

	// wait for server to exit
	wait(nullptr);

    // delete all thread
    delete [] pat_thread;
    delete [] worker_thread; 
    delete [] hist_thread; 
    delete [] buffer_thread; 



    

}
