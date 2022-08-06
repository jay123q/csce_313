#include "BoundedBuffer.h"
#include <iostream>
using namespace std;


BoundedBuffer::BoundedBuffer (int _cap) : cap(_cap) {
    // modify as needed
}

BoundedBuffer::~BoundedBuffer () {
    // modify as needed
}

void BoundedBuffer::push (char* msg, int size) {
    // 1. Convert the incoming byte sequence given by msg and size into a vector<char>
    vector<char> data(msg, msg + size);
    // 2. Wait until there is room in the queue (i.e., queue lengh is less than cap)
    unique_lock<mutex> mux_unique(m);
    // 3. Then push the vector at the end of the queue
    // 4. Wake up threads that were waiting for push
    push_perms.wait(mux_unique, [this]
    {
        return (int)(this->size()) < cap; 
    });
    // use std:: mutex to lock
    q.push(data);
    m.unlock(); // you had a deadlock by having this too before the wait

    pop_perms.notify_one();
}

int BoundedBuffer::pop (char* msg, int size) {
    // only want one thing working    // 1. Wait until the queue has at least 1 item
    unique_lock<mutex> mux_unique (m);

    pop_perms.wait(mux_unique, [this]
        { return this->size() > 0;}
    );


    vector <char> d = q.front(); 

    q.pop(); // 2. Pop the front item of the queue. The popped item is a vector<char>
    // 3. Convert the popped vector<char> into a char*, copy that into msg; assert that the vector<char>'s length is <= size
    int convert_size_int = (int) d.size();
    assert( (convert_size_int <= size ));

    
    memcpy(msg,d.data(), d.size() );
    // 4. Wake up threads that were waiting for pop
    mux_unique.unlock(); // stop touching queue then unlock 

    push_perms.notify_one();

    // 5. Return the vector's length to the caller so that they know how many bytes were popped
    return d.size();
}

size_t BoundedBuffer::size () {
    return q.size();
}