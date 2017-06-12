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

void NcursesFunctions::initialise(volatile bool *close) {
	initscr(); /* Start curses mode */
	raw(); /* Line buffering disabled */
	keypad(stdscr, TRUE); /* We get F1, F2 etc.. */
	noecho(); /* Don't echo() while we do getch */
	start_color(); /* Start color */
}

void NcursesFunctions::setBool(volatile bool *close){
	this->closeB = close;
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
	printw("Anzahl AS %i \n", (*asMap).size());
	move(1, 20);
	printw("Ueberwachte AS\n");
	move(2, 0);
	std::map<const char*, long>::iterator iter;
	for (iter = (*sources).begin(); iter != (*sources).end(); iter++) {
		printw("Source: %s, naechtes Update in %i  Minute(n) \n", iter->first, (600 - ((seconds - (*sources)[iter->first]))) / 60);
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

/*void NcursesFunctions::printDiagramm(WINDOW *win, std::string *yAchse, std::string *xAchse, std::string *diagrammName, std::multiset<const OutageData*, OutageData> *data) {
 curs_set(0);
 unsigned int x = 1;
 unsigned int y = 1;
 //TODO: window size

 getmaxyx(win, y, x);

 //Creating space for Diagram Naming
 //check if diagramm still fits
 if ((x - 5) < 1 || (y - 5) < 1) {
 return;
 }

 //Beschriefte Diagramm

 if ((*xAchse).size() > x) {
 (*xAchse) = (*xAchse).data()[0] + '.';
 }
 if ((*yAchse).size() > y) {
 (*yAchse) = (*yAchse).data()[0] + '.';
 }

 wmove(win, y - 1, (x / 2) - ((*xAchse).size() / 2));
 wprintw(win, (*xAchse).data());
 unsigned int startPositionYAchse = (y / 2) - ((*yAchse).size() / 2);
 unsigned int k = 0;
 for (k = 0; k < (*yAchse).size(); k++) {
 wmove(win, k + startPositionYAchse, 0);
 waddch(win, (unsigned long int) (*yAchse).data()[k]);
 }

 //Zeichne Grundgerüst
 //Zeichne Grundgerüst yAchse
 for (k = 0; k < (y - 2); k++) {
 wmove(win, k, 1);
 waddch(win, (unsigned long int) '|');
 }

 unsigned int rowCount = k - 1;

 wmove(win, k, 1);
 waddch(win, (unsigned long int) '+');
 for (k = 0; k < x - 2; k++) {
 wmove(win, y - 2, 2 + k);
 waddch(win, (unsigned long int) '-');
 }
 unsigned int columnCount = k - 1;

 //Ermittle Min-Max Werte

 unsigned int size = data->size();

 wmove(win, 0, 0);
 wprintw(win, "Size: %i", size);

 std::multiset<const OutageData*, OutageData>::iterator iter = (*data).begin();
 int startTimestamp = (*iter)->getTimestamp();

 iter = (*data).end();
 iter--;

 int endTimestamp = (*iter)->getTimestamp();

 unsigned int timeIntervallOverall = endTimestamp - startTimestamp;

 unsigned int timeIntervallPerCaloumn = timeIntervallOverall / columnCount;
clear
 unsigned int position;

 unsigned int maxValue = 0;

 for (iter = (*data).begin(); iter != (*data).end(); iter++) {
 if (maxValue < (*iter)->getCountPrefixes()) {
 maxValue = (*iter)->getCountPrefixes();
 }
 }

 wmove(win, y - 1, 0);
 wprintw(win, "MaxTimeStamp: %i", endTimestamp);

 unsigned int prefixCountPerRow = maxValue / rowCount;
 if (prefixCountPerRow == 0) {
 prefixCountPerRow = 1;
 }

 for (iter = (*data).begin(); iter != (*data).end(); iter++) {
 position = ((*iter)->getTimestamp() - startTimestamp) / timeIntervallPerCaloumn;
 wmove(win, 6, 5);
 if (position > columnCount) {
 position = columnCount;
 }
 unsigned int rowAmount = (*iter)->getCountPrefixes() / prefixCountPerRow;
 if (rowAmount > rowCount) {
 rowAmount = rowCount;
 }
 for (k = 0; k < rowAmount; k++) {
 wmove(win, y - 3 - k, 2 + (position));
 waddch(win, (unsigned int) '|');
 }
 }

 //Zeichne Balken
 }*/

void NcursesFunctions::printDiagramm(WINDOW *win, std::string *yAchse, std::string *xAchse, std::string *diagrammName, std::multiset<const OutageData*, OutageData> *data, DiagrammTyp typ, int color) {
	unsigned int x = 1;
	unsigned int y = 1;
	//TODO: window size

	getmaxyx(win, y, x);
	x -= 3;
	y -= 3;

	int columnCount = x;

	std::multiset<const OutageData*, OutageData>::iterator iter = (*data).end();
	iter--;
	unsigned int secsPerDay = 3600 * 24;
	//TODO: Aktuellen Timespamt verwenden
	unsigned int timeStampsPerColumn = secsPerDay / columnCount;
	unsigned int lastTimestamp = (*iter)->getTimestamp();

	int *array = (int*) malloc(sizeof(int) * columnCount);

	int arrayPosition = 0;

	for(int i = 0; i< columnCount; i++){
		array[i] = 0;
	}

	for (iter = (*data).begin(); iter != (*data).end(); iter++) {
		if ((lastTimestamp < secsPerDay) && ((*iter)->getTimestamp() < secsPerDay)) {
			arrayPosition = (*iter)->getTimestamp() / timeStampsPerColumn;
			if (arrayPosition > (columnCount - 1)) {
				arrayPosition = (columnCount - 1);
			}
			array[arrayPosition] += (*iter)->getCountPrefixes();
		} else if (((*iter)->getTimestamp() > (lastTimestamp - secsPerDay))) {
			arrayPosition = ((*iter)->getTimestamp() - (lastTimestamp - secsPerDay)) / timeStampsPerColumn;
			if (arrayPosition > (columnCount - 1)) {
				arrayPosition = (columnCount - 1);
			}
			array[arrayPosition] += (*iter)->getCountPrefixes();
		}
	}
	this->printDiagram2(win, yAchse, xAchse, diagrammName, array, columnCount, typ, color);
}

int NcursesFunctions::printDiagram2(WINDOW *win, std::string *yAchse, std::string *xAchse, std::string *diagrammName, int *array, int length, DiagrammTyp typ, int color) {
	curs_set(0);
	unsigned int x = 1;
	unsigned int y = 1;
	//TODO: window size

	getmaxyx(win, y, x);

	//Creating space for Diagram Naming
	//check if diagramm still fits
	if ((x - 5) < 1 || (y - 5) < 1) {
		return -1;
	}

	//Beschriefte Diagramm

	if ((*xAchse).size() > x) {
		(*xAchse) = (*xAchse).data()[0] + '.';
	}
	if ((*yAchse).size() > y) {
		(*yAchse) = (*yAchse).data()[0] + '.';
	}

	wmove(win, y - 1, (x / 2) - ((*xAchse).size() / 2));
	wprintw(win, (*xAchse).data());
	unsigned int startPositionYAchse = (y / 2) - ((*yAchse).size() / 2);
	unsigned int k = 0;
	for (k = 0; k < (*yAchse).size(); k++) {
		wmove(win, k + startPositionYAchse, 0);
		waddch(win, (unsigned long int) (*yAchse).data()[k]);
	}

	//Zeichne Grundgerüst
	//Zeichne Grundgerüst yAchse
	for (k = 0; k < (y - 2); k++) {
		wmove(win, k, 1);
		waddch(win, (unsigned long int) '|');
	}

	unsigned int rowCount = k - 1;

	wmove(win, k, 1);
	waddch(win, (unsigned long int) '+');
	for (k = 0; k < x - 2; k++) {
		wmove(win, y - 2, 2 + k);
		waddch(win, (unsigned long int) '-');
	}
	unsigned int columnCount = k - 1;

	int maxValueY = 0;

	for (int i = 0; i < length; i++) {
		if (array[i] > maxValueY)
			maxValueY = array[i];
	}


	wattron(win,COLOR_PAIR(color));
	unsigned int prefixCountPerRow = maxValueY / rowCount;
	if (prefixCountPerRow == 0) {
		prefixCountPerRow = 1;
	}
	switch (typ) {
	case KURVE:
	case KURVEINTERPOLIERT:
		for (int i = 0; i < length; i++) {
			unsigned int rowAmount = array[i] / prefixCountPerRow;
			if (rowAmount > rowCount) {
				rowAmount = rowCount;
			}
			wmove(win, y - 3 - rowAmount, 2 + (i));
			if ((i + 1) < length) {
				unsigned int rowAmountI1 = array[i + 1] / prefixCountPerRow;
				if (rowAmountI1 > rowCount) {
					rowAmountI1 = rowCount;
				}
				if (rowAmount < rowAmountI1) {
					waddch(win, (unsigned int) '/');
				} else if (rowAmount == rowAmountI1) {
					waddch(win, (unsigned int) '-');
				} else {
					waddch(win, (unsigned int) '\\');
				}
			} else {
				waddch(win, (unsigned int) '-');
			}

		}
		break;
	case BALKEN:
		for (int i = 0; i < length; i++) {
			unsigned int rowAmount = array[i] / prefixCountPerRow;
			if (rowAmount > rowCount) {
				rowAmount = rowCount;
			}
			for (k = 0; k < rowAmount; k++) {
				wmove(win, y - 3 - k, 2 + (i));
				waddch(win, (unsigned int) '|');
			}
		}
		break;
	default:
		return -1;
	}
	wattroff(win,COLOR_PAIR(color));
	return 0;
}

void NcursesFunctions::read() {
	char blub = 0;
	blub = getchar();
	if (blub == 'q') {
		(*closeB) = true;
	}
}

NcursesFunctions::NcursesFunctions() {

}
NcursesFunctions::~NcursesFunctions() {

}

