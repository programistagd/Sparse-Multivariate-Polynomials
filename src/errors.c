#include "errors.h"
#include "parser.h"

#include <assert.h>
#include <stdio.h>

ParsingResult ParsingError(){
    ParsingResult r;
    r.type = PRT_ERROR;
    r.result.error.line = GetCurrentLine();
    r.result.error.column = GetCurrentColumn();
    return r;
}

bool IsError(ParsingResult r){
    return r.type == PRT_ERROR;
}

ParsingResult PackCoeff(poly_coeff_t x){
    ParsingResult r;
    r.type = PRT_COEFF;
    r.result.coeff = x;
    return r;
}

poly_coeff_t UnpackCoeff(ParsingResult r){
    assert(r.type == PRT_COEFF);
    return r.result.coeff;
}

ParsingResult PackExp(poly_exp_t x){
    ParsingResult r;
    r.type = PRT_EXP;
    r.result.exp = x;
    return r;
}

poly_exp_t UnpackExp(ParsingResult r){
    assert(r.type == PRT_EXP);
    return r.result.exp;
}

ParsingResult PackDeg(unsigned int x){
    ParsingResult r;
    r.type = PRT_DEG;
    r.result.deg = x;
    return r;
}

unsigned int UnpackDeg(ParsingResult r){
    assert(r.type == PRT_DEG);
    return r.result.deg;
}

ParsingResult PackPoly(Poly p){
    ParsingResult r;
    r.type = PRT_POLY;
    r.result.poly = p;
    return r;
}

Poly UnpackPoly(ParsingResult r){
    assert(r.type == PRT_POLY);
    return r.result.poly;
}

void PrintParsingError(ParsingResult r){
    assert(r.type == PRT_ERROR);
    fprintf(stderr, "ERROR %d %d\n", r.result.error.line, r.result.error.column);
}