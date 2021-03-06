#ifndef BGP_ELEMENT_HPP_INCLUDED
#define BGP_ELEMENT_HPP_INCLUDED

#include <iostream>
#include <utility>
#include <list>
#include <sstream>

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

        Element::Type type(void) const {
            return Element::Type(elem->type);
        }

        std::time_t timestamp(void) const {
            return static_cast<std::time_t>(elem->timestamp);
        }

        std::uint32_t peer_asnumber(void) const {
            return elem->peer_asnumber;
        }

        std::pair<PeerState, PeerState> peer_state(void) const {
            return std::pair<PeerState, PeerState>(Element::PeerState(elem->old_state),
                                                   Element::PeerState(elem->new_state));
        }

        bgpstream_addr_storage_t peer_address(void) const {
            return elem->peer_address;
        }

        bgpstream_addr_storage_t next_hop(void) const {
            return elem->nexthop;
        }

        bgpstream_pfx_storage_t prefix(void) const {
            return elem->prefix;
        }

        std::list<uint32_t> as_path(void) const {
            bgpstream_as_path_iter_t it;
            bgpstream_as_path_iter_reset(&it);

            std::list<uint32_t> path;

            while (auto seg = bgpstream_as_path_get_next_seg(elem->aspath, &it)) {
                // FIXME: figure out what the other types mean
                if (seg->type == BGPSTREAM_AS_PATH_SEG_ASN)
                    path.push_back(reinterpret_cast<bgpstream_as_path_seg_asn_t *>(seg)->asn);
                else
                    BOOST_LOG_TRIVIAL(debug) << "AS_PATH ERROR (not implemented)";
            }

            return path;
        }

        std::string as_path_string(void) const {
            std::stringstream ss;

            for (const auto& x : as_path()) {
                ss << x << " ";
            }

            return ss.str();
        }

        // Return bgpstream string representation
        std::string as_string(void) const {

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

    inline std::ostream& operator<<(std::ostream& os, BGP::Element& e) {
        os << e.as_string();
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const BGP::Element::Type& type) {
        switch (type) {
        case BGP::Element::Type::RIB: os << 'R'; break;
        case BGP::Element::Type::Announcement: os << 'A'; break;
        case BGP::Element::Type::Withdrawal: os << 'W'; break;
        case BGP::Element::Type::PeerState: os << 'S'; break;
        default: os << '?'; break;
        };
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const BGP::Element::PeerState& peer_state) {
        switch (peer_state) {
        case BGP::Element::PeerState::Idle: os << "Idle"; break;
        case BGP::Element::PeerState::Connect: os << "Connect"; break;
        case BGP::Element::PeerState::Active: os << "Active"; break;
        case BGP::Element::PeerState::OpenSent: os << "OpenSent"; break;
        case BGP::Element::PeerState::OpenConfirm: os << "OpenConfirm"; break;
        case BGP::Element::PeerState::Established: os << "Established"; break;
        case BGP::Element::PeerState::Clearing: os << "Clearing"; break;
        case BGP::Element::PeerState::Deleted: os << "Deleted"; break;
        default: os << '?'; break;
        };
        return os;
    }


}




#endif
