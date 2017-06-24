#ifndef STREAM_CONTROLLER_HPP_INCLUDED
#define STREAM_CONTROLLER_HPP_INCLUDED

#include <string>
#include <ctime>
#include <vector>

#include "bgp/stream.hpp"

class StreamController {

    typedef std::function<int(unsigned)> status_fn;
    typedef std::function<int(unsigned, BGP::Record&)> consumer_fn;

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
    BGP::Record record;

    bool update(const BGP::Record& r, collector_entry& e);

    std::pair<bool, unsigned> handle(const BGP::Record& r);

    status_fn begin_rib_fn = [] (auto c) {
        BOOST_LOG_TRIVIAL(info) << "Default begin_rib_fn called for collector " << c << std::endl;
        return 0;
    };

    status_fn begin_updates_fn = [] (auto c) {
        BOOST_LOG_TRIVIAL(info) << "Default start_rib_fn called for collector " << c << std::endl;
        return 0;
    };

    consumer_fn next_fn = [] (auto c, auto&) {
        BOOST_LOG_TRIVIAL(info) << "Default next_fn called for collector " << c << std::endl;
        return 0;
    };

public:

    // Construct by moving BGP::Stream
    StreamController(BGP::Stream&& s);

    void on_rib_begin(const status_fn& fn) {
        begin_rib_fn = fn;
    }

    void on_updates_begin(const status_fn& fn) {
        begin_updates_fn = fn;
    }

    void on_next(const consumer_fn& fn) {
        next_fn = fn;
    }

    // Start the encapsulated BGP::Stream
    void start(void);

    // Print collector stats
    void print(void) const;
};

#endif
