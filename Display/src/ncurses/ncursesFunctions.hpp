/*
 * ncursesFunctions.hpp
 *
 *  Created on: 27.05.2017
 *      Author: pyro
 */

#include <map>
#include <set>
#include <ncurses.h>
#include <string>
#include "OutageData.hpp"

#ifndef NCURSESFUNCTIONS_HPP_
#define NCURSESFUNCTIONS_HPP_

enum DiagrammTyp {BALKEN, KURVE, KURVEINTERPOLIERT};



class NcursesFunctions{
private:
	int screenState = 0;
	const static int MainWindow = 0;
	const static int OptionsMenu = 1;
	const static int SourceMenu = 2;
	const static int reloadSources = 3;
	std::map<int, long>* asMap;
	std::map<const char*, long>* sources;
	volatile bool *closeB;

public:
	int printScreen();
	void initialise(volatile bool *close);
	void close();
	NcursesFunctions(void);
	~NcursesFunctions(void);
	void printDiagramm(WINDOW *win, std::string *yAchse, std::string *xAchse, std::string *diagrammName, std::multiset<const OutageData*, OutageData> *data, DiagrammTyp typ, int color);
	void read();
	void setBool(volatile bool *close);

private:
	int printOptionsMenu(void);
	int printMainScreen();
	int printSourceMenu();
	int switchScreen(void);
	int printDiagram2(WINDOW *win, std::string *yAchse, std::string *xAchse, std::string *diagrammName, int *array, int length, DiagrammTyp typ, int color);
};



#endif /* NCURSESFUNCTIONS_HPP_ */
