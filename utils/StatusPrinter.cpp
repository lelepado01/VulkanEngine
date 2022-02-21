
#include "StatusPrinter.h"

StatusPrinter::StatusPrinter(){}

StatusPrinter::~StatusPrinter(){}

int calculateTriangles(int n) {    
    if(n < 0)   return 1;    
    if(n == 0)  return 0;
    return ((2*n -2) *3) + calculateTriangles(n-2);   
}

void StatusPrinter::Print(unsigned int vertexNumber, float fps){
    if (counter % printIterval == 0){
        counter = 0;
        std::cout << "---------------------------\n";
        std::cout << "Vertices before Tessellation: " << vertexNumber << "\n";
        std::cout << "Estimated Total Vertices: " << vertexNumber * calculateTriangles(4) << "\n";
        std::cout << "FPS: " << fps << "\n";
        std::cout << "---------------------------\n";
    }
    counter++;
}

void StatusPrinter::Print(unsigned int vertexNumber, unsigned int culledFaces, float fps){
    if (counter % printIterval == 0){
        counter = 0;
        std::cout << "---------------------------\n";
        std::cout << "Vertices before Tessellation: " << vertexNumber << "\n";
        std::cout << "Estimated Total Vertices: " << vertexNumber * calculateTriangles(4) << "\n";
        std::cout << "Faces Frustrum Culled: " << culledFaces << "\n";
        std::cout << "FPS: " << fps << "\n";
        std::cout << "---------------------------\n";
    }
    counter++;
}