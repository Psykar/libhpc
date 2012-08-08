// Copyright 2012 Luke Hodkinson

// This file is part of libhpc.
// 
// libhpc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// libhpc is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with libhpc.  If not, see <http://www.gnu.org/licenses/>.

#include <math.h>
#include "libhpc/debug/debug.hh"
#include "num.hh"
#include "mymath.hh"

int powi(int x, int e) {
    int r = 1;
    ASSERT(x >= 0);
    while(e) {
	r *= x;
	e--;
    }
    return r;
}

void tri_bary(const double** crds, const double* pnt, double* bc) {
    double a, b, c, d, e, f;

    a = crds[0][0] - crds[2][0];
    b = crds[1][0] - crds[2][0];
    c = crds[2][0] - pnt[0];
    d = crds[0][1] - crds[2][1];
    e = crds[1][1] - crds[2][1];
    f = crds[2][1] - pnt[1];

    bc[0] = (b*f - c*e)/(a*e - b*d);
    bc[1] = (a*f - c*d)/(b*d - a*e);
    bc[2] = 1.0 - bc[0] - bc[1];
}