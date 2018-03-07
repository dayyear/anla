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

    srand((unsigned) time( NULL));

    if (argc > 2) {
        std::cerr << "usage: " << argv[0] << " [index] [--login] [--collect] [--test]" << std::endl;
    } else if (argc == 2) {
        if (std::string(argv[1]) == "--login")
            sxd::login();
        else if (std::string(argv[1]) == "--analyze")
            sxd::analyze();
        else if (std::string(argv[1]) == "--collect")
            sxd::collect();
        else if (std::string(argv[1]) == "--test")
            test::https();
        else {
            sxd::run(std::stoi(argv[1]));
        }
    } else {
        sxd::run();
    }
    return 0;
}
