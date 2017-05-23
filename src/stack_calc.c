/** @file
   Implementacja kalkulatora stosowego

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include "stack_calc.h"
#include <stdio.h>

void CalcZero(PolyStack* stack){
    CalcPush(stack, PolyZero());
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
    CalcPush(stack, copy);
}

void CalcAdd(PolyStack* stack){

}

void CalcMul(PolyStack* stack){
	
}

void CalcNeg(PolyStack* stack){
	
}

void CalcSub(PolyStack* stack){
	
}

void CalcIsEq(PolyStack* stack){
	
}

void CalcDeg(PolyStack* stack){
	
}

void CalcDegBy(PolyStack* stack, unsigned int idx){
	
}

void CalcAt(PolyStack* stack, poly_coeff_t x){
	
}

void CalcPrint(PolyStack* stack){
	
}

void CalcPop(PolyStack* stack){
	
}

void CalcPush(PolyStack* stack, Poly p){

}