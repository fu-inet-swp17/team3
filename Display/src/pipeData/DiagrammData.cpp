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
	unsigned int count = 0;
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

void DiagrammData::addASToList(int as, int value) {
	this->asList[as] += value;

}

void DiagrammData::getASWithNumberInTopList(std::vector<int> *asList) {
	std::map<int,int> topList;
	for (std::map<int,int>::iterator it=this->asList.begin(); it!=this->asList.end(); it++){
		topList[this->asList[it->first]] = it->first;
	}
	for (std::map<int,int>::iterator it=--topList.end(); it!=topList.begin(); it--){
		asList->push_back(topList[it->first]);
	}
	std::map<int,int>::iterator it=topList.begin();
	asList->push_back(topList[it->first]);
}

int DiagrammData::getValueForAS(int as) {
	return asList[as];
}
