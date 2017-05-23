/** @file
   Interfejs kalkulatora stosowego

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#ifndef __STACK_CALC_H__
#define __STACK_CALC_H__

#include "dynamics.h"
#include "poly.h"

void CalcZero(PolyStack* stack);

void CalcIsCoeff(PolyStack* stack);

void CalcIsZero(PolyStack* stack);

void CalcClone(PolyStack* stack);

void CalcAdd(PolyStack* stack);

void CalcMul(PolyStack* stack);

void CalcNeg(PolyStack* stack);

void CalcSub(PolyStack* stack);

void CalcIsEq(PolyStack* stack);

void CalcDeg(PolyStack* stack);

void CalcDegBy(PolyStack* stack, unsigned int idx);

void CalcAt(PolyStack* stack, poly_coeff_t x);

void CalcPrint(PolyStack* stack);

void CalcPop(PolyStack* stack);

void CalcPush(PolyStack* stack, Poly p);

#endif