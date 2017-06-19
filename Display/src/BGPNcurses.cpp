//============================================================================
// Name        : BGPTest.cpp
// Author      : Pyro
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "ncurses/ncursesFunctions.hpp"
#include <random>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>

//using namespace std;

volatile bool chosenOne = false;
volatile bool *closeNonse = &chosenOne;
std::ifstream outagesPipe, rtrPipe;
boost::mutex rtrPipeMutex, outagesPipeMutex;
std::multiset<const Tupel*, Tupel> rtrValids;
std::multiset<const Tupel*, Tupel> *rtrvalidTmp = &rtrValids;
std::multiset<const Tupel*, Tupel> rtrInvalid;
std::multiset<const Tupel*, Tupel> *rtrInvalidTmp = &rtrInvalid;
std::multiset<const Tupel*, Tupel> outages;
std::multiset<const Tupel*, Tupel> *outagesTmp = &outages;

void run() {
	NcursesFunctions ncurses = NcursesFunctions();
	ncurses.setBool(closeNonse);
	ncurses.read();
}

void rtrRun() {
	rtrPipe.open("rtrPipe", std::ios::in);
	if (outagesPipe.good() == false) {
		printf("Can't open pipe: rtrPipe");
		chosenOne = true;
		return;
	}
	std::string line, tmp;
	std::istringstream someStream;
	int timestamp = 0;
	int invalits = 0;
	int valits = 0;
	Tupel out;
	if (rtrPipe.is_open()) {
		while (getline(rtrPipe, line)) {
			someStream.str(line);
			if (getline(someStream, tmp, '|')) {
				timestamp = std::stoi(tmp);
			} else {
				break;
			}
			if (getline(someStream, tmp, '|')) {
				valits = std::stoi(tmp);
			} else {
				break;
			}
			if (getline(someStream, tmp, '|')) {
				invalits = std::stoi(tmp);
			} else {
				break;
			}
			someStream.str(std::string());
			someStream.clear();
			rtrPipeMutex.lock();
			rtrValids.insert(new Tupel(timestamp, valits));
			rtrInvalid.insert(new Tupel(timestamp, invalits));
			rtrPipeMutex.unlock();
		}
		rtrPipe.close();
	}
	chosenOne = true;
}

void outagesRun() {
	outagesPipe.open("outagesPipe", std::ios::in);
	if (outagesPipe.good() == false) {
		printf("Can't open pipe: outagesPipe");
		chosenOne = true;
		return;
	}
	std::string line, tmp;
	std::istringstream someStream;
	int timestamp = 0;
	int asNumber = 0;
	int prefixes = 0;
	Tupel out;
	if (outagesPipe.is_open()) {
		while (getline(outagesPipe, line)) {
			someStream.str(line);
			if (getline(someStream, tmp, '|')) {
				timestamp = std::stoi(tmp);
			} else {
				break;
			}
			if (getline(someStream, tmp, '|')) {
				asNumber = std::stoi(tmp);
			} else {
				break;
			}
			if (getline(someStream, tmp, '|')) {
				prefixes = std::stoi(tmp);
			} else {
				break;
			}
			someStream.str(std::string());
			someStream.clear();
			outagesPipeMutex.lock();
			outages.insert(new Tupel(timestamp, prefixes));
			outagesPipeMutex.unlock();
		}
		outagesPipe.close();
	}
	chosenOne = true;
}

int main() {
	(*closeNonse) = false;

	NcursesFunctions ncurses = NcursesFunctions();

	ncurses.initialise(closeNonse);

	std::string xAchse = std::string();
	xAchse += "Timestamp";
	std::string yAchse = std::string();
	yAchse += "Outages";
	std::string digname = std::string();
	digname += "Test2";
	std::string yAchse2 = std::string();
	yAchse2 += "rtrLib Missis";
	std::string digname2 = std::string();
	digname2 += "Test2";
	//Tupel* outage;

	rtrValids.insert(new Tupel(0, 0));
	rtrInvalid.insert(new Tupel(0, 0));
	outages.insert(new Tupel(0, 0));
	int counter = 0;
	int zeitMultiplikator = 1;

	nodelay(stdscr, TRUE);
	timeout(0);

	//getchar();

	boost::thread t { run };
	boost::thread rtrpipe { rtrRun };
	boost::thread outagepipe { outagesRun };

	//getchar();

	int y = 0;
	int x = 0;

	getmaxyx(stdscr, y, x);

	WINDOW *outagesWindow = newwin(y / 2, x / 2, 0, 0);
	WINDOW *rtrLibWindow = newwin(y / 2, x / 2, (y / 2) + 1, 0);

	while ((*closeNonse) != true) {
		init_pair(1, COLOR_BLUE, COLOR_BLACK);
		init_pair(2, COLOR_GREEN, COLOR_BLACK);
		init_pair(3, COLOR_RED, COLOR_BLACK);
		outagesPipeMutex.lock();
		ncurses.printDiagramm(outagesWindow, &yAchse2, &xAchse, &digname2, outagesTmp, BALKEN, 1);
		outagesPipeMutex.unlock();
		rtrPipeMutex.lock();
		ncurses.printDiagramm(rtrLibWindow, &yAchse, &xAchse, &digname, rtrvalidTmp, KURVE, 2);
		ncurses.printDiagramm(rtrLibWindow, &yAchse, &xAchse, &digname, rtrInvalidTmp, KURVE, 3);
		rtrPipeMutex.unlock();
		wrefresh(outagesWindow);
		wrefresh(rtrLibWindow);
		//refresh();
		boost::this_thread::sleep_for(boost::chrono::milliseconds { 200 });
		std::uniform_int_distribution<std::mt19937::result_type> dist62(3600 * (zeitMultiplikator - 1), 3600 * zeitMultiplikator);
		counter++;
		if ((counter % 10) == 0) {
			zeitMultiplikator++;
		}
		clear();
		wclear(outagesWindow);
		wclear(rtrLibWindow);
		getmaxyx(stdscr, y, x);
		wresize(outagesWindow, y / 2, x / 2);
		wresize(rtrLibWindow, y / 2, x / 2);
		mvwin(rtrLibWindow, (y / 2) + 1, 0);
	}

	ncurses.close();

	return 0;
}
