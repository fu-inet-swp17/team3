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

int PipeReader::addDiagramm(std::string formatter, int row, int column, int countRow, int countColumn, bool reverseXY, DiagrammTyp typ, NCURSES_COLOR_T color, std::string nameXAchse, std::string nameYAchse, std::string nameDiagram) {
	this->formatters.push_back(formatter);
	this->row.push_back(row);
	this->countRow.push_back(countRow);
	this->column.push_back(column);
	this->countColumn.push_back(countColumn);
	this->typs.push_back(typ);
	this->reverseXY.push_back(reverseXY);
	this->diagrammData.push_back(new DiagrammData(typ));
	init_pair(colorcounter, color, COLOR_BLACK);
	this->colors.push_back(colorcounter++);
	this->xAchsisName.push_back(nameXAchse);
	this->yAchsisName.push_back(nameYAchse);
	this->diagrammName.push_back(nameDiagram);
	WINDOW *win;
	if(diagrams[nameDiagram] == 0){
		win = newwin(countRow, countColumn, row, column);
		diagrams[nameDiagram] = win;
	}else{
		win = diagrams[nameDiagram];
	}
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
		for (unsigned int i = 0; i < window.size(); i++) {
			ncurses->printDiagramm(window[i], yAchsisName[i], xAchsisName[i], diagrammName[i], *diagrammData[i], typs[i], colors[i]);
		}
		for (unsigned int i = 0; i < window.size(); i++) {
			wrefresh(window[i]);
		}
	}
}

void PipeReader::readInformationFromLine(int diagram, std::string line) {
	//TODO:: add switch block
	switch (typs[diagram]) {
	case BALKEN:
	case KURVE:
	case KURVEINTERPOLIERT:
		int x, y;
		if (reverseXY[diagram]) {
			sscanf(line.c_str(), formatters[diagram].c_str(), &x, &y);
		} else {
			sscanf(line.c_str(), formatters[diagram].c_str(), &y, &x);
		}
		diagrammData[diagram]->addTupel(x, y);
		break;
	case TOPLIST:
		char* regex1 = (char*) malloc(sizeof(char) * 255);
		char* regex2 = (char*) malloc(sizeof(char) * 255);
		sscanf(formatters[diagram].c_str(), "%[^~]~%s", regex1, regex2);
		char* tmp = (char*) malloc(sizeof(char) * 255);
		sscanf(line.c_str(), regex1, tmp);
		rekursiveTopListRead(std::string(regex2), std::string(tmp), diagram);
		free(regex1);
		free(regex2);
		free(tmp);
		break;
	}
}

int PipeReader::startListener() {
	boost::thread t(&PipeReader::run, this);
	return 0;
}

void PipeReader::rekursiveTopListRead(std::string formatter, std::string line, int diagram) {
	int asName = 0, invalid = 0;
	char* tmp = (char*) malloc(sizeof(char) * 255);
	sscanf(line.c_str(), formatter.c_str(), &asName, &invalid, tmp);
//	printf("asName: %i, invalid: %i", asName, invalid);
//	getchar();
	if (asName == invalid && asName == 0) {
		free(tmp);
		return;
	}
	diagrammData[diagram]->addASToList(asName, invalid);
	rekursiveTopListRead(formatter, std::string(tmp), diagram);
	free(tmp);
}
