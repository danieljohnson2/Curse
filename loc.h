#ifndef LOC_H_INCLUDED
#define LOC_H_INCLUDED

#include "defs.h"

Loc make_loc (int x, int y);
Loc subtract_locs (Loc left, Loc right);
Loc add_locs (Loc left, Loc right);
Loc offset_loc (Loc origin, int dx, int dy);
bool equal_locs (Loc left, Loc right);

#endif
