#ifndef PREFIX_STORE_HPP_INCLUDED
#define PREFIX_STORE_HPP_INCLUDED

#include <list>
#include <map>

#include "bgp/bgp.hpp"
#include "split_map.hpp"

class PrefixStore {

    struct prefix_status {
        uint32_t vp; uint32_t origin;

        prefix_status(uint32_t v, uint32_t o) : vp(v), origin(o) {}
    };

private:
    
    prefix::split_map<std::list<prefix_status>> pfx_map;

public:

    void upsert_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t origin, std::time_t tm);

    void delete_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t origin, std::time_t tm);

    PrefixStore(void);

    void add_rib_element(const BGP::Element& e);

    void add_announcement(const BGP::Element& e);

    void add_withdrawal(const BGP::Element& e);
};

#endif
