//============================================================================
// Name        : anla.cpp
// Author      : Àî×Ó
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include "sxd.h"
#include "test.h"

using namespace std;

int main(int argc, char *argv[]) {

    std::srand((unsigned) std::time( NULL));

    if(argc >= 2)
        sxd::run(argv[1], true);
    else
        sxd::run("menu", false);

    return 0;
}
