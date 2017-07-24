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

void NcursesFunctions::initialise() {
	initscr(); /* Start curses mode */
	raw(); /* Line buffering disabled */
	keypad(stdscr, TRUE); /* We get F1, F2 etc.. */
	noecho(); /* Don't echo() while we do getch */
	start_color(); /* Start color */
}

void NcursesFunctions::printDiagramm(WINDOW *win, std::string yAchse, std::string xAchse, std::string diagrammName, DiagrammData data, DiagrammTyp typ, int color) {
	unsigned int x = 1;
	unsigned int y = 1;

	getmaxyx(win, y, x);
	x -= 3;
	y -= 3;

	int columnCount = x;

	unsigned int secsPerDay = 3600 * 24;

	if (typ != TOPLIST) {
		int *array = (int*) malloc(sizeof(int) * columnCount);
		data.constractColumArray(array, columnCount, secsPerDay);
		this->printDiagram2(win, yAchse, xAchse, diagrammName, array, columnCount, typ, color);
	} else {
		this->printTopList(win,diagrammName,data);
	}
}
int NcursesFunctions::printDiagram2(WINDOW *win, std::string yAchse, std::string xAchse, std::string diagrammName, int *array, int length, DiagrammTyp typ, int color) {
	curs_set(0);
	int x = 1;
	int y = 1;
	//TODO: window size

	getmaxyx(win, y, x);

	//Creating space for Diagram Naming
	//check if diagramm still fits
	if ((x - 5) < 1 || (y - 5) < 1) {
		return -1;
	}

	//Beschriefte Diagramm

	if (xAchse.size() > x) {
		xAchse = xAchse.data()[0] + '.';
	}
	if (yAchse.size() > y) {
		yAchse = yAchse.data()[0] + '.';
	}

	wmove(win, y - 1, (x / 2) - (xAchse.size() / 2));
	wprintw(win, xAchse.data());
	unsigned int startPositionYAchse = (y / 2) - (yAchse.size() / 2);
	unsigned int k = 0;
	for (k = 0; k < yAchse.size(); k++) {
		wmove(win, k + startPositionYAchse, 0);
		waddch(win, (unsigned long int) yAchse.data()[k]);
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

	wattron(win, COLOR_PAIR(color));
	unsigned int prefixCountPerRow = maxValueY / rowCount;
	if (prefixCountPerRow == 0) {
		prefixCountPerRow = 1;
	}

	unsigned int lastValue = 0;
	switch (typ) {
	case KURVE:
	case KURVEINTERPOLIERT:
		for (int i = 0; i < length; i++) {
			unsigned int rowAmount = array[i] / prefixCountPerRow;
			if (rowAmount > rowCount) {
				rowAmount = rowCount;
			}
			if (rowAmount > (lastValue + 1)) {
				rowAmount = lastValue + 1;
			} else if (lastValue != 0 && rowAmount < (lastValue - 1)) {
				rowAmount = lastValue - 1;
			}
			wmove(win, y - 2 - rowAmount, 2 + (i));
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
			lastValue = rowAmount;
		}
		break;
	case BALKEN:
		for (int i = 0; i < length; i++) {
			unsigned int rowAmount = array[i] / prefixCountPerRow;
			if (rowAmount > rowCount) {
				rowAmount = rowCount;
			}
			for (k = 0; k < rowAmount; k++) {
				wmove(win, y - 2 - k, 2 + (i));
				waddch(win, (unsigned int) '|');
			}
		}
		break;
	default:
		return -1;
	}
	wattroff(win, COLOR_PAIR(color));
	return 0;
}

int NcursesFunctions::printTopList(WINDOW* win, std::string name, DiagrammData data) {
	int xAchse = 0, yAchse = 0;
	getmaxyx(win, yAchse, xAchse);
	std::vector<int> asList;
	data.getASWithNumberInTopList(&asList);
	if (yAchse - 2 < 1) {
		return -1;
	}
	std::string diagramTitle;
	diagramTitle += "Top ";
	diagramTitle += std::to_string(asList.size());
	diagramTitle += " ASList";
	if (xAchse < diagramTitle.length()) {
		return -1;
	}
	wmove(win,0,0);
	wprintw(win,diagramTitle.c_str());
	int line = 3;
	for (std::vector<int>::iterator it = asList.begin(); it != asList.end(); it++, line++) {
		if (line < xAchse) {
			std::string entry;
			entry += std::to_string(*it);
			entry += " with ";
			entry += std::to_string(data.getValueForAS(*it));
			entry += " invalids.";
			wmove(win,line,0);
			wprintw(win,entry.c_str());
		}else{
			break;
		}
	}
	return 0;
}

void NcursesFunctions::close() {
}

NcursesFunctions::NcursesFunctions(void) {
}

NcursesFunctions::~NcursesFunctions(void) {
}
