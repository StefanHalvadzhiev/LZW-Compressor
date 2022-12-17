#pragma once
#include <iostream>
#include "LZW.h"
#include <string>

using namespace std;
using namespace std;
namespace fs = std::experimental::filesystem::v1;


class Terminal {
private:
	LZW compressor;

	bool running;
	string command;

	string input();

	void zip();
	void unzip();
	void info();
	void refresh();
	void ec();
	void help();

public:
	Terminal();
	Terminal(const Terminal&) = delete;
	void operator=(const Terminal&) = delete;

	bool isRunning() const;
	void execute();
};

