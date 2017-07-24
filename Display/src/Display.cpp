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
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

void readConfig(std::string filename) {
	std::ifstream config(filename);
	std::string line;
	char *pipereader = (char*) malloc(sizeof(char) * 255), *diagrammTyp = (char*) malloc(sizeof(char) * 255), *NameXAchse = (char*) malloc(sizeof(char) * 255), *NameYAchse = (char*) malloc(sizeof(char) * 255), *Formatter = (char*) malloc(sizeof(char) * 255), *DiagrammName = (char*) malloc(sizeof(char) * 255);
	memset(pipereader, 0, sizeof(char) * 255);
	memset(diagrammTyp, 0, sizeof(char) * 255);
	memset(NameXAchse, 0, sizeof(char) * 255);
	memset(NameYAchse, 0, sizeof(char) * 255);
	memset(Formatter, 0, sizeof(char) * 255);
	memset(DiagrammName, 0, sizeof(char) * 255);

	int XStart, YStart, XRange, YRange, Farbe, InvertierteParameter;
	if (config.is_open()) {
		while (getline(config, line)) {
			if (!(line.c_str()[0] == '#')) {
				sscanf(line.c_str(), "pipereader=%s {", pipereader);
				if (std::string(pipereader).size() > 0) {
					PipeReader* reader = new PipeReader(std::string(pipereader));
					while (getline(config, line)) {
						if (line.c_str()[0] == '}') {
							reader->startListener();
							break;
						}
						if (!(line.c_str()[0] == '#')) {
							sscanf(line.c_str(), "diagrammTyp=%s ,NameXAchse=%s ,NameYAchse=%s ,XStart=%i ,YStart=%i ,XRange=%i ,YRange=%i ,Farbe=%i ,Formatter=%s ,InvertierteParameter=%i ,DiagrammName=%s\n", diagrammTyp, NameXAchse, NameYAchse, &XStart, &YStart, &XRange, &YRange,
									&Farbe, Formatter, &InvertierteParameter, DiagrammName);
							DiagrammTyp typ;
							if (std::string(diagrammTyp) == std::string("KURVEINTERPOLIERT")) {
								typ = KURVEINTERPOLIERT;
							} else if (std::string(diagrammTyp) == std::string("KURVE")) {
								typ = KURVE;
							} else if (std::string(diagrammTyp) == std::string("BALKEN")) {
								typ = BALKEN;
							} else if (std::string(diagrammTyp) == std::string("TOPLIST")) {
								typ = TOPLIST;
							} else {
								return;
							}
							printf("diagrammTyp=%s ,NameXAchse=%s ,NameYAchse=%s ,XStart=%i ,YStart=%i ,XRange=%i ,YRange=%i ,Farbe=%i ,Formatter=%s ,InvertierteParameter=%i ,DiagrammName=%s\n", diagrammTyp, NameXAchse, NameYAchse, XStart, YStart, XRange, YRange,
																Farbe, Formatter, InvertierteParameter, DiagrammName);
							getchar();
							reader->addDiagramm(std::string(Formatter), XStart, YStart, YRange, XRange, (bool) InvertierteParameter, typ, Farbe, std::string(NameYAchse), std::string(NameXAchse), std::string(DiagrammName));
						}
					}
				}
			}
		}
	}
}

int main(int argc, char **argv) {
	NcursesFunctions::initialise();
//	PipeReader* reader = new PipeReader("outage");
//	std::string formater1 = "%i|%*i|%i";
//	reader->addDiagramm(formater1, 0, 0, 10, 100, true, DiagrammTyp::BALKEN, COLOR_RED, "xAchse", "yAchse", "AS Valids");
//	std::string formater2 = "%i|%i";
//	reader->addDiagramm(formater2, 0, 0, 10, 100, true, DiagrammTyp::KURVEINTERPOLIERT, COLOR_MAGENTA, "xAchse1", "yAchse1", "AS Valids");
//	std::string formater3 = "%*i|%*i|%*i|%*i|%s %i|%i|%s";
//	reader->addDiagramm(formater3, 0, 100, 50, 100, true, DiagrammTyp::TOPLIST, COLOR_WHITE, "", "", "Test3");
//	reader->startListener();
	readConfig("display.conf");
	char quit = 0;
	while (true) {
		quit = getchar();
		if (quit == 'q') {
			endwin();
		}
	}
//	sleep(10000);
}

/*pipereader=rtr{
 diagrammTyp=KURVEINTERPOLIERT,NameXAchse=Outages,NameYAchse=TimeStamp,XStart=0,YStart=0,XRange=100,YRange=50,Farbe=1,Formatter=%i|%*i|%i,InvertierteParameter=true
 }*/

