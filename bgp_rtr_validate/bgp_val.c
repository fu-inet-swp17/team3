/* compile and link: gcc <program file> -lpthread -lbgpstream -lrtr */

/* For Debian/Ubuntu:                                      */
/* export LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu/ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "bgpstream.h"
#include "rtrlib/rtrlib.h"

#define IP4_BITS 32
#define IP4_MAX 1U << (IP4_BITS - 1)   // unsigned INT_MAX + 1

uint32_t last_elem_timestamp = 0;
uint32_t last_print_timestamp = 0;
int pfx_valid = 0;
int pfx_invalid = 0;
int pfx_notfound = 0;
bool new_data = false;

/* Array for AS with invalid updates */
typedef struct
{
  int *asn;
  int *invalid_updates;
  size_t used;
  size_t size;
} asn_array;
asn_array a;

void init_asn_array(asn_array *a, size_t initial_size)
{
  a->asn = (int *)malloc(initial_size * sizeof(int));
  a->invalid_updates = (int *)malloc(initial_size * sizeof(int));
  a->used = 1;
  a->size = initial_size + 1;
  a->asn[0] = 0;
  a->invalid_updates[0] = 0;
}

/* Return position of AS in array */
int array_pos_asn(asn_array *a, int asn)
{
  int i;
  for(i=0; i<a->used; i++) {
    if(a->asn[i] == asn) {
      return i;
    }
  }
  return -1;
}

/* Add new AS to array (if not already there) or increment its counter */ 
void add_asn(asn_array *a, int bad_asn)
{
  int pos = array_pos_asn(a, bad_asn);
  if(pos >= 0) {
    a->invalid_updates[pos]++;
  }
  else {
    if(a->used == a->size) {
      a->size *= 2;
      a->asn = (int *)realloc(a->asn, a->size * sizeof(int));
      a->invalid_updates = (int *)realloc(a->invalid_updates, a->size * sizeof(int));
    }
    a->asn[a->used] = bad_asn;
    a->invalid_updates[a->used++] = 1;
  }
}

/* Convert IPv4 address to host byte order */
in_addr_t ip4_to_hbo(char *cp)
{
  struct in_addr in;
  inet_aton(cp, &in);
  return(ntohl(in.s_addr));
}

/* Tree for IPv4 pfx */
typedef struct ip4_tree
{
  bool leaf;
  bool digit;
  int mask_len;
  int asn;
  struct ip4_tree *left;
  struct ip4_tree *right;
} ip4_tree;
ip4_tree *ip4_t;

void add_node(ip4_tree **node, bool digit, bool leaf)
{
  (*node) = malloc(sizeof(ip4_tree));
  (*node)->digit = digit;
  (*node)->left = 0;
  (*node)->right = 0;
}

/* Add IPv4 pfx */
void pfx_to_ip4_tree(ip4_tree **par, int asn, in_addr_t addr, int mask_len)
{
  int d = 0;
  if(!(*par)) {
    add_node(par, 0, true);
  }
  ip4_tree *pos = *par;
  int i;
  for(i=0; i<IP4_BITS; i++) {
    d = (IP4_MAX >> i) & addr;
    if(d) {
      if(!(pos->right)) {
        add_node(&(pos->right), 1, true);
      }
      pos->leaf = false;
      pos = pos->right;
    } else {
      if(!(pos->left)) {
        add_node(&(pos->left), 0, true);
      }
      pos->leaf = false;
      pos = pos->left;
    }
  }
  pos->asn = asn;
  pos->mask_len = mask_len;
}

/* Print: <valid>|<invalid>|<not found>|<AS #1>|<AS #1 updates>|<AS #2>|... */
int print_data(int chart[5][2])
{
  /*
  fprintf(stdout, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
    last_elem_timestamp,  // timestamp of last processed record
  */
  fprintf(stdout, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
    pfx_valid,            // no. of updates with valid pfx
    pfx_invalid,          // no. of updates with invalid pfx
    pfx_notfound,         // no. of updates with pfx not in RPKI
    chart[0][0],          // #1 AS with invalid updates
    chart[0][1],          // #1 AS invalid updates counter
    chart[1][0],          // #2 ...
    chart[1][1],          // #2 ...
    chart[2][0],          // #3 ...
    chart[2][1],          // #3 ...
    chart[3][0],          // #4 ...
    chart[3][1],          // #4 ...
    chart[4][0],          // #5 AS with invalid updates
    chart[4][1]           // #5 AS invalid updates counter
  );
  fflush(stdout);
  last_print_timestamp = (uint32_t)time(NULL);
  new_data = false;
  return 0;
}

