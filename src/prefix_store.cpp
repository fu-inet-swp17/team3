#include <iomanip>

#include "prefix_store.hpp"
#include "util/formatters.hpp"

using util::format_prefix;

PrefixStore::PrefixStore(lost_prefix_cb_t&& cb) : lost_prefix_cb(cb) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::PrefixStore()";
}

void PrefixStore::flush(unsigned collector) {
    BOOST_LOG_TRIVIAL(info) << "Flushing collector #" << collector;

    pfx_map.for_each([=](std::list<prefix_status>& l) {

            std::remove_if(l.begin(), l.end(),
                           [=](const prefix_status v) {
                               return (v.collector == collector);
                           });

            return 0;
        });

    BOOST_LOG_TRIVIAL(info) << "Done";
}

void PrefixStore::upsert_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t origin, std::uint32_t collector, std::time_t tm) {

    auto& l = pfx_map[pfx];

    auto m = std::find_if(l.begin(), l.end(),
                          [&](const prefix_status v) {
                              return ((v.vp == vp) && (v.collector == collector));
                          });

    if (m == l.end()) {

        l.emplace_back(vp, origin, collector);

    } else if (m->origin != origin) {

        m->origin = origin;
    }

    auto& lost = lost_prefixes[origin];

    if (std::find(lost.begin(), lost.end(), pfx) == lost.end()) {

        auto rem = std::remove(lost.begin(), lost.end(), pfx);

        if (rem != lost.end()) {
            lost.erase(rem, lost.end());

            BOOST_LOG_TRIVIAL(info) << std::put_time(std::gmtime(&tm), "%F %T") << ": "
                                    << "AS" << std::setw(10) << std::left << origin << " RECOV "
                                    << format_prefix(pfx) << " "
                                    << "(" << lost.size() << " lost)";
        }
    }
}

void PrefixStore::delete_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t, std::uint32_t collector, std::time_t tm) {

    auto& l = pfx_map[pfx];

    if (l.empty()) {
        // Prefix was not visible from any VP
        BOOST_LOG_TRIVIAL(debug) << "? " << format_prefix(pfx);

    } else {

        // Look if matching prefix_status exists
        auto m = std::find_if(l.begin(), l.end(),
                              [=](const prefix_status v) {
                                  return ((v.vp == vp) &&
                                          (v.collector == collector));
                              });

        if (m == l.end()) {
            BOOST_LOG_TRIVIAL(debug) << "* " << format_prefix(pfx);
            BOOST_LOG_TRIVIAL(debug) << " ? vp: " << vp;
        } else {

            auto origin = (*m).origin;

            l.erase(m);

            BOOST_LOG_TRIVIAL(debug) << "* " << format_prefix(pfx);
            BOOST_LOG_TRIVIAL(debug) << " - vp: " << vp
                                     << " origin: " << origin;

            for (const auto& rem : l) {
                BOOST_LOG_TRIVIAL(debug) << "  vp: " << rem.vp
                                         << " origin: " << rem.origin;
            }

            if (l.empty()) {

                if (pfx.address.version == BGPSTREAM_ADDR_VERSION_IPV4) {

                    prefix::ipv4_key_t in(pfx);

                    bool has_outer = pfx_map.super_prefixes(in, [=] (const prefix::ipv4_key_t& p, std::list<prefix_status>& l) {

                            auto outer = std::find_if(l.begin(), l.end(),
                                                      [=](const prefix_status v) {
                                                          return ((v.vp == vp) &&
                                                                  (v.origin == origin) &&
                                                                  (v.collector == collector));
                                                      });

                            if (outer != l.end()) {
                                std::cout << in << " still reachable through " << p << std::endl;
                                return 1;
                            } else {
                                return 0;
                            }
                        });

                    if (!has_outer) {
                        lost_prefixes[origin].push_back(pfx);
                        
                        BOOST_LOG_TRIVIAL(info) << std::put_time(std::gmtime(&tm), "%F %T") << ": "
                                                << "AS" << std::setw(10) << std::left << origin << " LOST  "
                                                << format_prefix(pfx) << " "
                                                << "(" << lost_prefixes[origin].size() << " lost)";

                        for (auto p : lost_prefixes[origin]) {
                            std::cout << "  - " << format_prefix(p) << std::endl;
                        }
                        // if (active.size() == 0) {
                        //     lost_prefix_cb(pfx, origin, tm, lost);
                        // }
                    }

                } else {

                    /* IPv6 not implemented yet */
                    
                }
            }
        }
    }
}

void PrefixStore::add_withdrawal(unsigned collector, const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_withdrawal()";

    delete_status(e.prefix(), e.peer_asnumber(), e.as_path().back(), collector, e.timestamp());
}

void PrefixStore::add_rib_element(unsigned collector, const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_rib_element()";

    upsert_status(e.prefix(), e.peer_asnumber(), e.as_path().back(), collector, e.timestamp());
}

void PrefixStore::add_announcement(unsigned collector, const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_announcement()";

    upsert_status(e.prefix(), e.peer_asnumber(), e.as_path().back(), collector, e.timestamp());
}
