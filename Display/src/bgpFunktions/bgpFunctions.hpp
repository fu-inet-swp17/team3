/*
 * bgpFunctions.hpp
 *
 *  Created on: 25.05.2017
 *      Author: pyro
 */

#include <map>
#include <cstring>

#ifndef BGPFUNCTIONS_HPP_
#define BGPFUNCTIONS_HPP_

class less_str{
public:
	bool operator()(const char* s1, const char* s2){
		return std::strcmp(s1, s2) < 0;
	}
};

class BGPFunctions{
private:
	std::map<int, long>* asMap;
	std::map<const char*, long>* sources;

public:
	void initialise(std::map<int, long> *asMap, std::map<const char*, long> *sources);
	void updateBGPData(void);
	BGPFunctions();
	~BGPFunctions();
private:
	int bgpUpdates(long from, long to, const char *peerName);
};

#endif /* BGPFUNCTIONS_HPP_ */
