#ifndef BGPRECORD_HPP_INCLUDED
#define BGPRECORD_HPP_INCLUDED

#include <iostream>
#include <memory>
#include <experimental/optional>

#include <boost/log/trivial.hpp>

#include "bgp/element.hpp"

extern "C" {
#include "bgpstream_record.h"
}

using std::experimental::optional;

namespace BGP {

    typedef std::unique_ptr<bgpstream_record_t, void(*)(bgpstream_record_t*)> unique_record_ptr;

    class Record {

    public:
        enum Status {
            // The record is valid and may be used.
            Valid = BGPSTREAM_RECORD_STATUS_VALID_RECORD,
            // Source is not empty, but no valid record was found.
            FilteredSource = BGPSTREAM_RECORD_STATUS_FILTERED_SOURCE,
            // Source has no entries.
            EmptySource = BGPSTREAM_RECORD_STATUS_EMPTY_SOURCE,
            // Source is corrupted.
            CorruptedSource = BGPSTREAM_RECORD_STATUS_CORRUPTED_SOURCE,
            // Record is corrupted.
            CorruptedRecord = BGPSTREAM_RECORD_STATUS_CORRUPTED_RECORD
        };

        enum DumpType {
            // The record contains data for a BGP Update message.
            Update = BGPSTREAM_UPDATE, // (0)
            // The record contains data for a BGP RIB message.
            RIB = BGPSTREAM_RIB // (1)
        };

        enum Position {
            //This is the first record of the dump.
            Start = BGPSTREAM_DUMP_START,
            // This is a record in the middle of the dump, i.e. not
            // the first or the last record of the dump
            Middle = BGPSTREAM_DUMP_MIDDLE,
            // This is the last record of the dump.
            End = BGPSTREAM_DUMP_END
        };
        
        // create from Record object from a unique_ptr reference, it's
        // custom deleter will be picked up.
        Record(unique_record_ptr& record_ptr) : record(std::move(record_ptr)) {
            BOOST_LOG_TRIVIAL(trace) << "BGP::Record::Record()";
        }

        Record::Status status(void) {
            return Record::Status(record->status);
        }

        Record::Position position(void) {
            return Record::Position(record->dump_pos);
        }

        std::string project(void) {
            return std::string(record->attributes.dump_project);
        }

        std::string collector(void) {
            return std::string(record->attributes.dump_collector);
        }

        unsigned long dump_time(void) {
            return record->attributes.dump_time;
        }

        unsigned long record_time(void) {
            return record->attributes.record_time;
        }

        Record::DumpType dump_type(void) {
            return Record::DumpType(record->attributes.dump_type);
        }

        void print(void) {
            bgpstream_record_print_mrt_data(record.get());
        }

        optional<Element> next(void) {
            // The returned pointer is guaranteed to be valid until
            // the record is re-used in a subsequent call to
            // bgpstream_get_next_record, or is destroyed with
            // bgpstream_record_destroy.

            bgpstream_elem_t* next = bgpstream_record_get_next_elem(record.get());

            if (next) {
                auto elem_buf_ptr = unique_elem_ptr(bgpstream_elem_create(),
                                                    bgpstream_elem_destroy);
                // FIXME: we copy for now, this might become a bottlehead
                // FIXME: check return value & throw exception if NULL
                bgpstream_elem_copy(elem_buf_ptr.get(), next);

                // gets moved
                return Element(elem_buf_ptr);
            } else {
                return {};
            }
        }

    private:
        std::shared_ptr<bgpstream_record_t> record;
    };

    std::ostream& operator<<(std::ostream& os, Record& e) {
        os << e.record_time() << "|"
           << e.project() << "|"
           << e.collector();
        return os;
    }

}

#endif
