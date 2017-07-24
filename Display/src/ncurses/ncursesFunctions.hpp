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
#include "../constants/Enums.hpp"
#include "../pipeData/DiagrammData.hpp"

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
	volatile bool *closeB;

public:
	void close();
	void static initialise();
	NcursesFunctions(void);
	~NcursesFunctions(void);
	void printDiagramm(WINDOW *win, std::string yAchse, std::string xAchse, std::string diagrammName, DiagrammData data, DiagrammTyp typ, int color);

private:
	int printDiagram2(WINDOW *win, std::string yAchse, std::string xAchse, std::string diagrammName, int *array, int length, DiagrammTyp typ, int color);
	int printTopList(WINDOW *win, std::string name, DiagrammData data);
};



#endif /* NCURSESFUNCTIONS_HPP_ */
