#ifndef PREFIX_STORE_HPP_INCLUDED
#define PREFIX_STORE_HPP_INCLUDED

#include <list>
#include <map>
#include <set>

#include "bgp/bgp.hpp"
#include "split_map.hpp"

class PrefixStore {

    struct prefix_status {
        uint32_t vp; uint32_t origin; uint32_t collector;

        prefix_status(uint32_t v, uint32_t o, uint32_t c) : vp(v), origin(o), collector(c) {}
    };

    typedef std::function<void(bgpstream_pfx_storage_t, std::uint32_t, std::time_t, const std::list<bgpstream_pfx_storage_t>&)> lost_prefix_cb_t;
    
private:

    prefix::split_map<std::list<prefix_status>> pfx_map;

    std::map<std::uint32_t, std::list<bgpstream_pfx_storage_t>> active_prefixes;
    std::map<std::uint32_t, std::list<bgpstream_pfx_storage_t>> lost_prefixes;

    void upsert_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t origin, std::uint32_t collector, std::time_t tm);
    void delete_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t origin, std::uint32_t collector, std::time_t tm);

    lost_prefix_cb_t lost_prefix_cb;
    
public:

    PrefixStore(lost_prefix_cb_t&& cb);

    void flush(unsigned collector);

    void add_rib_element(unsigned collector, const BGP::Element& e);

    void add_announcement(unsigned collector, const BGP::Element& e);

    void add_withdrawal(unsigned collector, const BGP::Element& e);
};

#endif
