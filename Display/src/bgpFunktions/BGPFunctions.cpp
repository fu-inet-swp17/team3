/*
 * BGPFunctions.cpp
 *
 *  Created on: 25.05.2017
 *      Author: pyro
 */

#include "bgpFunctions.hpp"
#include <iostream>
#include <string>
#include <ctime>
#include <map>
extern "C" {
#include "bgpstream.h"
#include "bgpdump/bgpdump_util.h"
}

void BGPFunctions::initialise(std::map<int, long> *asMap, std::map<const char*, long> *sources) {
	this->asMap = asMap;
	this->sources = sources;
}

void BGPFunctions::updateBGPData() {
	time_t seconds;
	seconds = time(NULL);
	std::map<const char*, long>::iterator iter;

	std::cout << "Updating BGPData" << std::endl;

	int count = 0;

	//Hohle Updates der Letzten 10 Minuten
	for (iter = (*sources).begin(); iter != (*sources).end(); iter++) {
		//iter->first
		if ((*sources)[iter->first] == 0) {
			(*sources)[iter->first] = seconds - 650;
		}
		if ((*sources)[iter->first] < (seconds - (60 * 10))) {
			count = bgpUpdates((*sources)[iter->first], ((*sources)[iter->first] + 600), iter->first);
			(*sources)[iter->first] = ((*sources)[iter->first] + 600);
		}
	}

}

int BGPFunctions::bgpUpdates(long from, long to, const char *peerName) {
	bgpstream_t *bs = bs = bgpstream_create();
	bgpdump_log_to_syslog();

	if (!bs) {
		fprintf(stderr, "ERROR: Could not create BGPStream instance\n");
		return -1;
	}

	/* Allocate memory for a re-usable bgprecord instance */
	bgpstream_record_t *bs_record = bgpstream_record_create();
	if (bs_record == NULL) {
		fprintf(stderr, "ERROR: Could not create BGPStream record\n");
		return -1;
	}

	/* The broker interface is set by default */

	/* Select bgp data from RRC06 and route-views.jinx collectors only */
	bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, peerName);

	/* Process updates only */
	bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_RECORD_TYPE, "updates");

	/* Select a time interval to process:
	 * Sun, 10 Oct 2010 10:10:10 GMT -  Sun, 10 Oct 2010 11:11:11 GMT */
	bgpstream_add_interval_filter(bs, from, to);

	/* Start bgpstream */
	if (bgpstream_start(bs) < 0) {
		fprintf(stderr, "ERROR: Could not init BGPStream\n");
		return -1;
	}

	int get_next_ret = 0;
	int elem_counter = 0;

	/* Pointer to a bgpstream elem, memory is borrowed from bgpstream,
	 * use the elem_copy function to own the memory */
	bgpstream_elem_t *bs_elem = NULL;

	/* Read the stream of records */
	do {
		/* Get next record */
		get_next_ret = bgpstream_get_next_record(bs, bs_record);
		/* Check if there are new records and if they are valid records */
		if (get_next_ret && bs_record->status == BGPSTREAM_RECORD_STATUS_VALID_RECORD) {
			/* Get next elem in the record */
			while ((bs_elem = bgpstream_record_get_next_elem(bs_record)) != NULL) {
				//std::cout << bs_elem->peer_asnumber << std::endl;
				elem_counter++;
				(*asMap)[bs_elem->peer_asnumber] = (long) elem_counter;
			}
		}
	} while (get_next_ret > 0);

	/* De-allocate memory for the bgpstream record */
	bgpstream_record_destroy(bs_record);

	/* De-allocate memory for the bgpstream */
	bgpstream_destroy(bs);

	return elem_counter;
}

BGPFunctions::BGPFunctions(void) {

}
;
BGPFunctions::~BGPFunctions(void) {

}

