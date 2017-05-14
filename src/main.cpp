#include <iostream>
#include <chrono>
#include <ctime>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "bgp/bgp.hpp"

#include "config.hpp"

int main(void) {


    boost::log::core::get()->set_filter(boost::log::trivial::severity
                                        // >= boost::log::trivial::info);
                                        >= boost::log::trivial::warning);
                                        //>= boost::log::trivial::trace);


    BGP::Stream stream;

    // Set from 1 minutes ago to live mode
    stream.add_interval(std::chrono::system_clock::now() - std::chrono::minutes(10));

    // Sun, 10 Oct 2010 10:10:10 GMT -  Sun, 10 Oct 2010 11:11:11 GMT
    // stream.add_interval(1286705410, 1286709071);

    stream.add_filter(BGP::Filter::Collector, "rrc01");
    stream.add_filter(BGP::Filter::Collector, "rrc02");
    stream.add_filter(BGP::Filter::Collector, "rrc03");
    stream.add_filter(BGP::Filter::Collector, "rrc04");
    stream.add_filter(BGP::Filter::Collector, "rrc05");
    stream.add_filter(BGP::Filter::Collector, "rrc06");
    stream.add_filter(BGP::Filter::Project, "ris");
    stream.add_filter(BGP::Filter::RecordType, "updates");

    // Start the stream
    stream.start();


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

                char elem_type;

                switch ((*element).type()) {
                        case BGP::Element::Type::RIB:
                            elem_type = 'R'; break;
                        case BGP::Element::Type::Announcement:
                            elem_type = 'A'; break;
                        case BGP::Element::Type::Withdrawal:
                            elem_type = 'W'; break;
                        case BGP::Element::Type::PeerState:
                            elem_type = 'S'; break;
                        default:
                            elem_type = '?';
                };

                std::cout
                    << ((*record).dump_type() ? "R" : "U") << "|"
                    << elem_type << "|"
                    << *record << "|"
                    << *element
                    << std::endl;
            }
        }
    }

    return 0;
}
