#ifndef UTIL_PROGRAM_OPTIONS_HPP_INCLUDED
#define UTIL_PROGRAM_OPTIONS_HPP_INCLUDED

#include <experimental/optional>

#include "boost/program_options.hpp"

namespace util {

    namespace po = boost::program_options;
    using std::experimental::optional;
    using po::options_description;
    using po::variables_map;
    using po::value;
    using std::vector;
    using std::string;

    class ProgramOptions {
    private:
        options_description desc;
        variables_map arguments;
        optional<int> _exit;

        void print_help(int code) {
            std::cout << desc << "\n";
            _exit = optional<int>(code);
            return;
        }

    public:

        ProgramOptions(int argc, char** argv) : desc("Options") {

            desc.add_options()
                // Help text
                ("help,h", "This help text")
                // Project filters
                ("project,p", value<vector<string>>(),
                 "Filter records by project (e.g. routeviews, ris)*")
                // Collector filters
                ("collector,c", value<vector<string>>(),
                 "Filter records by collector (e.g. route-views2, rrc01)*");

            try {
                po::store(po::parse_command_line(argc, argv, desc), arguments);
                po::notify(arguments);

            } catch(po::error& e) {
                std::cerr << "error: " << e.what() << std::endl;
                print_help(2);
            }

            if (arguments.count("help")) print_help(0);
        }

        optional<vector<string>> project_filters(void) {
            if (arguments.count("project")) {
                return arguments["project"].as<vector<string>>();
            } else {
                return {};
            }
        }

        optional<vector<string>> collector_filters(void) {
            if (arguments.count("collector")) {
                return arguments["collector"].as<vector<string>>();
            } else {
                return {};
            }
        }

        optional<int> exit(void) {
            return _exit;
        }
    };
}

#endif
