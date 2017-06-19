/*
 * OutageData.cpp
 *
 *  Created on: 04.06.2017
 *      Author: pyro
 */
#include "OutageData.hpp"

unsigned int Tupel::getTimestamp(void) const {
	return this->timesstamp;
}

unsigned int Tupel::getCountPrefixes(void) const{
	return this->countPrefix;
}

bool Tupel::operator ()(const Tupel* a1, const Tupel* a2) const {
	return a1->getTimestamp() < a2->getTimestamp();
}

Tupel::Tupel(int timestamp, int countPrefix) {
	this->timesstamp = timestamp;
	this->countPrefix = countPrefix;
}

Tupel::Tupel(void) {
}

