#ifndef BGPSTREAM_HPP_INCLUDED
#define BGPSTREAM_HPP_INCLUDED

#include <iostream>
#include <memory>
#include <experimental/optional>

extern "C" {
#include "bgpstream.h"
}

using std::experimental::optional;

namespace BGP {

    class Record {
    private:
        std::shared_ptr<bgpstream_record_t> record;
        
    public:
        // create from Record-Object from bgstream_record_t pointer
        Record(bgpstream_record_t* r) : record(r, bgpstream_record_destroy) {
            // std::cout << this << " BGP::Record::Record()" << std::endl;
        }

        ~Record(void) {
            // std::cout << this << " BGP::Record::~Record()" << std::endl;
        }

        // FIXME: create some nice Enum as return value
        int status(void) {
            return record->status;
        }

        optional<bgpstream_elem_t*> next(void) {
            // The returned pointer is guaranteed to be valid until
            // the record is re-used in a subsequent call to
            // bgpstream_get_next_record, or is destroyed with
            // bgpstream_record_destroy FIXME: think about this &
            // manage it
            bgpstream_elem_t* e = bgpstream_record_get_next_elem(record.get());

            if (e) {
                return e;
            } else {
                return {};
            }
        }
    };
    
    class Stream {
    private:
        bgpstream_t* bs;
        
    public:
        Stream(void) : bs(bgpstream_create()) {
            std::cout << "BGP::Stream::Stream()" << std::endl;

            if (!bs) {
                // FIXME: exception class
                throw std::runtime_error("Could not create BGPStream instance");
            }

            // FIXME: Move everything after this into constructor arguments

            // Select bgp data from RRC06 and route-views.jinx collectors only
            bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "rrc06");
            bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "route-views.jinx");
            bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_RECORD_TYPE, "updates");

            // Select a time interval to process (Sun, 10 Oct 2010 10:10:10 GMT -  Sun, 10 Oct 2010 11:11:11 GMT)
            bgpstream_add_interval_filter(bs,1286705410,1286709071);
        }

        void start(void) {
            std::cout << "BGP::Stream::start()" << std::endl;

            // FIXME: exception class or proper return code
            if (bgpstream_start(bs) < 0) {
                throw std::runtime_error("Could not init BGPStream instance");
            }
        }

        optional<Record> next() {
            // FIXME: fix this constructor mess
            bgpstream_record_t* r = bgpstream_record_create();
            int ret = bgpstream_get_next_record(bs, r);
            
            if (ret > 0) {
                return Record(r);
            }
            else if (ret == 0) {
                // end-of-stream reached
                // FIXME: reflect that the stream has ended by setting a private variable in here
                return {};
            }
            else {
                // error occured FIXME: exception class (not runtime)
                throw std::runtime_error("Error in BGP::Stream::next()");
            }
        }

        ~Stream(void) {
            std::cout << "BGP::Stream::~Stream()" << std::endl;
            // free bgpstream_t memory
            bgpstream_destroy(bs);
        }
    };
}

#endif
