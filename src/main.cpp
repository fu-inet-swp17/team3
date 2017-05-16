#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <iomanip>
#include <string>
#include <sstream>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "bgp/bgp.hpp"
#include "rtr/rtr.hpp"

#include "util/program_options.hpp"

#include "config.hpp"

using util::ProgramOptions;

// FIXME: move into util
std::string format_ip(const bgpstream_addr_storage_t& addr) {
    char buf[INET6_ADDRSTRLEN] = {0};

    if (addr.version == BGPSTREAM_ADDR_VERSION_IPV4) {
        inet_ntop(AF_INET, &addr.ipv4, (char*)&buf[0], sizeof buf);
    }
    else if (addr.version == BGPSTREAM_ADDR_VERSION_IPV6) {
        inet_ntop(AF_INET6, &addr.ipv6, &buf[0], sizeof buf);
    }
    else {
        return " -- malformed ip -- ";
    }

    return std::string(buf);
}

// FIXME: move into util
std::string format_prefix(const bgpstream_pfx_storage_t& pfx) {
    std::stringstream ss;

    ss << format_ip(pfx.address) << "/" << std::to_string(pfx.mask_len);

    return ss.str();
}

int main(int argc, char** argv) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);

    ProgramOptions opts(argc, argv);

    if (auto ret = opts.exit()) return *ret;

    RTR::Manager mtr;
    BGP::Stream stream;

    // Set from 10 minutes ago to live mode
    stream.add_interval(std::chrono::system_clock::now() - std::chrono::minutes(10));

    // Sun, 10 Oct 2010 10:10:10 GMT -  Sun, 10 Oct 2010 11:11:11 GMT
    // stream.add_interval(1286705410, 1286709071);

    if (auto project_filters = opts.project_filters())
        for (auto &p : *project_filters)
            stream.add_filter(BGP::Filter::Project, p);
    else
        stream.add_filter(BGP::Filter::Project, "ris"); // add standard filter

    if (auto collector_filters = opts.collector_filters())
        for (auto &c : *collector_filters)
            stream.add_filter(BGP::Filter::Collector, c);

    // Start the stream
    stream.start();

    // Start MTRlib
    // mtr.start();

    while (auto record = stream.next()) {
        auto r = *record;

        BOOST_LOG_TRIVIAL(debug)
            << "[record] "
            << "project = " << r.project() << ", "
            << "collector = " << r.collector() << ", "
            << "status = " << r.status() << ", "
            << "dump_pos = " << r.position() << ", "
            << "dump_time = " << r.dump_time() << ", "
            << "record_time = " << r.record_time();

        if (r.status() == BGP::Record::Status::Valid) {

            while (auto element = (*record).next()) {

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

                // Get element from optional<Element>
                auto e = *element;

                // Get timestamp of element
                auto tm = e.timestamp();

                // [<date time>] [AS xxx aaa.bbb.ccc.ddd]
                std::cout
                    << " [" << std::put_time(std::gmtime(&tm), "%F %T") << "] "
                    << "[AS" << e.peer_asnumber() << " "
                    << format_ip(e.peer_address()) << "] ";

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

                // Print announced/withdrawn prefix
                if (do_prefix) {
                    std::cout << "prefix: " << format_prefix(e.prefix()) << " ";
                }

                // Print announced AS path
                if (do_path) {
                    std::cout << "next hop: " << format_ip(e.next_hop()) << " via path:";

                    for (uint32_t asn : e.as_path()) {
                        std::cout << " " << asn;
                    }
                }

                std::cout << std::endl;
            }
        }
    }

    // mtr.stop();

    return 0;
}
