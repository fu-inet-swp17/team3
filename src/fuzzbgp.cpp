#include <iostream>
#include <iomanip>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "stream_controller.hpp"
#include "prefix_store.hpp"

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

    // Start stream controller
    sc.start();

    // Stores the prefixes
    PrefixStore pfx_store;

    // Stats
    unsigned processed = 0, ignored = 0;

    // Reused per loop
    BGP::Record r;

    // Ignore / Flush / Process for each Record
    StreamController::Instruction op;

    // Numerical identifier for the collector
    unsigned collector;

    while (true) {

        // Get instruction & collector number
        std::tie(op, collector) = sc.next(r);

        switch (op) {
            case StreamController::Instruction::Ignore:
                ignored++;
                break;

            case StreamController::Instruction::Flush:
                pfx_store.flush(collector);

            case StreamController::Instruction::Apply:
            case StreamController::Instruction::Process:
                processed++;

                // Loop through individual BGP::Elements and process
                for (const auto& e : r)
                    process_element(pfx_store, collector, e);
        }

        // Print stats every 10 000 records
        if (!((processed + ignored) % 10000)) {
            BOOST_LOG_TRIVIAL(info) << processed << " records processed (" << ignored << " ignored)";
        }
    }

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
