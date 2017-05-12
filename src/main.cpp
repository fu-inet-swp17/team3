#include <iostream>

#include "bgp/bgp.hpp"

#include "config.hpp"

// int main(int argc,char *argv[]) {
int main(void) {

    BGP::Stream stream;

    stream.start();

    int elem_counter = 0;
    
    while (auto record = stream.next()) {
        // std::cout << "Received Record" << std::endl;
        while (auto element = (*record).next()) {
            // std::cout << "Received Element" << std::endl;
            elem_counter++;
        }
    }

    std::cout << "Read " << elem_counter << " elems" << std::endl;
    
    return 0;
}
