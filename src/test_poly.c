#include "poly.h"

#include <assert.h>

//temporary:
void PolyPrint(const Poly* p, int var);

int main()
{
    Poly zero = PolyZero();
    Poly c1 = PolyFromCoeff(1);
    Poly c2 = PolyFromCoeff(2);
    Mono mx = MonoFromPoly(&c1, 1);
    Mono mx2 = MonoFromPoly(&c1, 2);
    Poly x = PolyAddMonos(1, &mx);
    Poly x2 = PolyAddMonos(1, &mx2);
    
    PolyPrint(&zero, 0);
    PolyPrint(&c1, 0);
    PolyPrint(&c2, 0);
    PolyPrint(&x, 0);
    PolyPrint(&x2, 0);

    Poly c2px = PolyAdd(&c2, &x);
    Poly c2pxpx2 = PolyAdd(&c2px, &x2);

    PolyPrint(&c2px, 0);
    PolyPrint(&c2pxpx2, 0);

    Poly c2test = PolySub(&c2px, &x);

    assert(PolyIsEq(&c2, &c2test));
}
