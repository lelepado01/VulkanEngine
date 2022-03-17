
#include <iostream>
#include <cmath>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <unordered_map>

class StatusPrinter {
private:
    int counter = 0;
    int printIterval = 100; 
	int terminalWidth = -1;
	int terminalHeight = -1;
	
	std::string separator; 

	std::unordered_map<std::string, int> variableDataDictionary; 

private: 
	int getWindowSize();  
	void updateSeparator(); 

public:
    StatusPrinter();
    ~StatusPrinter();

	void Clear(); 
	void Separator(); 

    void Print(); 

	void AddParam(std::string name, int value); 
};
