#ifndef PREFIX_STORE_HPP_INCLUDED
#define PREFIX_STORE_HPP_INCLUDED

#include <list>
#include <map>

#include "bgp/bgp.hpp"

class PrefixStore {

    struct prefix_status { uint32_t vp; uint32_t origin_as; };

    // Type for holding prefix -> [prefix_status] std::mapping
    typedef std::map<bgpstream_pfx_storage_t, std::list<prefix_status>> pfx_map_t;
    
private:
    
    pfx_map_t pfx_map;

    bgpstream_pfx_storage_t last_inserted_prefix;

    BGP::Element::Type last_element_type;

    void add_element(const BGP::Element& e);
    
public:
    
    PrefixStore(void);

    void add_rib_element(const BGP::Element& e);

    void add_announcement(const BGP::Element& e);

    void add_withdrawal(const BGP::Element& e);
};

#endif
