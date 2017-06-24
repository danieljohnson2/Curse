#include "util.h"

/* Returns the sign of the value givne */
int
isign (int value)
{
    if (value > 0)
        return 1;
    else if (value < 0)
        return -1;
    else
        return 0;
}
