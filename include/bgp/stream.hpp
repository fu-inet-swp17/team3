#ifndef BGPSTREAM_HPP_INCLUDED
#define BGPSTREAM_HPP_INCLUDED

#include <iostream>
#include <chrono>
#include <memory>
#include <experimental/optional>

#include <boost/log/trivial.hpp>

#include "bgp/record.hpp"

extern "C" {
#include "bgpstream.h"
}

using std::experimental::optional;

namespace BGP {

    typedef std::chrono::time_point<std::chrono::system_clock> time_point;
    
    enum Filter {
        // Filter records based on associated project (e.g. 'ris')
        Project = BGPSTREAM_FILTER_TYPE_PROJECT,
        // Filter records based on collector (e.g. ‘rrc01’)
        Collector = BGPSTREAM_FILTER_TYPE_COLLECTOR,
        // Filter records based on record type (e.g. ‘updates’)
        RecordType = BGPSTREAM_FILTER_TYPE_RECORD_TYPE,
        // Filter elems based on peer ASN.
        ElemPeerASN = BGPSTREAM_FILTER_TYPE_ELEM_PEER_ASN,
        // Filter elems based on prefix.
        ElemPrefix = BGPSTREAM_FILTER_TYPE_ELEM_PREFIX,
        // Filter elems based on the community attribute.
        ElemCommunity = BGPSTREAM_FILTER_TYPE_ELEM_COMMUNITY
    };

    class bad_alloc : std::bad_alloc { };
    class exception : std::exception { };
    class config_error : std::exception { };

    class Stream {

    private:
        std::unique_ptr<bgpstream_t, void(*)(bgpstream_t*)> stream;
        bgpstream_data_interface_id_t if_id;
        bool started = false;
        
    public:
        Stream(void) : stream(bgpstream_create(), bgpstream_destroy) {
            BOOST_LOG_TRIVIAL(trace) << "BGP::Stream::Stream()";
            
            if (!stream) {
                // libbgpstream couldn't allocate memory
                throw bad_alloc();
            }

            if_id = bgpstream_get_data_interface_id(stream.get());
        }

        // Set the data interface by name ('singlefile', 'csvfile', etc.)
        void set_interface(const std::string& if_name) {
            
            bgpstream_data_interface_id_t new_if_id =
                bgpstream_get_data_interface_id_by_name(stream.get(), if_name.c_str());

            // data interface not known by libbgpstream
            if (!new_if_id) {
                throw config_error();
            }

            // set new data interface id
            bgpstream_set_data_interface(stream.get(), new_if_id);

            // update member
            if_id = bgpstream_get_data_interface_id(stream.get());
        }

        // Get a string representation of the currently set data interface
        std::pair<std::string, std::string> get_interface(void) {
            bgpstream_data_interface_info_t* if_info =
                bgpstream_get_data_interface_info(stream.get(), if_id);

            return std::make_pair<std::string, std::string>(std::string(if_info->name),
                                                            std::string(if_info->description));
        }

        void set_interface_option(const std::string& opt_name, const std::string& opt_value) {
            bgpstream_data_interface_option_t* opt_type =
                bgpstream_get_data_interface_option_by_name(stream.get(), if_id, opt_name.c_str());

            if (opt_type)
                bgpstream_set_data_interface_option(stream.get(), opt_type, opt_value.c_str());
            else
                throw config_error();
        }
        
        void add_filter(Filter filter_type, std::string filter_value) {
            if (started) {
                // don't allow filter changes while running
                throw std::logic_error("filter change on already started stream");
            }
            
            bgpstream_add_filter(stream.get(),
                                 static_cast<bgpstream_filter_type_t>(filter_type),
                                 filter_value.c_str());
        }

        // Set an interval filter from begin (inclusive) to end (inclusive)
        void add_interval(unsigned long begin, unsigned long end) {
            BOOST_LOG_TRIVIAL(trace)
                << "BGP::Stream::add_interval("
                << begin << ", " << end << ")";

            if (started) {
                // don't allow filter changes while running
                throw std::logic_error("filter change on already started stream");
            }
            
            bgpstream_add_interval_filter(stream.get(), begin, end);
        }

        // Set an interval filter from begin (inclusive) till forever (live mode)
        void add_interval(unsigned long begin) {
            BOOST_LOG_TRIVIAL(trace) << "BGP::Stream::add_interval(" << begin << ")";

            if (started) {
                // don't allow filter changes while running
                throw std::logic_error("filter change on already started stream");
            }
            
            bgpstream_add_interval_filter(stream.get(), begin, BGPSTREAM_FOREVER);
        }
        
        // Set an interval filter from begin (inclusive) to end (inclusive)
        void add_interval(time_point begin, time_point end) {
            using namespace std::chrono;
            
            auto t0 = duration_cast<seconds>(begin.time_since_epoch()).count();
            auto t1 = duration_cast<seconds>(end.time_since_epoch()).count();
            
            add_interval(t0, t1);
        }
        
        // Set an interval filter from begin (inclusive) till forever (live mode)
        void add_interval(time_point begin) {
            using namespace std::chrono;
            
            auto t = duration_cast<seconds>(begin.time_since_epoch()).count();
            
            add_interval(t);
        }

        // Start the stream
        void start(void) {
            BOOST_LOG_TRIVIAL(trace) << "BGP::Stream::start()";

            if (bgpstream_start(stream.get()) < 0) {
                throw config_error();
            }

            started = true;
        }

        optional<Record> next() {
            BOOST_LOG_TRIVIAL(trace) << "BGP::Stream::next()";

            auto record_ptr = unique_record_ptr(bgpstream_record_create(), bgpstream_record_destroy);

            int ret = bgpstream_get_next_record(stream.get(), record_ptr.get());
            
            if (ret > 0) {
                return Record(record_ptr);;
            }
            else if (ret == 0) {
                // end-of-stream reached
                return {};
            }
            else {
                throw exception();
            }
        }
    };
}

#endif
