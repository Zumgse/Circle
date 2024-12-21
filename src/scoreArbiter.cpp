#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

int scoreArbiter(
            int numCircsKilled,
            int numTrisKilled,
            int numSqsKilled
        ){
    return ((numCircsKilled * 3) + (numTrisKilled*2) + numSqsKilled);
}