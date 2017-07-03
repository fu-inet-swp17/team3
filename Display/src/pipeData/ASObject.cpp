/*
 * ASObject.cpp
 *
 *  Created on: 03.07.2017
 *      Author: pyro
 */

#include "ASObject.hpp"

const std::string& ASObject::getAsName() const {
	return asName;
}

void ASObject::setAsName(const std::string& asName) {
	this->asName = asName;
}

int ASObject::getAsNumber() const {
	return asNumber;
}

void ASObject::setAsNumber(int asNumber) {
	this->asNumber = asNumber;
}

int ASObject::getRtrFails() const {
	return rtrFails;
}

void ASObject::setRtrFails(int rtrFails) {
	this->rtrFails = rtrFails;
}

bool ASObject::operator <(const ASObject a1) {
	return this->rtrFails < a1.getRtrFails();
}
