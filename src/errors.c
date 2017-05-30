/** @file
   Implementacja obsługi błędów parsowania wielomianów i liczb (współczynników)
   
   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include "errors.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>

#include "utils.h"

ParsingResult ParsingError()
{
    ParsingResult r;
    r.type = PRT_ERROR;
    r.result.error.line = GetCurrentLine();
    r.result.error.column = GetCurrentColumn();
    return r;
}

bool IsError(ParsingResult r)
{
    return r.type == PRT_ERROR;
}

ParsingResult PackCoeff(poly_coeff_t x)
{
    ParsingResult r;
    r.type = PRT_COEFF;
    r.result.coeff = x;
    return r;
}

poly_coeff_t UnpackCoeff(ParsingResult r)
{
    assert(r.type == PRT_COEFF);
    return r.result.coeff;
}

ParsingResult PackExp(poly_exp_t x)
{
    ParsingResult r;
    r.type = PRT_EXP;
    r.result.exp = x;
    return r;
}

poly_exp_t UnpackExp(ParsingResult r)
{
    assert(r.type == PRT_EXP);
    return r.result.exp;
}

ParsingResult PackVar(unsigned int x)
{
    ParsingResult r;
    r.type = PRT_VAR;
    r.result.var = x;
    return r;
}

unsigned int UnpackVar(ParsingResult r)
{
    assert(r.type == PRT_VAR);
    return r.result.var;
}

ParsingResult PackPoly(Poly p)
{
    ParsingResult r;
    r.type = PRT_POLY;
    r.result.poly = p;
    return r;
}

Poly UnpackPoly(ParsingResult r)
{
    assert(r.type == PRT_POLY);
    return r.result.poly;
}

void PrintParsingError(ParsingResult r)
{
    assert(r.type == PRT_ERROR);
    fprintf(stderr, "ERROR %d %d\n", r.result.error.line, r.result.error.column);
}