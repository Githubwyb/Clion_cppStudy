/*
 * @Author WangYubo
 * @Date 09/17/2018
 * @Description
 */

#include "log.hpp"

#include <iostream>
#include <exception>

using namespace std;

class Trace {
    static int counter;
    int objid;
public:
    Trace() {
        objid = counter++;
        cout << "Constructing Trace #" << objid << endl;
        if (objid == 3)
            throw 3;
    }
    ~Trace() {
        cout << "Destructing Trace #" << objid << endl;
    }
};

int Trace::counter = 0;

int main() {
    try {
        Trace n1;
        //Throws exception;
        Trace array[5];
        Trace n2; //Won't get here.
    } catch (int i) {
        cout << "Caught " << i << endl;
    }
}
