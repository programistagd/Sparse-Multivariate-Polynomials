#ifndef __ERRORS_H__
#define __ERRORS_H__

#include "poly.h"

typedef struct ParsingResult{
    union{
        poly_coeff_t coeff;
        poly_exp_t exp;
        unsigned int deg;
        Poly poly;
        struct{
            unsigned int line;
            unsigned int column;
        } error;
    } result;
    enum {
        PRT_ERROR,
        PRT_COEFF,
        PRT_EXP,
        PRT_DEG,
        PRT_POLY
    } type;
} ParsingResult;

ParsingResult ParsingError();

bool IsError(ParsingResult r);

ParsingResult PackCoeff(poly_coeff_t x);

poly_coeff_t UnpackCoeff(ParsingResult r);

ParsingResult PackExp(poly_exp_t x);

poly_exp_t UnpackExp(ParsingResult r);

ParsingResult PackDeg(unsigned int x);

unsigned int UnpackDeg(ParsingResult r);

ParsingResult PackPoly(Poly p);

Poly UnpackPoly(ParsingResult r);

void PrintParsingError(ParsingResult r);


#endif