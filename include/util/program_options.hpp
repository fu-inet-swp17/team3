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
    using std::pair;

    typedef pair<unsigned long, unsigned long> interval;

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
                 "Filter records by collector (e.g. route-views2, rrc01)*")
                // Data interface select
                ("interface,d", value<string>(),
                 "Select data interface")
                // Time interval
                ("window,w", value<vector<string>>(),
                 "Process records within the given time window")
                // Data interface options
                ("option,o", value<vector<string>>(),
                 "Set option for selected data interface");

            try {
                po::store(po::parse_command_line(argc, argv, desc), arguments);
                po::notify(arguments);

            } catch(po::error& e) {
                std::cerr << "error: " << e.what() << std::endl;
                print_help(2);
            }

            if (arguments.count("help")) print_help(0);

            // Do not accept interface options if no interface has been manually selected.
            if (arguments.count("option") && !arguments.count("interface")) {
                std::cerr << "error: select interface when setting interface options." << std::endl;
                print_help(2);
            }
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

        optional<string> interface(void) {
            if (arguments.count("interface")) {
                return arguments["interface"].as<string>();
            } else {
                return {};
            }
        }

        vector<pair<string, string>> interface_options(void) {
            auto ret = vector<pair<string, string>>();

            // If any 'option' parameters present
            if (arguments.count("option")) {
                auto options = arguments["option"].as<vector<string>>();

                // For each option string "option-name,option-value"
                for (auto &s : options) {

                    // Find separator ','
                    auto comma = s.find(',', 0);

                    if(comma != string::npos) {
                        ret.push_back(std::make_pair<string, string>(s.substr(0, comma),
                                                                     s.substr(comma+1, string::npos)));
                        continue;
                    }

                    // Find alternative separator '='
                    auto equal = s.find('=', 0);

                    if(equal != string::npos) {
                        ret.push_back(std::make_pair<string, string>(s.substr(0, equal),
                                                                     s.substr(equal+1, string::npos)));
                        continue;
                    }

                    // Boolean option, empty value
                    ret.push_back(std::pair<string, string>(s, string()));
                }
            }

            return ret;
        }

        // FIXME: Handle stoul exceptions. This should also happen at option parse time
        vector<interval> intervals(void) {
            auto ret = vector<interval>();

            // If any 'window' parameters present
            if (arguments.count("window")) {
                auto options = arguments["window"].as<vector<string>>();

                // For each interval string "<start>[,<end>]"
                for (auto &s : options) {
                    // Find separator ','
                    auto comma = s.find(',', 0);

                    if(comma != string::npos) {
                        auto start = std::stoul(s.substr(0, comma));
                        auto end   = std::stoul(s.substr(comma+1, string::npos));

                        ret.push_back(interval(start, end));

                        continue;
                    }

                    // No end given
                    ret.push_back(interval(std::stoul(s), 0));
                }
            }

            return ret;
        }

        optional<int> exit(void) {
            return _exit;
        }
    };
}

#endif
