#include "poly.h"

#include <assert.h>
#include <stdio.h>

//temporary:
void PolyPrint(const Poly* p, int var);

int main()
{
    Poly zero = PolyZero();
    Poly c1 = PolyFromCoeff(1);
    Poly n1 = PolyFromCoeff(-1);
    Poly c2 = PolyFromCoeff(2);
    Mono mx = MonoFromPoly(&c1, 1);
    Mono mx2 = MonoFromPoly(&c1, 2);
    Poly x = PolyAddMonos(1, &mx);
    Poly x2 = PolyAddMonos(1, &mx2);
    
    printf("0 = "); PolyPrint(&zero, 0);
    printf("1 = "); PolyPrint(&c1, 0);
    printf("2 = "); PolyPrint(&c2, 0);
    printf("-1 = "); PolyPrint(&n1, 0);
    printf("x = "); PolyPrint(&x, 0);
    printf("x^2 = "); PolyPrint(&x2, 0);

    Poly c2x = PolyMul(&c2, &x);
    Poly c4 = PolyMul(&c2, &c2);
    Poly n4 = PolyMul(&c4, &n1);
    Poly x3 = PolyMul(&x, &x2);
    Poly x4 = PolyMul(&x2, &x2);
    Poly n4x3 = PolyMul(&x3, &n4);

    printf("2x = "); PolyPrint(&c2x, 0);
    printf("4 = "); PolyPrint(&c4, 0);
    printf("-4 = "); PolyPrint(&n4, 0);
    printf("x^3 = "); PolyPrint(&x3, 0);
    printf("x^4 = "); PolyPrint(&x4, 0);
    printf("-4x^3 = "); PolyPrint(&n4x3, 0);

    Poly c2px = PolyAdd(&c2, &x);
    Poly c2pxpx2 = PolyAdd(&c2px, &x2);

    printf("2+x = "); PolyPrint(&c2px, 0);
    printf("2+x+x^2 = "); PolyPrint(&c2pxpx2, 0);

    Poly c3 = PolyAdd(&c2, &c1);
    printf("3 = "); PolyPrint(&c3, 0);

    Poly c5px = PolyAdd(&c3, &c2px);
    printf("5+x = "); PolyPrint(&c5px, 0);

    Poly c2test = PolySub(&c2px, &x);

    printf("2 = "); PolyPrint(&c2test, 0);
    assert(PolyIsEq(&c2, &c2test));
    
    Poly x_2 = PolyClone(&x);
    Mono my = MonoFromPoly(&x_2, 0);
    Poly x_3 = PolyClone(&x);
    Mono mxy = MonoFromPoly(&x_3, 1);
       
    Poly y = PolyAddMonos(1, &my);
    Poly xy = PolyAddMonos(1, &mxy);

    printf("y = "); PolyPrint(&y, 0);
    printf("xy = "); PolyPrint(&xy, 0);

    Poly c1py = PolyAdd(&c1, &y);
    printf("1+y = "); PolyPrint(&c1py, 0);

    Poly c2pxXc1py = PolyMul(&c2px, &c1py);
    printf("(1+y)(2+x) = 2+2y+x+xy = "); PolyPrint(&c2pxXc1py, 0);

    Poly mul2 = PolyMul(&c2pxpx2, &c2px);
    printf("(2+x+x^2)(2+x) = 4 + 4x + 3x^2 + x^3 = "); PolyPrint(&mul2, 0);

    
        Poly v2 = PolyAt(&c2, 123);
        printf("2 = "); PolyPrint(&v2, 0);

        Poly v27 = PolyAt(&x3, 3);
        printf("27 = "); PolyPrint(&v27, 0);

        Poly v8 = PolyAt(&c2pxpx2, -3);
        printf("8 = "); PolyPrint(&v8, 0);

        Poly v12 = PolyAt(&mul2, 1);
        Poly v4 = PolyAt(&mul2, 0);
        printf("12 = "); PolyPrint(&v12, 0);
        printf("4 = "); PolyPrint(&v4, 0);

        Poly v5p5y = PolyAt(&c2pxXc1py, 3);
        printf("5+5x = "); PolyPrint(&v5p5y, 0);
    
    //TODO cleanup -> PolyDestroy all
    Poly* polys[] = {
        &c1, &n1, &c2, &zero, &x, &x2, &c2x, &c4, &n4, &x3, &x4, &n4x3, 
        &c2px, &c2pxpx2, &c3, &c5px, &c2test, &x_2, &x_3, &y, &xy,
        &c1py, &c2pxXc1py, &mul2,
        &v2, &v27, &v8, &v12, &v4, &v5p5y
    };
    int polylen = sizeof(polys)/sizeof(Poly*);
    
    for(int i = 0; i< polylen; ++i){
        PolyDestroy(polys[i]);
    }
}
