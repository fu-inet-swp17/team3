/*
 * OutageData.hpp
 *
 *  Created on: 04.06.2017
 *      Author: pyro
 */

#ifndef OUTAGEDATA_HPP_
#define OUTAGEDATA_HPP_

class Tupel{



private:
	unsigned int timesstamp;
	unsigned int countPrefix;

public:
	unsigned int getTimestamp(void) const;
	unsigned int getCountPrefixes(void) const;
	bool operator()(const Tupel* a1, const Tupel* a2) const;
	Tupel(int timestamp, int countPrefix);
	Tupel(void);
};


#endif /* OUTAGEDATA_HPP_ */