void *print_thread()
{
  /* Top 5 AS with the most invalid updates */
  int top_asn[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
  int length = sizeof(top_asn)/sizeof(top_asn[0]);
  int i,j;
  int max = 0;
  for(;;) {
    sleep(5);
    if((last_elem_timestamp > 0) && (new_data)) {
      for(i=0; i<length; i++) {
        max = 0;
        for(j=0; j<a.used; j++) {
          if(i>0) {
            /* TODO Check if ASN is already in array to avoid duplicates */
            if((a.invalid_updates[j] > a.invalid_updates[max])
              && (a.invalid_updates[j] <= top_asn[i-1][1])
              && (a.asn[j] != top_asn[i-1][0])) {
              max = j;
            }
          }
          else {
            if(a.invalid_updates[j] > a.invalid_updates[max]) {
              max = j;
            }
          }
        }
        top_asn[i][0] = a.asn[max];
        top_asn[i][1] = a.invalid_updates[max];
        //printf("TOP: %d (%d)\n", top_asn[i][0], top_asn[i][1]);
      }
      print_data(top_asn);
    }
  }
  return 0;
}

int main(int argc, char **argv)
{
  /* Allocate memory for bgpstream */
  bgpstream_t *bs = bgpstream_create();
  bgpstream_record_t *record = bgpstream_record_create();
  bgpstream_data_interface_option_t *option;
  bgpstream_elem_t *elem = NULL;
  bgpstream_as_path_seg_t *seg = NULL;
  int elem_counter = 0;
  int elem_invalid_counter = 0;
  char ip_str[40];
  char buffer[80];
  char file_url[80];
  char rtr_buffer[INET_ADDRSTRLEN];
  
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
  
  /* Thread for printing */
  pthread_t tid;
  pthread_create(&tid, NULL, &print_thread, NULL);
  
  /* Init AS array */
  init_asn_array(&a, 5);
  
  /* Get current date and time */
  time_t now = time(NULL);
  time_t rib_file = now - now % 28800;
  struct tm *timestamp;
  char year_month[10];
  char ymd_time[20];
  timestamp = gmtime(&rib_file);
  strftime(year_month, 10, "%Y.%m", timestamp);
  
  /* Read the RIB file */
  bgpstream_set_data_interface(bs, BGPSTREAM_DATA_INTERFACE_SINGLEFILE);
  option = bgpstream_get_data_interface_option_by_name(
    bs, BGPSTREAM_DATA_INTERFACE_SINGLEFILE, "rib-file"
  );
  bgpstream_add_interval_filter(bs, (uint32_t)rib_file, (uint32_t)(rib_file+28800));
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_PROJECT, "ris");
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_PROJECT, "routeviews");
  int i;
  for(i=1; i<argc; i++) {
    bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, argv[i]);
  }
  //bgpstream_add_rib_period_filter(bs, 3600);
  bgpstream_add_rib_period_filter(bs, 28800);
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_RECORD_TYPE, "ribs");

  for(i=1; i<argc; i++) {
    timestamp = gmtime(&rib_file);
    strftime(ymd_time, 20, "%Y%m%d.%H%M", timestamp);
    strcpy(file_url, "http://data.ris.ripe.net/");
    strcat(file_url, argv[i]);
    strcat(file_url, "/");
    strcat(file_url, year_month);
    strcat(file_url, "/bview.");
    strcat(file_url, ymd_time);
    strcat(file_url, ".gz");
    bgpstream_set_data_interface_option(bs, option, file_url);
    bgpstream_start(bs);
    while(bgpstream_get_next_record(bs, record) > 0) {
      /* Ignore invalid records */
      if (record->status != BGPSTREAM_RECORD_STATUS_VALID_RECORD) {
        elem_invalid_counter++;
        continue;
      }
      /* Extract elems from the current record */
      while ((elem = bgpstream_record_get_next_elem(record)) != NULL) {
        /* Select only announcements */
        if (elem->type == BGPSTREAM_ELEM_TYPE_RIB) {
          /* Address */
          bgpstream_addr_ntop(ip_str, 40, &(elem->prefix.address));
          lrtr_ip_str_to_addr(ip_str, &pref);
          /* Origin AS */
          seg = bgpstream_as_path_get_origin_seg(elem->aspath);
          bgpstream_as_path_seg_snprintf(buffer, 80, seg);
          /* Validate */
          rtr_mgr_validate(
            conf, atoi(buffer), &pref, elem->prefix.mask_len, &result
          );
          /* Add to tree */
          if(elem->prefix.address.version == BGPSTREAM_ADDR_VERSION_IPV4) {
            pfx_to_ip4_tree(
              &ip4_t, atoi(buffer), ip4_to_hbo(ip_str), elem->prefix.mask_len
            );
          }
          elem_counter++;
          lrtr_ip_addr_to_str(&pref, rtr_buffer, sizeof(rtr_buffer));
          last_elem_timestamp = elem->timestamp;
          switch(result) {
            case BGP_PFXV_STATE_VALID:
              pfx_valid++;
              break;
            case BGP_PFXV_STATE_INVALID:
              pfx_invalid++;
              add_asn(&a, atoi(buffer));  // add AS to array
              break;
            case BGP_PFXV_STATE_NOT_FOUND:
              pfx_notfound++;
              break;
            default:
              break;
          }
          new_data = true;
        }
      }
    }
    bgpstream_stop(bs);
  }
  bgpstream_destroy(bs);
  bgpstream_record_destroy(record);
  
  /* Create new stream */
  bs = bgpstream_create();
  record = bgpstream_record_create();
  
  /* Switch to broker in live mode */
  bgpstream_set_data_interface(bs, BGPSTREAM_DATA_INTERFACE_BROKER);
  
  /* Set metadata filters */
  for(i=1; i<argc; i++) {
    bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, argv[i]);
  }
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "route-views2");
  bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_RECORD_TYPE, "updates");
  bgpstream_add_interval_filter(bs, (uint32_t)rib_file, BGPSTREAM_FOREVER);
  
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
        bgpstream_addr_ntop(ip_str, 40, &(elem->prefix.address));
        //fprintf(stdout, "%s\n", buffer);
        lrtr_ip_str_to_addr(ip_str, &pref);
        /* Mask length */
        //fprintf(stdout, "%d\n", elem->prefix.mask_len);
        /* Origin AS */
        seg = bgpstream_as_path_get_origin_seg(elem->aspath);
        bgpstream_as_path_seg_snprintf(buffer, 80, seg);
        //fprintf(stdout, "%s\n", buffer);
        /* Validate */
        rtr_mgr_validate(
          conf, atoi(buffer), &pref, elem->prefix.mask_len, &result
        );
        /* Add to tree */
        if(elem->prefix.address.version == BGPSTREAM_ADDR_VERSION_IPV4) {
          pfx_to_ip4_tree(
            &ip4_t, atoi(buffer), ip4_to_hbo(ip_str), elem->prefix.mask_len
          );
        }
        elem_counter++;
        lrtr_ip_addr_to_str(&pref, rtr_buffer, sizeof(rtr_buffer));
        last_elem_timestamp = elem->timestamp;
        switch(result) {
          case BGP_PFXV_STATE_VALID:
            pfx_valid++;
            break;
          case BGP_PFXV_STATE_INVALID:
            pfx_invalid++;
            add_asn(&a, atoi(buffer));  // add AS to array
            break;
          case BGP_PFXV_STATE_NOT_FOUND:
            pfx_notfound++;
            break;
          default:
            break;
        }
        new_data = true;
      }
    }
    //print_data();
  }
  
  /* The section below should be unreachable because of live mode */
  
  /* Print the number of elems */
  printf("\tProcessed %d elems, %d invalid elems\n",
    elem_counter, elem_invalid_counter);
  
  /* Stop the RTR Connection Manager and release memory */
  rtr_mgr_stop(conf);
  rtr_mgr_free(conf);
  free(groups[0].sockets);
  
  /* Stop bgpstream and de-allocate memory */
  bgpstream_stop(bs);
  bgpstream_destroy(bs);
  bgpstream_record_destroy(record);
  
  /* Release memory for AS array */
  free(a.asn);
  free(a.invalid_updates);
  a.asn = a.invalid_updates = NULL;
  a.used = a.size = 0;
  
  return 0;
}
