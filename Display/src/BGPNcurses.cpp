//============================================================================
// Name        : BGPTest.cpp
// Author      : Pyro
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <map>
#include "ncurses/ncursesFunctions.hpp"
#include <random>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>

//using namespace std;

volatile bool chosenOne = false;
volatile bool *closeNonse = &chosenOne;

void run() {
	NcursesFunctions ncurses = NcursesFunctions();
	ncurses.setBool(closeNonse);
	ncurses.read();
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

	std::multiset<const OutageData*, OutageData> kkk;
	std::multiset<const OutageData*, OutageData> *tmp = &kkk;
	std::multiset<const OutageData*, OutageData> kkk2;
	std::multiset<const OutageData*, OutageData> *tmp2 = &kkk2;

	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 10000);

	std::mt19937 rng2;
	rng2.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist62(0, 86400);

	//OutageData* outage;

	kkk.insert(new OutageData(dist62(rng2), dist6(rng)));
	kkk2.insert(new OutageData(dist62(rng2), dist6(rng)));
	int counter = 0;
	int zeitMultiplikator = 1;

	nodelay(stdscr, TRUE);
	timeout(0);

	//getchar();

	boost::thread t { run };

	//getchar();

	int y = 0;
	int x = 0;

	getmaxyx(stdscr, y, x);

	WINDOW *win = newwin(y / 2, x / 2, 0, 0);
	WINDOW *win2 = newwin(y / 2, x / 2, (y / 2) + 1, 0);

	while ((*closeNonse) != true) {
		kkk.insert(new OutageData(dist62(rng2), dist6(rng)));
		kkk2.insert(new OutageData(dist62(rng2), dist6(rng)));
		init_pair(1, COLOR_BLUE, COLOR_BLACK);
		init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
		ncurses.printDiagramm(win, &yAchse, &xAchse, &digname, tmp, BALKEN, 1);
		ncurses.printDiagramm(win2, &yAchse2, &xAchse, &digname2, tmp2, KURVE, 2);
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
