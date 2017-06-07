#include <iomanip>

#include "prefix_store.hpp"
#include "util/formatters.hpp"

using util::format_prefix;

PrefixStore::PrefixStore(void) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::PrefixStore()";
}

void PrefixStore::upsert_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t origin, std::time_t tm) {

    auto& l = pfx_map[pfx];

    auto m = std::find_if(l.begin(), l.end(),
                          [&](const prefix_status v) {
                              return (v.vp == vp);
                          });

    if (m == l.end()) {

        l.emplace_back(vp, origin);
        
    } else if (m->origin != origin) {

        m->origin = origin;
    }
}

void PrefixStore::delete_status(bgpstream_pfx_storage_t pfx, std::uint32_t vp, std::uint32_t origin, std::time_t tm) {

    auto& l = pfx_map[pfx];

    if (l.empty()) {
        // Prefix was not visible from any VP
        BOOST_LOG_TRIVIAL(debug) << "? " << format_prefix(pfx);

    } else {

        // Look if matching prefix_status exists
        auto m = std::find_if(l.begin(), l.end(),
                              [&](const prefix_status v) {
                                  return (v.vp == vp);
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
                BOOST_LOG_TRIVIAL(info) << std::put_time(std::gmtime(&tm), "%F %T") << ": "
                                        << "Lost AS" << std::setw(12) << std::left
                                        << origin << " " << format_prefix(pfx);
            }
        }
    }
}

void PrefixStore::add_withdrawal(const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_withdrawal()";

    delete_status(e.prefix(), e.peer_asnumber(), e.as_path().back(), e.timestamp());
}

void PrefixStore::add_rib_element(const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_rib_element()";

    upsert_status(e.prefix(), e.peer_asnumber(), e.as_path().back(), e.timestamp());
}

void PrefixStore::add_announcement(const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_announcement()";

    upsert_status(e.prefix(), e.peer_asnumber(), e.as_path().back(), e.timestamp());
}

