#ifndef RTR_HPP_INCLUDED
#define RTR_HPP_INCLUDED

#include <memory>

extern "C" {
#include "rtrlib/rtrlib.h"
}

static void status_fp(const struct rtr_mgr_group *group __attribute__((unused)),
                      enum rtr_mgr_status mgr_status,
                      const struct rtr_socket *rtr_sock,
                      void *data __attribute__((unused)))
{
    BOOST_LOG_TRIVIAL(info)
        << "RTR-Socket changed connection status to: "
        << rtr_state_to_str(rtr_sock->state)
        << "Mgr Status: "
        << rtr_mgr_status_to_str(mgr_status);
}

static void update_cb(struct pfx_table *p  __attribute__((unused)),
		      const struct pfx_record rec,
		      const bool added)
{
    char ip[INET6_ADDRSTRLEN];

    if (added)
        printf("+ ");
    else
        printf("- ");
    lrtr_ip_addr_to_str(&rec.prefix, ip, sizeof(ip));
    printf("%-40s   %3u - %3u   %10u\n",
           ip, rec.min_len, rec.max_len, rec.asn);
}

static void update_spki(struct spki_table *s __attribute__((unused)),
			const struct spki_record record,
			const bool added)
{
    char c;

    if (added)
        c = '+';
    else
        c = '-';

    printf("%c ", c);
    printf("ASN:  %u\n  ", record.asn);

    int i;
    int size = sizeof(record.ski);

    printf("SKI:  ");
    for (i = 0; i < size; i++) {
        printf("%02x", record.ski[i]);
        if (i < size - 1)
            printf(":");
    }
    printf("\n  ");

    i = 0;
    size = sizeof(record.spki);
    printf("SPKI: ");
    for (i = 0; i < size; i++) {
        if ((i % 40 == 0) && (i != 0))
            printf("\n	");

        printf("%02x", record.spki[i]);
        if (i < size - 1)
            printf(":");
    }
    printf("\n");
}

namespace RTR {

    class Manager {
    private:
        tr_tcp_config tcp_config { (char *) "rpki-validator.realmv6.org", (char *)"8282", 0 };
        tr_socket tr_sock;

        rtr_socket rtr;
        rtr_mgr_config *conf;
        rtr_mgr_group groups[1];

    public:
        Manager(void) {
            BOOST_LOG_TRIVIAL(trace) << "MTR::Manager::Manager()";
        
            tr_tcp_init(&tcp_config, &tr_sock);

            rtr.tr_socket = &tr_sock;

            groups[0].sockets_len = 1;
            groups[0].sockets = new rtr_socket*;
            groups[0].sockets[0] = &rtr;
            groups[0].preference = 1;

            int ret = rtr_mgr_init(&conf, groups, 1, 30, 600, 600,
                                   update_cb, update_spki, status_fp, NULL);

            if (ret == RTR_ERROR)
                BOOST_LOG_TRIVIAL(warning) << "Error in rtr_mgr_init";
            else if (ret == RTR_INVALID_PARAM)
                BOOST_LOG_TRIVIAL(warning) << "Invalid params passwd to rtr_mgr_init";

            if (!conf)
                BOOST_LOG_TRIVIAL(warning) << "conf == nullptr";
        }

        void start(void) {
            rtr_mgr_start(conf);
        }

        void stop(void) {
            rtr_mgr_stop(conf);
        }

        ~Manager(void) {
            BOOST_LOG_TRIVIAL(trace) << "MTR::Manager::~Manager()";

            rtr_mgr_stop(conf);
            rtr_mgr_free(conf);

            delete groups[0].sockets;
        }
    };
}

#endif
