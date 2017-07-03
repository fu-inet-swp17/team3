/*
 * ASObject.hpp
 *
 *  Created on: 03.07.2017
 *      Author: pyro
 */

#include <string>

#ifndef PIPEDATA_ASOBJECT_HPP_
#define PIPEDATA_ASOBJECT_HPP_

class ASObject {

private:
	int asNumber;
	std::string asName;
	int rtrFails;

public:
	const std::string& getAsName() const;
	void setAsName(const std::string& asName);
	int getAsNumber() const;
	void setAsNumber(int asNumber);
	int getRtrFails() const;
	void setRtrFails(int rtrFails);
	bool operator<(const ASObject a1);
};

#endif /* PIPEDATA_ASOBJECT_HPP_ */
