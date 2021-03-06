/*
 * PipeReader.hpp
 *
 *  Created on: 02.07.2017
 *      Author: pyro
 */

#include "../constants/Enums.hpp"
#include "DiagrammData.hpp"
#include <vector>
#include <ncurses.h>
#include "ASObject.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#ifndef PIPEDATA_PIPEREADER_HPP_
#define PIPEDATA_PIPEREADER_HPP_

class PipeReader{
private:
	std::string pipeName;
	std::vector<std::string> formatters;
	std::vector<DiagrammTyp> typs;
	std::vector<int> row, column, countRow, countColumn;
	std::vector<bool> reverseXY;
	std::vector<DiagrammData*> diagrammData;
	std::vector<WINDOW*> window;
	std::ifstream pipe;
	bool ignoreEOF;
	std::vector<std::string> xAchsisName, yAchsisName, diagrammName;
	std::vector<int> colors;
	int colorcounter = 1;
	std::map<std::string,WINDOW*> diagrams;


public:
	int addDiagramm(std::string formatter, int row, int column, int countRow, int countColumn, bool reverseXY, DiagrammTyp typ, NCURSES_COLOR_T color, std::string nameXAchse, std::string nameYAchse, std::string nameDiagram);
	int startListener();

private:
	void run();
	void readInformationFromLine(int position, std::string line);
	void rekursiveTopListRead(std::string formatter, std::string line, int diagram);

public:
	PipeReader(std::string pipeName, bool ignoreEOF = false);
	~PipeReader();
};



#endif /* PIPEDATA_PIPEREADER_HPP_ */
