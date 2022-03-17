
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

int StatusPrinter::calculateTriangles(int n) {    
    if(n < 0)   return 1;    
    if(n == 0)  return 0;
    return ((2*n -2) *3) + calculateTriangles(n-2);   
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

void StatusPrinter::Print(unsigned int vertexNumber, float fps){
	Clear();  
    if (counter % printIterval == 0){
        counter = 0;
		getWindowSize(); 
		updateSeparator(); 
		Separator(); 
        std::cout << "Vertices pre-Tess: \t" << vertexNumber << "\n";
        std::cout << "Total Vertices: \t" << vertexNumber * calculateTriangles(4) << "\n";
        std::cout << "FPS: \t" << fps << "\n";
        Separator(); 
    }
    counter++;
}

void StatusPrinter::Print(unsigned int vertexNumber, unsigned int culledFaces, float fps){
	Clear(); 
    if (counter % printIterval == 0){
        counter = 0;
		getWindowSize(); 		
		updateSeparator(); 
        Separator(); 
        std::cout << "Vertices pre-Tess: \t" << vertexNumber << "\n";
        std::cout << "Total Vertices: \t" << vertexNumber * calculateTriangles(4) << "\n";
        std::cout << "Faces Culled: \t\t" << culledFaces << "\n";
        std::cout << "FPS: \t\t\t" << fps << "\n";
        Separator(); 
    }
    counter++;
}