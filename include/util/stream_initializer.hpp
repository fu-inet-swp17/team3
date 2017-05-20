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

        // Set interface if given
        if (auto iface = opts.interface())
            stream.set_interface(*iface);
 
        // Print out description of the interface set
        BOOST_LOG_TRIVIAL(info) << "data interface: "
                                << stream.get_interface().first
                                << " (" << stream.get_interface().second << ")"
                                << std::endl;

        // Set interface options if any given
        for (auto& opt : opts.interface_options()) {
            try {
                stream.set_interface_option(opt.first, opt.second);

                BOOST_LOG_TRIVIAL(info) << "  option: " << opt.first
                                        << " = " << opt.second
                                        << std::endl;
                
            } catch (BGP::config_error& e) {
                BOOST_LOG_TRIVIAL(warning) << "unknown option '" << opt.first << "'";
            }
        }
        
        // Setup time window intervals
        for (auto& interval : opts.intervals()) {
            // 0 as end of interval means live mode
            if (interval.second == 0)
                stream.add_interval(interval.first);
            else
                stream.add_interval(interval.first, interval.second);
        }
        
        // Add project filters
        if (auto project_filters = opts.project_filters())
            for (auto &p : *project_filters)
                stream.add_filter(BGP::Filter::Project, p);

        // Add collector filters
        if (auto collector_filters = opts.collector_filters())
            for (auto &c : *collector_filters)
                stream.add_filter(BGP::Filter::Collector, c);
        
        return stream;        
    }
}

#endif
