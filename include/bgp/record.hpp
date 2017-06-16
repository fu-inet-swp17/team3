#ifndef BGPRECORD_HPP_INCLUDED
#define BGPRECORD_HPP_INCLUDED

#include <iostream>
#include <memory>
#include <iterator>
#include <experimental/optional>

#include <boost/log/trivial.hpp>

#include "bgp/element.hpp"
#include "bgp/stream.hpp"

extern "C" {
#include "bgpstream_record.h"
}

using std::experimental::optional;

namespace BGP {

    typedef std::unique_ptr<bgpstream_record_t, void(*)(bgpstream_record_t*)> unique_record_ptr;

    class Stream;

    class Record {

        friend class BGP::Stream;

        class iterator {
            
        private:
            Record& record;
            Element e;
        
        public:
            typedef std::input_iterator_tag iterator_category;
            typedef Element value_type;
            typedef ptrdiff_t difference_type;
            typedef Element* pointer;
            typedef Element& reference;
            
            iterator(Record& r, bool end) : record(r), e(nullptr) {
                if (!end)
                    e.elem = bgpstream_record_get_next_elem(record.record.get());
            }

            bool operator==(const iterator& other) {
                return (this->e.elem == other.e.elem);
            }

            bool operator!=(const iterator& other) {
                return !(*this == other);
            }

            // pre-increment
            iterator& operator++() {
                e.elem = bgpstream_record_get_next_elem(record.record.get());
                return *this;
            }

            // post-increment
            iterator operator++(int) {
                iterator tmp = *this; 
                e.elem = bgpstream_record_get_next_elem(record.record.get());
                return tmp;
            }
            
            reference operator*() {
                return e;
            }

            pointer operator->() {
                return &e;
            }
        };
        
    public:
        enum class Status {
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

        enum class DumpType {
            // The record contains data for a BGP Update message.
            Update = BGPSTREAM_UPDATE, // (0)
            // The record contains data for a BGP RIB message.
            RIB = BGPSTREAM_RIB // (1)
        };

        enum class Position {
            //This is the first record of the dump.
            Start = BGPSTREAM_DUMP_START,
            // This is a record in the middle of the dump, i.e. not
            // the first or the last record of the dump
            Middle = BGPSTREAM_DUMP_MIDDLE,
            // This is the last record of the dump.
            End = BGPSTREAM_DUMP_END
        };
        
        // Default constructor
        Record(void) : record(bgpstream_record_create(), bgpstream_record_destroy) { /* */ }

        Record::Status status(void) const {
            return Record::Status(record->status);
        }

        Record::Position position(void) const {
            return Record::Position(record->dump_pos);
        }

        std::string project(void) const {
            return std::string(record->attributes.dump_project);
        }

        std::string collector(void) const {
            return std::string(record->attributes.dump_collector);
        }

        std::time_t dump_time(void) const {
            return static_cast<std::time_t>(record->attributes.dump_time);
        }
        
        std::time_t record_time(void) const {
            return static_cast<std::time_t>(record->attributes.record_time);
        }

        Record::DumpType dump_type(void) const {
            return Record::DumpType(record->attributes.dump_type);
        }

        bool is_rib(void) const {
            return (dump_type() == Record::DumpType::RIB);
        }

        bool is_update(void) const {
            return (dump_type() == Record::DumpType::Update);
        }

        void print(void) const {
            bgpstream_record_print_mrt_data(record.get());
        }

        iterator begin() {
            return iterator(*this, false);
        }

        iterator end() {
            return iterator(*this, true);
        }

    private:
        unique_record_ptr record;
    };

    inline std::ostream& operator<<(std::ostream& os, Record& e) {
        os << e.record_time() << "|"
           << e.project() << "|"
           << e.collector();
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Record::Position& dump_pos) {
        switch (dump_pos) {
        case Record::Position::Start: os << 'S'; break;
        case Record::Position::Middle: os << 'M'; break;
        case Record::Position::End: os << 'E'; break;
        default: os << '?';
        }
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Record::DumpType& dump_type) {
        switch (dump_type) {
        case Record::DumpType::Update: os << 'U'; break;
        case Record::DumpType::RIB: os << 'R'; break;
        default: os << '?';
        }
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const Record::Status& status) {
        switch (status) {
        case Record::Status::Valid: os << "valid"; break;
        case Record::Status::FilteredSource: os << "filtered source"; break;
        case Record::Status::EmptySource: os << "empty source"; break;
        case Record::Status::CorruptedSource: os << "corrupted source"; break;
        case Record::Status::CorruptedRecord: os << "corrupted record"; break;
        default: os << "invalid status";
        }
        return os;
    }

}

#endif
