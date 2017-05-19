#ifndef STREAM_INITIALIZER_HPP_INCLUDED
#define STREAM_INITIALIZER_HPP_INCLUDED

#include <experimental/optional>

#include "util/program_options.hpp"
#include "bgp/stream.hpp"

namespace util {

    // Create a BGP::Stream for quick testing
    BGP::Stream example_live_stream(void) {
        BGP::Stream stream;
        
        // Set from 10 minutes ago to live mode with 5 active ris collectors
        stream.add_interval(std::chrono::system_clock::now() - std::chrono::minutes(10));
        stream.add_filter(BGP::Filter::Project, "ris");
        stream.add_filter(BGP::Filter::Collector, "rrc00");
        stream.add_filter(BGP::Filter::Collector, "rrc01");
        stream.add_filter(BGP::Filter::Collector, "rrc03");
        stream.add_filter(BGP::Filter::Collector, "rrc04");
        stream.add_filter(BGP::Filter::Collector, "rrc10");

        return stream;
    }

    // Create a BGP::Stream instance from program options
    BGP::Stream stream_from_options(ProgramOptions& opts) {
        BGP::Stream stream;

        // FIXME: not implemented yet
        if (auto interface = opts.interface()) {
            std::cout << "Interface: " << *interface << std::endl;

            for (auto& opt_value : opts.interface_options())
                std::cout << "Interface option '"
                          << opt_value.first << "' = '"
                          << opt_value.second << "'" << std::endl;
        }

        // FIXME: not implemented yet
        for (auto& interval : opts.intervals())
            std::cout << "Start: " << interval.first << ", End: " << interval.second << std::endl;
        
        // Add project filters
        if (auto project_filters = opts.project_filters())
            for (auto &p : *project_filters)
                stream.add_filter(BGP::Filter::Project, p);

        return stream;        
    }
}

#endif
