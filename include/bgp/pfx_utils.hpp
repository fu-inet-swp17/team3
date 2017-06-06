#ifndef BGP_PFX_UTILS_HPP_INCLUDED
#define BGP_PFX_UTILS_HPP_INCLUDED

extern "C" {
#include "bgpstream_utils_pfx.h"
}

inline bool operator==(const bgpstream_pfx_storage_t& l, const bgpstream_pfx_storage_t& r) {

    return (bgpstream_pfx_storage_equal(const_cast<bgpstream_pfx_storage_t *>(&l),
                                        const_cast<bgpstream_pfx_storage_t *>(&r)) != 0);
}

inline bool operator!=(const bgpstream_pfx_storage_t& l, const bgpstream_pfx_storage_t& r) {

    return !(l == r);
}


inline bool operator<(const bgpstream_pfx_storage_t& l, const bgpstream_pfx_storage_t& r) {

    auto l_ver = l.address.version;
    auto r_ver = r.address.version;

    switch (l_ver) {
    case BGPSTREAM_ADDR_VERSION_IPV4:
        switch (r_ver) {
        // compare ipv4 vs. ipv4
        case BGPSTREAM_ADDR_VERSION_IPV4: {
            std::uint32_t l_addr = ntohl(l.address.ipv4.s_addr);
            std::uint32_t r_addr = ntohl(r.address.ipv4.s_addr);
        
            if (l_addr == r_addr)
                return (l.mask_len < r.mask_len);
            else
                return (l_addr < r_addr);
        }
        case BGPSTREAM_ADDR_VERSION_IPV6: return true;
        default: return false;
        }
    case BGPSTREAM_ADDR_VERSION_IPV6: {
        switch (r_ver) {
        // compare ipv6 vs. ipv6
        case BGPSTREAM_ADDR_VERSION_IPV6: {
            for (auto i = 0; i < 16; i++) {
                std::uint8_t ll = l.address.ipv6.s6_addr[i];
                std::uint8_t rr = r.address.ipv6.s6_addr[i];

                if (ll < rr)
                    return true;
                if (ll > rr)
                    return false;
                if (ll == rr)
                    continue;
            }
        
            return (l.mask_len < r.mask_len);
        }
        case BGPSTREAM_ADDR_VERSION_IPV4: return false;
        default: return false;
        }
    }
    default: return false;
    }
}

#endif
