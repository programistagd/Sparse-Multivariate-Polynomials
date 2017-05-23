#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__

#include "poly.h"

typedef struct Monos{
    Mono* monos;
    unsigned int length;
    unsigned int cap;
} Monos;

Monos MonosEmpty();

void MonosAppend(Monos* r, Mono m);

void MonosFree(Monos* r);

void MonosDestroy(Monos* r);

Poly MonosMergeIntoPoly(Monos* m);

typedef struct String{
    char* str;
    int length;
    int capacity;
} String;

void StringExpand(String* s, int newcap);

void StringDestroy(String* s);

String StringEmpty();

void StringAppend(String* s, char c);

const char* StringCStr(String* s);

unsigned int StringLength(const String* s);

bool StringCmp(const String* s, const char* c);

typedef struct PolyStack{
    Poly* polys;
    unsigned int length;
    unsigned int cap;
} PolyStack;

PolyStack PolyStackEmpty();

unsigned int PolyStackSize(const PolyStack* stack);

bool PolyStackIsEmpty(const PolyStack* stack);

const Poly* PolyStackPeek(PolyStack* stack);

Poly PolyStackPop(PolyStack* stack);

void PolyStackPush(PolyStack* s, Poly p);

#endif