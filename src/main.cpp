#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>
#include <string>
#include <sstream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "bgp/bgp.hpp"
#include "rtr/rtr.hpp"

#include "util/formatters.hpp"
#include "util/program_options.hpp"
#include "util/stream_initializer.hpp"

#include "config.hpp"

using util::ProgramOptions;

int main(int argc, char** argv) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);

    ProgramOptions opts(argc, argv);

    if (auto ret = opts.exit()) return *ret;

    bool rtr = false;

    BGP::Stream stream;
    RTR::Manager mtr;

    if (argc > 1) {
        // Create a stream from the supplied options
        stream = util::stream_from_options(opts);
    } else {
        // Start an example live stream
        stream = util::example_live_stream();
        rtr = true;
    }

    // Start the stream
    stream.start();

    // Start RTRlib
    if (rtr) mtr.start();

    std::cout << "Fetching data..." << std::endl;

    BGP::Record r;

    while (stream.next(r)) {

        BOOST_LOG_TRIVIAL(debug)
            << "[record] "
            << "project = " << r.project() << ", "
            << "collector = " << r.collector() << ", "
            << "status = " << r.status() << ", "
            << "dump_pos = " << r.position() << ", "
            << "dump_time = " << r.dump_time() << ", "
            << "record_time = " << r.record_time();

        if (r.status() == BGP::Record::Status::Valid) {

            int i = 1;

            for (auto it = r.begin(); it != r.end(); ++it) {

                BGP::Element& e = *it;

                std::cout << r.collector() << ": ";

                // Print Record dump type ('U'pdate or 'R'IB)
                switch (r.dump_type()) {
                case BGP::Record::DumpType::Update:
                    std::cout << "U";
                    break;
                case BGP::Record::DumpType::RIB:
                    std::cout << "R";
                    break;
                default:
                    std::cout << "?";
                }

                std::cout << i++;

                // Get timestamp of element
                auto tm = e.timestamp();

                // [<date time>] [AS xxx aaa.bbb.ccc.ddd]
                std::cout
                    << " [" << std::put_time(std::gmtime(&tm), "%F %T") << "] "
                    << "[AS" << e.peer_asnumber() << " "
                    << util::format_ip(e.peer_address()) << "] ";

                bool do_prefix = true;
                bool do_path = false;

                switch (e.type()) {
                case BGP::Element::Type::RIB:
                    std::cout << "RIB"; do_path = true;
                    break;
                case BGP::Element::Type::Announcement:
                    std::cout << "announced"; do_path = true;
                    break;
                case BGP::Element::Type::Withdrawal:
                    std::cout << "withdrawn"; do_path = false;
                    break;
                case BGP::Element::Type::PeerState:
                    std::cout << "changed state"; do_prefix = false;
                    break;
                default:
                    std::cout << "unknown"; do_prefix = false;
                    break;
                }

                std::cout << " ";

                auto pfx = e.prefix();

                // Print announced/withdrawn prefix
                if (do_prefix) {
                    std::cout << "prefix: " << util::format_prefix(pfx) << " ";
                }

                // Print announced AS path
                if (do_path) {
                    std::cout << "next hop: " << util::format_ip(e.next_hop()) << " via path:";

                    uint32_t origin = 0;

                    for (uint32_t asn : e.as_path()) {
                        std::cout << " " << asn;
                        origin = asn;
                    }

                    if (rtr) {

                        // Try verification
                        lrtr_ip_addr addr;
                        lrtr_ip_str_to_addr(util::format_ip(pfx.address).c_str(), &addr);

                        int rtr_res = mtr.validate(origin, &addr, pfx.mask_len);

                        std::cout << " rtr: ";

                        switch (rtr_res) {
                        case 2: std::cout << "\x1b[1;31mINVALID\x1b[0m"; break;
                        case 1: std::cout << "\x1b[1;33mNOT FOUND\x1b[0m"; break;
                        case 0: std::cout << "\x1b[1;32mVERIFIED\x1b[0m" ; break;
                        case -1: std::cout << "\x1b[1;31mERROR\x1b[0m"; break;
                        }
                    }
                }
                std::cout << std::endl;
            }
        }
    }

    if (rtr) mtr.stop();

    return 0;
}
