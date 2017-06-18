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
std::multiset<const OutageData*, OutageData> kkk;
std::multiset<const OutageData*, OutageData> *tmp = &kkk;
std::multiset<const OutageData*, OutageData> kkk2;
std::multiset<const OutageData*, OutageData> *tmp2 = &kkk2;

void run() {
	NcursesFunctions ncurses = NcursesFunctions();
	ncurses.setBool(closeNonse);
	ncurses.read();
}

void rtrRun() {
	rtrPipe.open("rtrPipe", std::ios::in);
	if (outagesPipe.good() == false) {
		printf("Can't open pipe: rtrPipe");
		std::exit(-1);
	}
	std::string line, tmp;
	std::istringstream someStream;
	int timestamp = 0;
	int asNumber = 0;
	int prefixes = 0;
	OutageData out;
	if (rtrPipe.is_open()) {
		while (getline(rtrPipe, line)) {
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
			rtrPipeMutex.lock();
			kkk.insert(new OutageData(timestamp, prefixes));
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
		std::exit(-1);
	}
	std::string line, tmp;
	std::istringstream someStream;
	int timestamp = 0;
	int asNumber = 0;
	int prefixes = 0;
	OutageData out;
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
			kkk2.insert(new OutageData(timestamp, prefixes));
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
	//OutageData* outage;

	kkk.insert(new OutageData(0, 0));
	kkk2.insert(new OutageData(0, 0));
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

	WINDOW *win = newwin(y / 2, x / 2, 0, 0);
	WINDOW *win2 = newwin(y / 2, x / 2, (y / 2) + 1, 0);

	while ((*closeNonse) != true) {
		init_pair(1, COLOR_BLUE, COLOR_BLACK);
		init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
		rtrPipeMutex.lock();
		ncurses.printDiagramm(win, &yAchse, &xAchse, &digname, tmp, BALKEN, 1);
		rtrPipeMutex.unlock();
		outagesPipeMutex.lock();
		ncurses.printDiagramm(win2, &yAchse2, &xAchse, &digname2, tmp2, KURVE, 2);
		outagesPipeMutex.unlock();
		wrefresh(win);
		wrefresh(win2);
		//refresh();
		boost::this_thread::sleep_for(boost::chrono::milliseconds { 200 });
		std::uniform_int_distribution<std::mt19937::result_type> dist62(3600 * (zeitMultiplikator - 1), 3600 * zeitMultiplikator);
		counter++;
		if ((counter % 10) == 0) {
			zeitMultiplikator++;
		}
		clear();
		wclear(win);
		wclear(win2);
		getmaxyx(stdscr, y, x);
		wresize(win, y / 2, x / 2);
		wresize(win2, y / 2, x / 2);
		mvwin(win2, (y / 2) + 1, 0);
	}

	ncurses.close();

	return 0;
}
