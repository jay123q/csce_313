#include "TCPRequestChannel.h"

using namespace std;


TCPRequestChannel::TCPRequestChannel (const std::string _ip_address, const std::string _port_no ) {
    // if server
    int port = atoi( _port_no.c_str() );
    if(_ip_address == "") // how fix
    {
        // create a socket on port, specify the requirments of the socket, domian, type, protocol
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server_socket < 0)
        {
            perror("server flames");
            exit(-1);
        }
        this->sockfd = server_socket; //  we are operating on the 


        struct sockaddr_in add_new_server;
        memset(&add_new_server, 0 , sizeof(add_new_server) ); // set zeros in server_socket

        add_new_server.sin_family = AF_INET;
        // add_new_server.sin_addr.s_addr = htonl(INADDR_ANY);
        add_new_server.sin_addr.s_addr = INADDR_ANY;
        add_new_server.sin_port = htons((short)port);
        auto sizeof_add = sizeof(add_new_server);
        // bind socket to address from machine, use get address ino
        int check_bind = bind( server_socket , (struct sockaddr *) &add_new_server , sizeof_add );
        if(check_bind < 0)
        {
            perror("bind flames");
            exit(-1);
        }
        // this sets up listening and call listen to mark the socket as listening
        int check_listen = listen( server_socket, 30 );
        if( check_listen < 0 )
        {
            perror("listen flames");
            exit(-1);
        }
        // watch byte ordering function given in powerpoint     
    }
    else
    {
    // if client us ip address
    // create the socket  on the port, domain, repeat of above
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    this->sockfd = client_socket; // update sockfd
    // connect socket to ip address no get info

    struct sockaddr_in info_abt_server;
    memset(&info_abt_server, 0 , sizeof(info_abt_server) );
    info_abt_server.sin_family = AF_INET;
    // add_new_server.sin_addr.s_addr = htonl(INADDR_ANY);
    info_abt_server.sin_port = htons((short)port);

    // convert ip address to binary
    inet_aton(_ip_address.c_str() , &info_abt_server.sin_addr );

    connect( client_socket , (struct sockaddr *) &info_abt_server , sizeof(info_abt_server));
    }

}

TCPRequestChannel::TCPRequestChannel (int _sockfd) {
// literally just setting up the number variable
// you call accept_conn
    this->sockfd = _sockfd;
}

TCPRequestChannel::~TCPRequestChannel () {
    //close the sockfd
    close(this->sockfd);
}

int TCPRequestChannel::accept_conn () {
    //implement the accept function in socket api
    struct sockaddr_storage store;
    //accept(...) -  returns sockfd of client
    socklen_t size = sizeof(store);
    int active_socket = accept(this->sockfd, (struct sockaddr *) &store , &size  );
    if(active_socket < 0 ){ strerror(errno);}
    return active_socket;
}

int TCPRequestChannel::cread (void* msgbuf, int msgsize) {
// use file functions read write/ send
    ssize_t temp = read(this->sockfd, msgbuf , msgsize );
    return temp;
}

int TCPRequestChannel::cwrite (void* msgbuf, int msgsize) {
    ssize_t temp = write( this->sockfd , msgbuf , msgsize );
    return temp;
}
