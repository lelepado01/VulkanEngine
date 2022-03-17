
#include "StatusPrinter.h"

StatusPrinter::StatusPrinter(){
	getWindowSize(); 
	updateSeparator(); 
}

StatusPrinter::~StatusPrinter(){}

int StatusPrinter::getWindowSize(){
	int cols = 80;
    int lines = 24;

    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    terminalWidth = ts.ts_cols;
    terminalHeight = ts.ts_lines;
}

void StatusPrinter::updateSeparator(){
	separator = "";
	for (int i = 0; i < terminalWidth; i++){
		separator.append("-"); 
	}
	separator.append("\n"); 
}

void StatusPrinter::Clear(){
	std::cout << "\x1B[2J\x1B[H";
}

void StatusPrinter::Separator(){
    std::cout << separator;
}

void StatusPrinter::Print(){
	Clear(); 
    if (counter % printIterval == 0){
        counter = 0;
		
		getWindowSize(); 		
		updateSeparator(); 

        Separator(); 

		for (const auto &element : variableDataDictionary){
        	std::cout << element.first << ": " << element.second << "\n";
		}
	
        Separator(); 
    }
    counter++;
}

void StatusPrinter::AddParam(std::string name, int value){
	variableDataDictionary[name] = value; 
}