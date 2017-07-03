/*
 * PipeReader.cpp
 *
 *  Created on: 02.07.2017
 *      Author: pyro
 */

#include "PipeReader.hpp"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <ncurses.h>
#include <cstdio>
#include "ASObject.hpp"
#include "../ncurses/ncursesFunctions.hpp"
#include "../constants/Enums.hpp"

PipeReader::PipeReader(std::string pipeName, bool ignoreEOF) {
	this->pipeName = pipeName;
	this->ignoreEOF = ignoreEOF;
	pipe.open(pipeName, std::ios::in);
	if (pipe.good() == false) {
		printf("Can't open pipe: %s", pipeName.c_str());
		exit(-1);
	}
}

PipeReader::~PipeReader() {

}

int PipeReader::addDiagramm(std::string formatter, int row, int column, int countRow, int countColumn, bool reverseXY, DiagrammTyp typ) {
	this->formatters.push_back(formatter);
	this->row.push_back(row);
	this->countRow.push_back(countRow);
	this->column.push_back(column);
	this->countColumn.push_back(countColumn);
	this->typs.push_back(typ);
	this->reverseXY.push_back(reverseXY);
	this->diagrammData.push_back(new DiagrammData(typ));
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	this->colors.push_back(1);
	WINDOW *win = newwin(countRow, countColumn, row, column);
	this->window.push_back(win);
	return formatters.size();
}

void PipeReader::run() {
	//initialize
	NcursesFunctions* ncurses = new NcursesFunctions();
	//read next line from pipe
	std::string line;
	while (getline(pipe, line)) {
		//get information from line
		for (unsigned int i = 0; i < diagrammData.size(); i++) {
			readInformationFromLine(i, line);
		}
		//clear windows
		for (unsigned int i = 0; i < window.size(); i++) {
			wclear(window[i]);
		}
		//TODO: paint diagramm(s)
//		printf("printDiagramm");
//		getchar();
		for (unsigned int i = 0; i < window.size(); i++) {
			//ncurses->printDiagramm(window[i], yAchsisName[i], xAchsisName[i], diagrammName[i], *diagrammData[i], typs[i], colors[i]);
			ncurses->printDiagramm(window[i], "test", "test", "blub", *diagrammData[i], typs[i], colors[i]);
		}
		for (unsigned int i = 0; i < window.size(); i++) {
			wrefresh(window[i]);
		}
	}
}

void PipeReader::readInformationFromLine(int diagram, std::string line) {
	//TODO:: add switch block
	int x, y;
	if (reverseXY[diagram]) {
		sscanf(line.c_str(), formatters[diagram].c_str(), &x, &y);
	} else {
		sscanf(line.c_str(), formatters[diagram].c_str(), &y, &x);
	}
	diagrammData[diagram]->addTupel(x, y);
}

int PipeReader::startListener() {
	NcursesFunctions::initialise();
	boost::thread t(&PipeReader::run, this);
	return 0;
}
