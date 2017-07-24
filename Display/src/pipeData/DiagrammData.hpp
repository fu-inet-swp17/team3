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
	std::map<int,int> asList;
	std::map<int,std::string> asNameList;

	DiagrammTyp typ;


public:
	DiagrammData(DiagrammTyp typ);
	~DiagrammData();
	void addTupel(int xValue, int yValue);
	void constructASNames();
	void addASToList(int as, int value);
	void getASWithNumberInTopList(std::vector<int> *asList);
	void constractColumArray(int *array, int length, int timeoverall);
	int getValueForAS(int as);

};


#endif /* PIPEDATA_DIAGRAMMDATA_HPP_ */
