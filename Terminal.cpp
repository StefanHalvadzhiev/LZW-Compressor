#pragma once
#include "Terminal.h"

// PRIVATE METHODS

string Terminal::input(){
	string command;
	cin >> command;
	return command;
}

void Terminal::zip(){
	compressor.zip();
}

void Terminal::unzip(){
	compressor.unzip();
}

void Terminal::info(){
	compressor.info();
}

void Terminal::refresh(){
	compressor.refresh();
}

void Terminal::ec(){
	compressor.ec();
}

void Terminal::help(){
	cout << "< 1.  Use ZIP <f> <file count> <output file> <input file name 1> ... <input file name N> to create an archive with compressed files." << endl;
	cout << "  1.2 Use ZIP <d> <input path> <output path> to create an archive with compressed directory." << endl;
	cout << "  2.  Use UNZIP to unarchive one or all files from an archive in a given directory." << endl;
	cout << "  3.  Use INFO to list compressed files with information regarding the level of compression." << endl;
	cout << "  4.  Use REFRESH to update an archive." << endl;
	cout << "  5.  Use EC to check for any existing damages in an archive." << endl;
}

// PUBLIC METHODS

Terminal::Terminal(){
	running = true;
}

bool Terminal::isRunning() const{
	return running;
}

void Terminal::execute() {
	cout << "> ";
	command = input();
	
	if (command == "exit") {
		running = false;
	}
	else if (command == "ZIP") {
		zip();
	}
	else if (command == "UNZIP") {
		unzip();
	}
	else if (command == "INFO") {
		info();
	}
	else if (command == "REFRESH") {
		refresh();
	}
	else if (command == "EC") {
		ec();
	}
	else if (command == "help"){
		help();
	}
	else {
		cout << "< Unknown command." << endl;
	}
	cout << endl;
}
