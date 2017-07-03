/*
 * DiagrammData.cpp
 *
 *  Created on: 03.07.2017
 *      Author: pyro
 */

#include "DiagrammData.hpp"
#include <set>
#include "ASObject.hpp"

DiagrammData::DiagrammData(DiagrammTyp typ) {
	this->typ = typ;
}

DiagrammData::~DiagrammData() {
}

void DiagrammData::addTupel(int xValue, int yValue) {
	this->xValue.push_back(xValue);
	this->yValue.push_back(yValue);
}

void DiagrammData::addLine(std::string line) {
	this->line.push_back(line);
}

void DiagrammData::changeTopListElement(int asNumber, int value) {
	//TODO:: ad some magic
}

void DiagrammData::constructASNames() {
}

void DiagrammData::constractColumArray(int* array, int length, int timeoverall) {
	int lastXValue = xValue[xValue.size() - 1];
	int firstxValue = lastXValue - timeoverall;
	while (firstxValue > xValue[0]) {
		xValue.erase(xValue.begin());
		yValue.erase(yValue.begin());
	}
	if(firstxValue < 0){
		firstxValue = 0;
	}
	int xValuePerColumn = timeoverall / length;
	int count = 0;
	for (int i = 0; i < length; i++) {
		array[i] = 0;
	}
	for (int i = 0; i < length; i++) {
		while ((firstxValue+(xValuePerColumn*(i+1))) > xValue[count]) {
			if(count > yValue.size()){
				return;
			}
			array[i] += yValue[count];
			count++;
		}
	}
}
