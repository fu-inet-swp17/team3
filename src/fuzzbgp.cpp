#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>
#include <string>
#include <algorithm>
#include <map>
#include <tuple>
#include <sstream>
#include <unordered_set>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "bgp/bgp.hpp"
#include "rtr/rtr.hpp"

#include "util/formatters.hpp"
#include "util/program_options.hpp"
#include "util/stream_initializer.hpp"

#include "collector_stream.hpp"
#include "prefix_store.hpp"

#include "config.hpp"

using util::ProgramOptions;

struct PrefixStatus {
    uint32_t vp;
    uint32_t origin_as;
};

typedef std::map<bgpstream_pfx_storage_t, std::list<PrefixStatus>> pfx_tree_t;


int main(int argc, char** argv) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);

    ProgramOptions opts(argc, argv);

    if (auto ret = opts.exit()) return *ret;

    CollectorStream stream("route-views2");
    stream.start(std::time(nullptr) - (60 * 60 * 8));

    // Use this for csv-file input
    // BGP::Stream stream = util::stream_from_options(opts);
    // stream.start();

    PrefixStore pfx_store;

    unsigned records = 0;

    BGP::Record r;

    while (stream.next(r)) {

        if (r.status() == BGP::Record::Status::Valid) {

            for (const auto& e : r) {
                 switch (e.type()) {
                 case BGP::Element::Type::RIB: {
                      pfx_store.add_rib_element(e);
                      break;
                 }
                 case BGP::Element::Type::Announcement: {
                      pfx_store.add_announcement(e);
                      break;
                 }
                 case BGP::Element::Type::Withdrawal: {
                      pfx_store.add_withdrawal(e);
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
        }

        else {
             const std::time_t& tm = r.record_time();

             BOOST_LOG_TRIVIAL(warning) << std::put_time(std::gmtime(&tm), "%F %T") << ": "
                                        << "Record not valid!";
        }

        if (!(++records % 10000))
            BOOST_LOG_TRIVIAL(info) << records << " records processed.";
    }

    return 0;
}
