#ifndef FORMATTERS_HPP_INCLUDED
#define FORMATTERS_HPP_INCLUDED

#include <arpa/inet.h>

extern "C" {
#include "bgpstream_utils_addr.h"
}

namespace util {

    // Format a bgpstream_addr_storage_t as string
    std::string format_ip(const bgpstream_addr_storage_t& addr) {
        char buf[INET6_ADDRSTRLEN] = {0};

        if (addr.version == BGPSTREAM_ADDR_VERSION_IPV4) {
            inet_ntop(AF_INET, &addr.ipv4, &buf[0], sizeof buf);
        }
        else if (addr.version == BGPSTREAM_ADDR_VERSION_IPV6) {
            inet_ntop(AF_INET6, &addr.ipv6, &buf[0], sizeof buf);
        }
        else {
            return "mal.for.med.ip";
        }

        return std::string(buf);
    }

    // Format a bgpstream_pfx_storage_t as string
    std::string format_prefix(const bgpstream_pfx_storage_t& pfx) {
        std::stringstream ss;

        ss << format_ip(pfx.address) << "/" << std::to_string(pfx.mask_len);

        return ss.str();
    }
}

#endif
