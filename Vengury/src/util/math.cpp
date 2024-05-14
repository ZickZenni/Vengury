#include "math.hpp"

#include <random>

namespace Vengury {
    int Random(int min, int max)
    {
        static bool first = true;
        if (first)
        {
            srand(time(NULL)); //seeding for the first time only!
            first = false;
        }
        return min + rand() % ((max + 1) - min);
    }

    float Random(float min, float max)
    {
        static bool first = true;
        if (first)
        {
            srand(time(NULL)); //seeding for the first time only!
            first = false;
        }
        return (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) * (max - min))) + min;
    }
}