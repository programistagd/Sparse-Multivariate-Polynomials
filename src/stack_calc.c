/** @file
   Implementacja kalkulatora stosowego

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include "stack_calc.h"
#include <stdio.h>

void CalcZero(PolyStack* stack){
    PolyStackPush(stack, PolyZero());
}

void CalcIsCoeff(PolyStack* stack){
    const Poly* p = PolyStackPeek(stack);
    printf("%d\n", PolyIsCoeff(p) ? 1 : 0);
}

void CalcIsZero(PolyStack* stack){
    const Poly* p = PolyStackPeek(stack);
    printf("%d\n", PolyIsZero(p) ? 1 : 0);
}

void CalcClone(PolyStack* stack){
    Poly copy = PolyClone(PolyStackPeek(stack));
    PolyStackPush(stack, copy);
}

void CalcOp(PolyStack* stack, Poly (*op)(const Poly*, const Poly*)){
    Poly a = PolyStackPop(stack);
    Poly b = PolyStackPop(stack);

    Poly sum = op(&a, &b);
    PolyDestroy(&a);
    PolyDestroy(&b);

    PolyStackPush(stack, sum);
}

void CalcAdd(PolyStack* stack){
    CalcOp(stack, PolyAdd);
}

void CalcMul(PolyStack* stack){
	CalcOp(stack, PolyMul);
}

void CalcNeg(PolyStack* stack){
	Poly p = PolyStackPop(stack);
    Poly n = PolyNeg(&p);
    PolyDestroy(&p);
    PolyStackPush(stack, n);
}

void CalcSub(PolyStack* stack){
	CalcOp(stack, PolySub);
}

void CalcIsEq(PolyStack* stack){
	Poly p = PolyStackPop(stack);
    bool eq = PolyIsEq(&p, PolyStackPeek(stack));
    PolyStackPush(stack, p);
    printf("%d\n", eq ? 1 : 0);
}

void CalcDeg(PolyStack* stack){
	poly_exp_t deg = PolyDeg(PolyStackPeek(stack));
    printf("%d\n", deg);
}

void CalcDegBy(PolyStack* stack, unsigned int idx){
	poly_exp_t deg = PolyDegBy(PolyStackPeek(stack), idx);
    printf("%d\n", deg);
}

void CalcAt(PolyStack* stack, poly_coeff_t x){
	Poly p = PolyStackPop(stack);
    Poly n = PolyAt(&p, x);
    
    PolyDestroy(&p);
    PolyStackPush(stack, n);
}

void CalcPrint(PolyStack* stack){
	PolyPrint(PolyStackPeek(stack));
}

void CalcPop(PolyStack* stack){
	Poly p = PolyStackPop(stack);
    PolyDestroy(&p);
}

void CalcPush(PolyStack* stack, Poly p){
    PolyStackPush(stack, p);
}