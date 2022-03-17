
#include <iostream>
#include <cmath>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

class StatusPrinter {
private:
    int counter = 0;
    int printIterval = 100; 

	int terminalWidth = -1;
	int terminalHeight = -1;
	std::string separator; 

private: 
	int calculateTriangles(int n);  
	int getWindowSize();  
	void updateSeparator(); 

public:
    StatusPrinter();
    ~StatusPrinter();

	void Clear(); 
	void Separator(); 

    void Print(unsigned int vertexNumber, float fps); 
    void Print(unsigned int vertexNumber, unsigned int culledFaces, float fps); 
};
