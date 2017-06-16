/*
 * OutageData.cpp
 *
 *  Created on: 04.06.2017
 *      Author: pyro
 */
#include "OutageData.hpp"

unsigned int OutageData::getTimestamp(void) const {
	return this->timesstamp;
}

unsigned int OutageData::getCountPrefixes(void) const{
	return this->countPrefix;
}

bool OutageData::operator ()(const OutageData* a1, const OutageData* a2) const {
	return a1->getTimestamp() < a2->getTimestamp();
}

OutageData::OutageData(int timestamp, int countPrefix) {
	this->timesstamp = timestamp;
	this->countPrefix = countPrefix;
}

OutageData::OutageData(void) {
}

