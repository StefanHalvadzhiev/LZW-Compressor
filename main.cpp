#include <iostream>
#include <filesystem>
#include "Terminal.h"
using namespace std;

int main() {
	Terminal terminal;

	while (terminal.isRunning()) {
		terminal.execute();
	}

	return 0;
}