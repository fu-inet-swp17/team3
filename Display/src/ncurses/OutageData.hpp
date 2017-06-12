/*
 * OutageData.hpp
 *
 *  Created on: 04.06.2017
 *      Author: pyro
 */

#ifndef OUTAGEDATA_HPP_
#define OUTAGEDATA_HPP_

class OutageData{



private:
	unsigned int timesstamp;
	unsigned int countPrefix;

public:
	unsigned int getTimestamp(void) const;
	unsigned int getCountPrefixes(void) const;
	bool operator()(const OutageData* a1, const OutageData* a2) const;
	OutageData(int timestamp, int countPrefix);
	OutageData(void);
};


#endif /* OUTAGEDATA_HPP_ */
