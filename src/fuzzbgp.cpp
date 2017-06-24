#include <iostream>
#include <iomanip>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "stream_controller.hpp"
#include "prefix_store.hpp"

#include "util/formatters.hpp"
#include "util/program_options.hpp"
#include "util/stream_initializer.hpp"

#include "config.hpp"

using namespace util;

void process_element(PrefixStore& pfx_store, unsigned collector, const BGP::Element& e);

int main(int argc, char** argv) {
    // Parse command line (largely compatible to bgpreader)
    ProgramOptions opts(argc, argv);

    if (opts.quiet())
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
    else
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    // Exit if program options say so
    if (auto ret = opts.exit()) return *ret;

    // Construct StreamController from BGP::Stream defined by program options
    StreamController sc(stream_from_options(opts));

    // Stores the prefixes
    PrefixStore pfx_store([&](bgpstream_pfx_storage_t pfx, uint32_t origin, std::time_t timestamp, const std::list<bgpstream_pfx_storage_t>& lost) {
            std::cout << timestamp << "|" << origin << "|" << lost.size() << "|";

            for (const auto& pfx : lost) {
                std::cout << util::format_prefix(pfx) << " ";
            }
            std::cout << std::endl;
        });

    sc.on_rib_begin([&] (auto c) {
            std::cout << "Starting RIB import on collector " << c << std::endl;
            pfx_store.flush(c);
            return 0;
        });

    sc.on_updates_begin([&] (auto c) {
            std::cout << "Starting to apply updates on collector " << c << std::endl;
            return 0;
        });

    sc.on_next([&](auto collector, auto& r) {

            // Loop through individual BGP::Elements and process
            for (const auto& e : r)
                process_element(pfx_store, collector, e);

            return 0;
        });

    // Start stream controller
    sc.start();

    return 0;
}

void process_element(PrefixStore& pfx_store, unsigned collector, const BGP::Element& e) {

    switch (e.type()) {

        case BGP::Element::Type::RIB: {
            pfx_store.add_rib_element(collector, e);
            break;
        }

        case BGP::Element::Type::Announcement: {
            pfx_store.add_announcement(collector, e);
            break;
        }

        case BGP::Element::Type::Withdrawal: {
            pfx_store.add_withdrawal(collector, e);
            break;
        }

        case BGP::Element::Type::PeerState: {
            const std::time_t& tm = e.timestamp();

            // FIXME: not handled yet
            BOOST_LOG_TRIVIAL(debug) << std::put_time(std::gmtime(&tm), "%F %T") << ": "
                                     << "Peer state of " << e.peer_asnumber() << " changed from "
                                     << "" << e.peer_state().first << " to " << e.peer_state().second;
            break;
        }

        default: {
            const std::time_t& tm = e.timestamp();

            BOOST_LOG_TRIVIAL(warning) << std::put_time(std::gmtime(&tm), "%F %T") << ": "
                                       << "Invalid element type!";
            break;
        }
    }
}
