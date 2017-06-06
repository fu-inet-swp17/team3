/* compile and link: gcc <program file> -lbgpstream -lrtr */

#include <stdio.h>
#include <stdlib.h>
#include "bgpstream.h"
#include "rtrlib/rtrlib.h"

int main(int argc, const char **argv)
{
  /* Allocate memory for bgpstream */
  bgpstream_t *bs = bgpstream_create();
  bgpstream_record_t *record = bgpstream_record_create();
  bgpstream_elem_t *elem = NULL;
  bgpstream_as_path_seg_t *seg = NULL;
  int elem_counter = 0;
  int elem_invalid_counter = 0;
  int pfx_valid = 0;
  int pfx_invalid = 0;
  int pfx_notfound = 0;
  char buffer[2048];
  char rtr_buffer[INET_ADDRSTRLEN];

  /* Set metadata filters */
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "rrc00");
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "route-views2");
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_RECORD_TYPE, "updates");
  bgpstream_add_interval_filter(bs, (uint32_t)time(NULL)-600, BGPSTREAM_FOREVER);

  /* Create a TCP socket */
  struct tr_socket tr_tcp;
  /* For testing only */
  char tcp_host[] = "rpki-validator.realmv6.org";
  char tcp_port[] = "8282";

  struct tr_tcp_config tcp_config = {
    tcp_host,   //IP
    tcp_port,   //Port
    NULL        //Source address
  };
  tr_tcp_init(&tcp_config, &tr_tcp);

  struct rtr_socket rtr_tcp;
  rtr_tcp.tr_socket = &tr_tcp;

  /* Create a group of RTR servers with preference 1 */
  struct rtr_mgr_group groups[1];
  groups[0].sockets = malloc(sizeof(struct rtr_socket*));
  groups[0].sockets_len = 1;
  groups[0].sockets[0] = &rtr_tcp;
  groups[0].preference = 1;

  /* Initialize the RTR Connection Manager */
  struct rtr_mgr_config *conf;
  rtr_mgr_init(&conf, groups, 1, 30, 600, 600,
    NULL,   //pfx_update_fp
    NULL,   //spki_update_fp
    NULL,   //status_fp
    NULL
  );

  /* Start the RTR Connection Manager */
  rtr_mgr_start(conf);

  /* Wait for server sync */
  while(!rtr_mgr_conf_in_sync(conf)) {
    sleep(1);
  }
  struct lrtr_ip_addr pref;
  enum pfxv_state result;

  /* Start the BGP stream */
  bgpstream_start(bs);

  /* Read the stream of records */
  while (bgpstream_get_next_record(bs, record) > 0) {
    /* Ignore invalid records */
    if (record->status != BGPSTREAM_RECORD_STATUS_VALID_RECORD) {
      elem_invalid_counter++;
      continue;
    }
    /* Extract elems from the current record */
    while ((elem = bgpstream_record_get_next_elem(record)) != NULL) {
      /* Select only announcements */
      if (elem->type == BGPSTREAM_ELEM_TYPE_ANNOUNCEMENT) {
        /* Timestamp */
        //fprintf(stdout, "%d\n", elem->timestamp);
        /* Prefix */
        //bgpstream_pfx_snprintf(buffer, 1024, (bgpstream_pfx_t *)&(elem->prefix));
        /* Address */
        bgpstream_addr_ntop(buffer, 40, &(elem->prefix.address));
        //fprintf(stdout, "%s\n", buffer);
        lrtr_ip_str_to_addr(buffer, &pref);
        /* Mask length */
        //fprintf(stdout, "%d\n", elem->prefix.mask_len);
        /* Origin AS */
        seg = bgpstream_as_path_get_origin_seg(elem->aspath);
        bgpstream_as_path_seg_snprintf(buffer, 8, seg);
        //fprintf(stdout, "%s\n", buffer);
        /* Validate */
        rtr_mgr_validate(
          conf, atoi(buffer), &pref, elem->prefix.mask_len, &result
        );
        elem_counter++;
        lrtr_ip_addr_to_str(&pref, rtr_buffer, sizeof(rtr_buffer));

        /* Print: <timestamp of last elem>|<valid>|<invalid>|<not found> */
        switch(result) {
          case BGP_PFXV_STATE_VALID:
            printf("%d|%d|%d|%d\n",
              elem->timestamp, ++pfx_valid, pfx_invalid, pfx_notfound);
            break;
          case BGP_PFXV_STATE_INVALID:
            printf("%d|%d|%d|%d\n",
              elem->timestamp, pfx_valid, ++pfx_invalid, pfx_notfound);
            break;
          case BGP_PFXV_STATE_NOT_FOUND:
            printf("%d|%d|%d|%d\n",
              elem->timestamp, pfx_valid, pfx_invalid, ++pfx_notfound);
            break;
          default:
            break;
        }
      }
    }
  }

  /* Print the number of elems */
  printf("\tProcessed %d elems, %d invalid elems\n",
    elem_counter, elem_invalid_counter);

  /* Stop the RTR Connection Manager and release memory */
  rtr_mgr_stop(conf);
  rtr_mgr_free(conf);
  free(groups[0].sockets);

  /* De-allocate memory for bgpstream */
  bgpstream_destroy(bs);
  bgpstream_record_destroy(record);

  return 0;
}
