/*
 * Display.cpp
 *
 *  Created on: 02.07.2017
 *      Author: pyro
 */

#include <string>
#include "pipeData/PipeReader.hpp"
#include "ncurses/ncursesFunctions.hpp"
#include <unistd.h>

int main(int argc, char **argv) {
	NcursesFunctions::initialise();
	PipeReader* reader = new PipeReader("outage");
	std::string formater1 = "%i|%*i|%i";
	reader->addDiagramm(formater1, 0, 0, 10, 100, true, DiagrammTyp::BALKEN, COLOR_RED, "xAchse", "yAchse", "AS Valids");
	std::string formater2 = "%i|%i";
	reader->addDiagramm(formater2, 0, 0, 10, 100, true, DiagrammTyp::KURVEINTERPOLIERT, COLOR_MAGENTA, "xAchse1", "yAchse1", "AS Valids");
	std::string formater3 = "%*i|%*i|%*i|%*i|%s %i|%i|%s";
	reader->addDiagramm(formater3, 0, 100, 50, 100, true, DiagrammTyp::TOPLIST, COLOR_WHITE, "", "", "Test3");
	reader->startListener();
	char quit = 0;
	while(true){
		quit = getchar();
		if(quit == 'q'){
			endwin();
		}
	}
//	sleep(10000);
}
