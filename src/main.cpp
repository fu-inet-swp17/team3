#include <iostream>
#include <chrono>
#include <ctime>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "bgp/bgp.hpp"

#include "util/program_options.hpp"

#include "config.hpp"

using util::ProgramOptions;
using BGP::Stream;

int main(int argc, char** argv) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);

    ProgramOptions opts(argc, argv);

    if (auto ret = opts.exit()) return *ret;

    Stream stream;

    // Set from 1 minutes ago to live mode
    stream.add_interval(std::chrono::system_clock::now() - std::chrono::minutes(10));

    // Sun, 10 Oct 2010 10:10:10 GMT -  Sun, 10 Oct 2010 11:11:11 GMT
    // stream.add_interval(1286705410, 1286709071);

    if (auto project_filters = opts.project_filters())
        for (auto &p : *project_filters)
            stream.add_filter(BGP::Filter::Project, p);

    if (auto collector_filters = opts.collector_filters())
        for (auto &c : *collector_filters)
            stream.add_filter(BGP::Filter::Collector, c);

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

                std::cout
                    << r.dump_type() << "|"
                    << (*element).type() << "|"
                    << r << "|"
                    << *element
                    << std::endl;
            }
        }
    }

    return 0;
}
