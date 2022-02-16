
#include "StatusPrinter.h"

StatusPrinter::StatusPrinter(){}

StatusPrinter::~StatusPrinter(){}

void StatusPrinter::Print(unsigned int vertexNumber, float fps){
    if (counter % printIterval == 0){
        counter = 0;
        std::cout << "---------------------------\n";
        std::cout << "Vertices before Tessellation: " << vertexNumber << "\n";
        std::cout << "Estimated Total Vertices: " << vertexNumber * pow(2, 4)<< "\n";
        std::cout << "FPS: " << fps << "\n";
        std::cout << "---------------------------\n";
    }
    counter++;
}