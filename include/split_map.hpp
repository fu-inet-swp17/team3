#ifndef PREFIX_MAP_HPP_INCLUDED
#define PREFIX_MAP_HPP_INCLUDED

#include <map>

extern "C" {
#include "bgpstream_utils_pfx.h"
}

namespace prefix {

    struct ipv4_key_t {
        in_addr addr;
        uint8_t mask;

    };

    struct ipv6_key_t {
        in6_addr addr;
        uint8_t mask;
    };

    inline bool operator<(const ipv4_key_t& l, const ipv4_key_t& r) {
        if (l.addr.s_addr < r.addr.s_addr)
            return true;

        if (l.addr.s_addr == r.addr.s_addr)
            return l.mask < r.mask;

        else
            return false;
    }

    inline bool operator<(const ipv6_key_t& l, const ipv6_key_t& r) {

        for (auto i = 0; i < 16; i++) {
            const std::uint8_t ll = l.addr.s6_addr[i];
            const std::uint8_t rr = r.addr.s6_addr[i];

            if (ll < rr)
                return true;
            else if (ll > rr)
                return false;
        }

        return (l.mask < r.mask);
    }

    template <typename T>
    class split_map {

        typedef std::map<ipv4_key_t, T> ipv4_map_t;
        typedef std::map<ipv6_key_t, T> ipv6_map_t;

    private:
        
        ipv4_map_t pfx_ipv4;
        ipv6_map_t pfx_ipv6;

        typename ipv4_map_t::iterator ipv4_hint = pfx_ipv4.end();
        typename ipv6_map_t::iterator ipv6_hint = pfx_ipv6.end();
        
    public:


        T& operator[] (const bgpstream_pfx_storage_t& k) {

            if (k.address.version == BGPSTREAM_ADDR_VERSION_IPV4) {
                ipv4_key_t in { k.address.ipv4, k.mask_len };

                return pfx_ipv4[in];
            } else {
                ipv6_key_t in { k.address.ipv6, k.mask_len };

                return pfx_ipv6[in];
            }
        }


        void insert(const bgpstream_pfx_storage_t& k, const T& v) {
            if (k.address.version == BGPSTREAM_ADDR_VERSION_IPV4) {
                ipv4_key_t in { k.address.ipv4, k.mask_len };

                ipv4_hint = pfx_ipv4.insert(ipv4_hint, std::make_pair(in, v));

            } else {
                ipv6_key_t in { k.address.ipv6, k.mask_len };

                ipv6_hint = pfx_ipv6.insert(ipv6_hint, std::make_pair(in, v));

            }
        }

        typename ipv4_map_t::size_type size(void) {
            return (pfx_ipv4.size() + pfx_ipv6.size());
        }

        typename ipv4_map_t::iterator begin4(void) {
            return pfx_ipv4.begin();
        }

        typename ipv4_map_t::iterator end4(void) {
            return pfx_ipv4.end();
        }

        typename ipv6_map_t::iterator begin6(void) {
            return pfx_ipv6.begin();
        }

        typename ipv6_map_t::iterator end6(void) {
            return pfx_ipv6.end();
        }
    };
}
#endif
