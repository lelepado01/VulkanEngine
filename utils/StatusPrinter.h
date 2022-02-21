
#include <iostream>
#include <cmath>

class StatusPrinter {
private:
    int counter = 0;
    int printIterval = 500; 

public:
    StatusPrinter();
    ~StatusPrinter();

    void Print(unsigned int vertexNumber, float fps); 
    void Print(unsigned int vertexNumber, unsigned int culledFaces, float fps); 
};
