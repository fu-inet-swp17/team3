/*
 * ncursesFunctions.cpp
 *
 *  Created on: 27.05.2017
 *      Author: pyro
 */

#include "ncursesFunctions.hpp"
#include <ncurses.h>
#include <iostream>
#include <ctime>

#include "../bgpFunktions/bgpFunctions.hpp"

int NcursesFunctions::printScreen() {
	switch (screenState) {
	case NcursesFunctions::MainWindow:
		return printMainScreen();
	case NcursesFunctions::OptionsMenu:
		return printOptionsMenu();
	case NcursesFunctions::SourceMenu:
		return printSourceMenu();
	default:
		screenState = 0;
		return 0;
	}

}

void NcursesFunctions::initialise(std::map<int, long> *asMap, std::map<const char*, long> *sources) {
	initscr(); /* Start curses mode */
	raw(); /* Line buffering disabled */
	keypad(stdscr, TRUE); /* We get F1, F2 etc.. */
	noecho(); /* Don't echo() while we do getch */
	this->asMap = asMap;
	this->sources = sources;
}

int NcursesFunctions::printMainScreen(void) {
	move(0, 0);
	printw("Main Menu Test\n");
	refresh();
	return switchScreen();
}
void NcursesFunctions::close(void) {
	endwin(); /* End curses mode */
}
int NcursesFunctions::printOptionsMenu(void) {
	return switchScreen();
}
int NcursesFunctions::printSourceMenu(void) {

	time_t seconds;
	seconds = time(NULL);

	move(0, 0);
	printw("Source Menu Test\n");
	printw("Anzahl AS %i\n", (*asMap).size());
	move(1, 20);
	printw("Ueberwachte AS\n");
	move(2, 0);
	std::map<const char*, long>::iterator iter;
	for (iter = (*sources).begin(); iter != (*sources).end(); iter++) {
		printw("Source: %s, naechtes Update in %i Minute(n) \n", iter->first, (600 - ((seconds - (*sources)[iter->first]))) / 60 );
	}
	refresh();
	return switchScreen();
}
int NcursesFunctions::switchScreen(void) {
	move(20, 0);
	printw("Main Menu: M, Options Menu: O, Source Menu: S, Quit: Q\n");
	refresh();
	move(0, 0);
	char ch = ' ';
	ch = getch();
	clear();

	switch (ch) {
	case 'm':
		screenState = NcursesFunctions::MainWindow;
		printw("Main Menu Test\n");
		break;
	case 'o':
		screenState = NcursesFunctions::OptionsMenu;
		printw("Options Menu Test\n");
		break;
	case 's':
		screenState = NcursesFunctions::SourceMenu;
		printw("Source Menu Test\n");
		break;
	case 'q':
		return 0;
	default:
		break;
	}
	return 1;
}
NcursesFunctions::NcursesFunctions() {

}
NcursesFunctions::~NcursesFunctions() {

}

