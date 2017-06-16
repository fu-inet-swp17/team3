#include "stream_controller.hpp"

StreamController::collector_entry::collector_entry(const std::string& n) : name(n) {
    reset();
}

void StreamController::collector_entry::reset(void) {
    rib_time = 0;

    last_update_dump = 0;
    last_update_record = 0;

    ribs_processed = 0;
    updates_processed = 0;
    updates_ignored = 0;

    in_rib = false;
}

void StreamController::collector_entry::print() const {
    std::cout << "{ name = " << name
              << ", rib_time = " << rib_time
              << ", last_update_dump = " << last_update_dump
              << ", last_update_record = " << last_update_record
              << ", in_rib = " << (in_rib ? "true" : "false")
              << ", ribs_processed = " << ribs_processed
              << ", updates_processed = " << updates_processed
              << ", updates_ignored = " << updates_ignored
              << " }" << std::endl;
}

StreamController::StreamController(BGP::Stream&& s) : stream(std::move(s)) { /* */ }

void StreamController::start(void) {
    stream.start();
}

std::pair<StreamController::Instruction, unsigned> StreamController::next(BGP::Record& r) {
    if (stream.next(r))
        return handle(r);
    else
        throw std::runtime_error("End of Stream");
}

void StreamController::print(void) const {

    std::cout << "Collector Log: " << std::endl;

    int i = 0;
    for (const auto& e : log) {
        std::cout << "    " << i++ << ": "; e.print();
    }
}

StreamController::Instruction StreamController::update(const BGP::Record& r, collector_entry& e) {

    const auto t = r.dump_type();
    const auto p = r.position();

    if (t == BGP::Record::DumpType::RIB) {
        e.ribs_processed++;

        if (p == BGP::Record::Position::Start) {

            if ((e.rib_time == 0) || (e.rib_time < r.dump_time())) {
                e.reset();
                e.rib_time = r.dump_time();
                e.ribs_processed = 1;
                e.in_rib = true;

                BOOST_LOG_TRIVIAL(info) << "starting RIB import for collector '" << e.name << "'";

                return Instruction::Flush;
            }

            else {
                throw std::runtime_error("Duplicate start record");
            }
        }

        else if (p == BGP::Record::Position::Middle) {

            if ((e.rib_time == r.dump_time())) {
                return Instruction::Process;
            } else
                throw std::runtime_error("Unexpected middle RIB record");
        }

        else if (p == BGP::Record::Position::End) {

            if ((e.rib_time == r.dump_time())) {
                e.in_rib = false;

                BOOST_LOG_TRIVIAL(info) << "completing RIB import for collector '" << e.name << "'";

                return Instruction::Process;
            } else
                throw std::runtime_error("Unexpected End RIB record");
        }

        else
            throw std::runtime_error("Invalid RIB record position");
    }

    else if (t == BGP::Record::DumpType::Update) {

        // If no RIB has ever been received, ignore updates
        if (!e.rib_time) {
            e.updates_ignored++;
            return Instruction::Ignore;
        }

        // If we are currently reading RIB data, ignore updates
        else if (e.in_rib) {
            e.updates_ignored++;
            return Instruction::Ignore;
        }

        e.updates_processed++;

        if (p == BGP::Record::Position::End) {

            if (e.last_update_dump < r.dump_time())
                e.last_update_dump = r.dump_time();
            else
                throw std::runtime_error("Unexpected end update record");
        }

        e.last_update_record = r.record_time();

        return Instruction::Process;
    }

    else
        throw std::runtime_error("Invalid DumpType");
}

std::pair<StreamController::Instruction, unsigned> StreamController::handle(const BGP::Record& r) {

    const auto name = r.collector();

    // Find collector by name
    const auto m = std::find_if(log.begin(), log.end(),
                                [&] (const collector_entry& v) { return (v.name == name); });

    if (m != log.end()) {

        // Call update(...) to find out what to do with this Record
        return std::make_pair(update(r, *m), m - log.begin());

    } else {
        BOOST_LOG_TRIVIAL(info) << "new collector '" << name << "'";

        // Create new collector entry at the end of the log
        log.emplace_back(name);

        // Try again
        return handle(r);
    }
}
