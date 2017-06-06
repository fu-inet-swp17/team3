#ifndef COLLECTOR_STREAM_HPP_INCLUDED
#define COLLECTOR_STREAM_HPP_INCLUDED

class CollectorStream {

private:
    std::string collector;
    BGP::Stream ribs, updates;

    bool ribs_completed = false;

public:
    CollectorStream(const std::string& c) : collector(c) {
        std::cout << "Create CollectorStream for '" << collector << "'" << std::endl;

        ribs.add_filter(BGP::Filter::RecordType, "ribs");
        ribs.add_filter(BGP::Filter::Collector, collector);

        updates.add_filter(BGP::Filter::RecordType, "updates");
        updates.add_filter(BGP::Filter::Collector, collector);
    }

    void start(unsigned long sp) {

        ribs.add_interval(sp);
        ribs.start();
    }

    bool next(BGP::Record& r) {

        bool ret;

        if (ribs_completed) {
            ret = updates.next(r);

            if (!ret) throw std::runtime_error("Update feed ended");

        } else {
            ret = ribs.next(r);

            if (r.position() == BGP::Record::Position::End) {
                std::cout << "Found end of RIB dump at record_time = " << r.record_time()
                          << ", dump_time = " << r.dump_time()
                          << std::endl;

                updates.add_interval(r.record_time());
                updates.start();
                ribs_completed = true;
            }

            if (!ret) throw std::runtime_error("Rib feed ended");
        }

        return ret;
    }
};

#endif
