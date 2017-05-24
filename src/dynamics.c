/** @file
   Implementacja dynamicznych struktur danych

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include "dynamics.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Monos MonosEmpty(){
    Monos r;
    r.cap = 4;
    r.monos = malloc(sizeof(Mono) * r.cap);
    r.length = 0;
    return r;
}

void MonosAppend(Monos* r, Mono m){
    if(r->length >= r->cap){
        r->cap = 3 * r->cap / 2;
        r->monos = realloc(r->monos, sizeof(Mono) * r->cap);
        assert(r->monos != NULL);
    }
    r->monos[r->length++] = m;
}

/**
 * Zwalnia pamięć używaną przez tablicę.
 * @param[in,out] r : tablica
 */
static void MonosFree(Monos* r){
    free(r->monos);
    r->monos = NULL;
}

void MonosDestroy(Monos* r){
    for(unsigned int i = 0; i < r->length; ++i){
        MonoDestroy(&r->monos[i]);
    }
    MonosFree(r);
}

Poly MonosMergeIntoPoly(Monos* m){
    Poly p = PolyAddMonos(m->length, m->monos);
    MonosFree(m);
    return p;
}

/**
 * Zwiększa ilość pamięci zaalokowanej dla łańcucha.
 * @param[in,out] s : łańcuch
 * @param[in] newcap : nowa pojemność
 */
static void StringExpand(String* s, int newcap){
    s->str = realloc(s->str, newcap);
    assert(s->str != NULL);
    s->capacity = newcap;
}

void StringDestroy(String* s){
    free(s->str);
    s->capacity = 0;
}

String StringEmpty(){
    String s;
    s.length = 0;
    s.str = NULL;
    StringExpand(&s, 10);
    s.str[0] = '\0';
    return s;
}

void StringAppend(String* s, char c){
    if(s->length >= s->capacity - 1){
        StringExpand(s, (3*s->capacity)/2);
    }

    assert(s->length < s->capacity);
    s->str[s->length++] = c;
    s->str[s->length] = '\0';
}

const char* StringCStr(String* s){
    return s->str;
}

unsigned int StringLength(const String* s){
    return s->length;
}

bool StringCmp(const String* s, const char* c){
    return strcmp(s->str, c) == 0;
}

void StringFree(String* s){
    free(s->str);
    s->str = NULL;
    s->length = s->capacity = 0;
}

PolyStack PolyStackEmpty(){
    PolyStack r;
    r.cap = 4;
    r.polys = malloc(sizeof(Poly) * r.cap);
    r.length = 0;
    return r;
}

unsigned int PolyStackSize(const PolyStack* stack){
    return stack->length;
}

bool PolyStackIsEmpty(const PolyStack* stack){
    return PolyStackSize(stack) == 0;
}

const Poly* PolyStackPeek(PolyStack* stack){
    assert(!PolyStackIsEmpty(stack));
    return &stack->polys[stack->length - 1];
}

Poly PolyStackPop(PolyStack* stack){
    assert(!PolyStackIsEmpty(stack));
    Poly p = stack->polys[stack->length - 1];
    stack->length--;
    //TODO maybe shrink sometimes?
    return p;
}

void PolyStackPush(PolyStack* s, Poly p){
    if(s->length >= s->cap){
        s->cap = 3 * s->cap / 2;
        s->polys = realloc(s->polys, sizeof(Poly) * s->cap);
        assert(s->polys != NULL);
    }
    assert(s->length < s->cap);
    s->polys[s->length++] = p;
}

void PolyStackDestroy(PolyStack* stack){
    for(unsigned int i = 0; i < stack->length; ++i){//usuwamy wielomiany które pozostały na stosie
        PolyDestroy(&stack->polys[i]);
    }
    free(stack->polys);
    stack->polys = NULL;
    stack->length = stack->cap = 0;
}