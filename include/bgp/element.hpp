#ifndef BGPELEMENT_HPP_INCLUDED
#define BGPELEMENT_HPP_INCLUDED

#include <iostream>
#include <utility>
#include <list>

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

    class Element {
    public:

        enum class Type {
            Unknown = BGPSTREAM_ELEM_TYPE_UNKNOWN,
            RIB = BGPSTREAM_ELEM_TYPE_RIB,
            Announcement = BGPSTREAM_ELEM_TYPE_ANNOUNCEMENT,
            Withdrawal = BGPSTREAM_ELEM_TYPE_WITHDRAWAL,
            PeerState = BGPSTREAM_ELEM_TYPE_PEERSTATE
        };

        enum class PeerState {
            Unknown = BGPSTREAM_ELEM_PEERSTATE_UNKNOWN,
            Idle = BGPSTREAM_ELEM_PEERSTATE_IDLE,
            Connect = BGPSTREAM_ELEM_PEERSTATE_CONNECT,
            Active = BGPSTREAM_ELEM_PEERSTATE_ACTIVE,
            OpenSent = BGPSTREAM_ELEM_PEERSTATE_OPENSENT,
            OpenConfirm = BGPSTREAM_ELEM_PEERSTATE_OPENCONFIRM,
            Established = BGPSTREAM_ELEM_PEERSTATE_ESTABLISHED,
            Clearing = BGPSTREAM_ELEM_PEERSTATE_CLEARING,
            Deleted = BGPSTREAM_ELEM_PEERSTATE_DELETED
        };

        bgpstream_elem_t* elem;

        Element(bgpstream_elem_t* e) : elem(e) { /* */ };
        
        Element::Type type(void) {
            return Element::Type(elem->type);
        }

        std::time_t timestamp(void) {
            return static_cast<std::time_t>(elem->timestamp);
        }

        std::uint32_t peer_asnumber(void) {
            return elem->peer_asnumber;
        }

        std::pair<PeerState, PeerState> peer_state(void) {
            return std::pair<PeerState, PeerState>(Element::PeerState(elem->old_state),
                                                   Element::PeerState(elem->new_state));
        }

        bgpstream_addr_storage_t peer_address(void) {
            return elem->peer_address;
        }

        bgpstream_addr_storage_t next_hop(void) {
            return elem->nexthop;
        }

        bgpstream_pfx_storage_t prefix(void) {
            return elem->prefix;
        }

        std::list<uint32_t> as_path(void) {
            bgpstream_as_path_iter_t it;
            bgpstream_as_path_iter_reset(&it);

            std::list<uint32_t> path;

            while (auto seg = bgpstream_as_path_get_next_seg(elem->aspath, &it)) {
                // FIXME: figure out what the other types mean
                if (seg->type == BGPSTREAM_AS_PATH_SEG_ASN)
                    path.push_back(reinterpret_cast<bgpstream_as_path_seg_asn_t *>(seg)->asn);
            }

            return path;
        }

        // Return bgpstream string representation
        std::string as_string(void) {

            char buf[2 << 16] = {0};

            // FIXME: report to bgpstream, overflow in bgpstream_elem_snprintf
            if(elem && bgpstream_elem_custom_snprintf(buf, sizeof(buf), elem, 0)) {
                buf[sizeof(buf) - 1] = 0;
                return std::string(buf);
            } else {
                // FIXME: handle error
                return std::string("no element");
            }
        }
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
