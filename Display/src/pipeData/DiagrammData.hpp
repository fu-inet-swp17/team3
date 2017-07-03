/*
 * DiagrammData.hpp
 *
 *  Created on: 02.07.2017
 *      Author: pyro
 */

#include <map>
#include <set>
#include <vector>
#include "ASObject.hpp"
#include "../constants/Enums.hpp"

#ifndef PIPEDATA_DIAGRAMMDATA_HPP_
#define PIPEDATA_DIAGRAMMDATA_HPP_


class DiagrammData{
public:
	std::vector<int> xValue, yValue;
private:
	std::vector<std::string> line;
	std::set<ASObject> topList;
	std::map<int,ASObject> asName;
	std::map<int,std::string> asNameList;
	DiagrammTyp typ;


public:
	DiagrammData(DiagrammTyp typ);
	~DiagrammData();
	void addTupel(int xValue, int yValue);
	void addLine(std::string line);
	void changeTopListElement(int asNumber, int value);
	void constructASNames();
	void constractColumArray(int *array, int length, int timeoverall);

};


#endif /* PIPEDATA_DIAGRAMMDATA_HPP_ */
