#include <iomanip>

#include "prefix_store.hpp"
#include "util/formatters.hpp"

using util::format_prefix;

PrefixStore::PrefixStore(void) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::PrefixStore()";
}

void PrefixStore::add_rib_element(const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_rib_element()";
    add_element(e);
}

void PrefixStore::add_announcement(const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_announcement()";
    add_element(e);
}

void PrefixStore::add_element(const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_element()";

    const auto& pfx = e.prefix();

    prefix_status pfx_stat { e.peer_asnumber(), e.as_path().back() };

    // Search for the prefix
    const auto pfx_match = pfx_map.find(pfx);

    // Found the prefix
    if (pfx_match != pfx_map.end()) {

        // If same prefix & same element type (RIB, update, ...)
        // then don't print prefix again for debugging output.
        if ((last_inserted_prefix != pfx) ||
            (last_element_type != e.type()))
            BOOST_LOG_TRIVIAL(debug) << "* " << format_prefix(pfx);

        auto& l = pfx_match->second;

        // Look if matching prefix_status exists
        auto stat_match = std::find_if(l.begin(), l.end(),
                                       [&](const prefix_status v) {
                                           return (v.vp == pfx_stat.vp);
                                       });

        // Found no prefix_status matching this VP
        if (stat_match == l.end()) {

            // Insert (VP, origin AS) mapping for this prefix
            pfx_map[pfx].push_back(pfx_stat);

            BOOST_LOG_TRIVIAL(debug) << " + vp: " << pfx_stat.vp
                                     << " origin: " << pfx_stat.origin_as;
        }
        // Found prefix_status matching this VP
        else {

            if (stat_match->origin_as != pfx_stat.origin_as) {

                auto old_origin = stat_match->origin_as;

                // Update origin AS for this prefix
                stat_match->origin_as = pfx_stat.origin_as;

                BOOST_LOG_TRIVIAL(debug) << " ! vp: " << pfx_stat.vp
                                         << " origin: " << pfx_stat.origin_as
                                         << " (before: " << old_origin << ")";
            }
        }
    }
    // This prefix was never seen before
    else {
        // Create prefix entry and insert (VP, origin AS) mapping for it
        pfx_map[pfx].push_back(pfx_stat);

        BOOST_LOG_TRIVIAL(debug) << "+ " << format_prefix(pfx);
        BOOST_LOG_TRIVIAL(debug) << " + vp: " << pfx_stat.vp
                                 << " origin: " << pfx_stat.origin_as;
    }

    last_inserted_prefix = pfx;
    last_element_type = e.type();
}

void PrefixStore::add_withdrawal(const BGP::Element& e) {
    BOOST_LOG_TRIVIAL(trace) << "PrefixStore::add_withdrawal()";

    const auto& pfx = e.prefix();
    const auto& vp = e.peer_asnumber();
    const auto& tm = e.timestamp();

    // Search for the prefix
    const auto pfx_match = pfx_map.find(pfx);

    // Found the prefix
    if (pfx_match != pfx_map.end()) {

        auto& l = pfx_match->second;

        // Look if matching prefix_status exists
        auto stat_match = std::find_if(l.begin(),
                                       l.end(),
                                       [&](const prefix_status v) {
                                           return (v.vp == vp);
                                       });

        // No matching prefix_status for this VP found
        if (stat_match == l.end()) {
            BOOST_LOG_TRIVIAL(debug) << "* " << format_prefix(pfx);
            BOOST_LOG_TRIVIAL(debug) << " ? vp: " << vp;
        }
        // Found matching prefix_status for this VP
        else {
            prefix_status saved_stat = *stat_match;

            // Delete prefix_status for this VP
            l.erase(stat_match);

            BOOST_LOG_TRIVIAL(debug) << "* " << format_prefix(pfx);
            BOOST_LOG_TRIVIAL(debug) << " - vp: " << saved_stat.vp
                                     << " origin: " << saved_stat.origin_as;

            for (const auto& rem : l) {
                BOOST_LOG_TRIVIAL(debug) << "  vp: " << rem.vp
                                         << " origin: " << rem.origin_as;
            }

            if (l.empty()) {
                BOOST_LOG_TRIVIAL(info) << std::put_time(std::gmtime(&tm), "%F %T") << ": "
                                        << "Lost AS" << std::setw(12) << std::left << saved_stat.origin_as << " " << format_prefix(pfx);
            }
        }
    }
    // This prefix was never seen before
    else {
        BOOST_LOG_TRIVIAL(debug) << "? " << format_prefix(pfx);
    }

    last_element_type = e.type();
}
