//============================================================================
// Name        : BGPTest.cpp
// Author      : Pyro
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "bgpFunktions/bgpFunctions.hpp"
#include <map>
#include "ncurses/ncursesFunctions.hpp"

using namespace std;

int main() {
	//cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	//bgpstream_add_interval_filter(bs, 1286705410, 1286709071);
	std::map<int, long> asMap;
	std::map<const char*, long> sources;
	std::map<int, long>* asMapP = &asMap;
	std::map<const char*, long>* sourcesP = &sources;

	(*sourcesP)["route-views2"] = 0;
	(*sourcesP)["route-views3"] = 0;
	(*sourcesP)["route-views4"] = 0;
	(*sourcesP)["route-views6"] = 0;
	(*sourcesP)["rrc00"] = 0;

	NcursesFunctions ncurses = NcursesFunctions();
	BGPFunctions bgpFunctions = BGPFunctions();

	ncurses.initialise(asMapP, sourcesP);
	bgpFunctions.initialise(asMapP, sourcesP);

	while (ncurses.printScreen()) {
		bgpFunctions.updateBGPData();
	}

	ncurses.close();

	/*
	 * bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "route-views3");
	 bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "route-views4");
	 bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "route-views6");
	 bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR, "route-views.eqix");
	 bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_COLLECTOR,
	 "route-views.jinx");
	 */

	return 0;
}
