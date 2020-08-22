#include "maths.hpp" 

namespace lazy
{
	namespace maths
	{
        float   random()
        {
            return (float) (rand()) / (float) (RAND_MAX);
        }
    }
}