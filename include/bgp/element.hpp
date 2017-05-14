#ifndef BGPELEMENT_HPP_INCLUDED
#define BGPELEMENT_HPP_INCLUDED

#include <iostream>

extern "C" {
#include "bgpstream_elem.h"

    // FIXME: internal function made for bgpreader, remove once we have our own
    // display functions
    char *bgpstream_elem_custom_snprintf(char *buf, size_t len,
                                         const bgpstream_elem_t *elem,
                                         int print_type);
}

using std::experimental::optional;

namespace BGP {

    typedef std::unique_ptr<bgpstream_elem_t, void(*)(bgpstream_elem_t*)> unique_elem_ptr;
    
    class Element {
    public:

        enum class Type {
            Unknown = BGPSTREAM_ELEM_TYPE_UNKNOWN,
            RIB = BGPSTREAM_ELEM_TYPE_RIB,
            Announcement = BGPSTREAM_ELEM_TYPE_ANNOUNCEMENT,
            Withdrawal = BGPSTREAM_ELEM_TYPE_WITHDRAWAL,
            PeerState = BGPSTREAM_ELEM_TYPE_PEERSTATE
        };
        
        Element(unique_elem_ptr& elem_ptr) : elem(std::move(elem_ptr)) {
            BOOST_LOG_TRIVIAL(trace) << "BGP::Element::Element()";
        }

        Element::Type type(void) {
            return Element::Type(elem->type);
        }
        
        // Return bgpstream string representation
        std::string as_string(void) {

            char buf[2 << 16] = {0};

            // FIXME: report to bgpstream, overflow in bgpstream_elem_snprintf
            if(elem.get() && bgpstream_elem_custom_snprintf(buf, sizeof(buf), elem.get(), 0)) {
                buf[sizeof(buf) - 1] = 0;
                return std::string(buf);
            } else {
                // FIXME: handle error
                return std::string("no element");
            }
        }

    private:
        std::shared_ptr<bgpstream_elem_t> elem;
    };

    std::ostream& operator<<(std::ostream& os, Element& e) {
        os << e.as_string();
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Element::Type& type) {
        switch (type) {
        case BGP::Element::Type::RIB: os << 'R'; break;
        case BGP::Element::Type::Announcement: os << 'A'; break;
        case BGP::Element::Type::Withdrawal: os << 'W'; break;
        case BGP::Element::Type::PeerState: os << 'S'; break;
        default: os << '?'; break;
        };
        return os;
    }

}

#endif
