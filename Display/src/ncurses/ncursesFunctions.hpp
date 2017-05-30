/*
 * ncursesFunctions.hpp
 *
 *  Created on: 27.05.2017
 *      Author: pyro
 */

#include <map>

#ifndef NCURSESFUNCTIONS_HPP_
#define NCURSESFUNCTIONS_HPP_

class NcursesFunctions{
private:
	int screenState = 0;
	const static int MainWindow = 0;
	const static int OptionsMenu = 1;
	const static int SourceMenu = 2;
	const static int reloadSources = 3;
	std::map<int, long>* asMap;
	std::map<const char*, long>* sources;

public:
	int printScreen();
	void initialise(std::map<int, long> *asMap, std::map<const char*, long> *sources);
	void close();
	NcursesFunctions(void);
	~NcursesFunctions(void);

private:
	int printOptionsMenu(void);
	int printMainScreen();
	int printSourceMenu();
	int switchScreen(void);
};



#endif /* NCURSESFUNCTIONS_HPP_ */
