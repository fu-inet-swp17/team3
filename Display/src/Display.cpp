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
	std::string formater = "%i|%i";
	reader->addDiagramm(formater,0,0,10,100,false,DiagrammTyp::BALKEN);
	reader->startListener();
	sleep(10000);
}
