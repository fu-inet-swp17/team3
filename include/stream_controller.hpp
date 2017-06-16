#ifndef STREAM_CONTROLLER_HPP_INCLUDED
#define STREAM_CONTROLLER_HPP_INCLUDED

#include <string>
#include <ctime>
#include <vector>

#include "bgp/stream.hpp"

class StreamController {

public:

    enum class Instruction {
        // Record contains updates for which we don't have RIBs yet -> ignore
        Ignore,
        // This record contains RIBs after an update period -> flush existing routes
        Flush,
        // This record contains RIBs or updates -> process
        Process
    };

private:

    struct collector_entry {
        std::string name;
        
        std::time_t rib_time;
        std::time_t last_update_dump;
        std::time_t last_update_record;

        unsigned long ribs_processed;
        unsigned long updates_processed;
        unsigned long updates_ignored;

        bool in_rib = false;

        collector_entry(const std::string& n);
        void reset(void);
        void print() const;
    };

    std::vector<collector_entry> log;
    BGP::Stream stream;

    Instruction update(const BGP::Record& r, collector_entry& e);

    std::pair<Instruction, unsigned> handle(const BGP::Record& r);

public:
    
    // Construct by moving BGP::Stream
    StreamController(BGP::Stream&& s);

    // Start the encapsulated BGP::Stream
    void start(void);

    // Fill next Record and receive pair of (Instruction, collector number)
    std::pair<Instruction, unsigned> next(BGP::Record& r);

    // Print collector stats
    void print(void) const;
};

#endif
