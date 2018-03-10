#include "Parser.h"
#include <iostream>
#include <math.h>
#include "lodepng.h"

int main(int argc, char* argv[]) {
    Scene* scene = Parser::parse(argc, argv);
    scene->render();

    return 0;
}