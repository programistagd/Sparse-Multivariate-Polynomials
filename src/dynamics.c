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

void MonosFree(Monos* r){
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

void StringExpand(String* s, int newcap){
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

bool StringCmp(String* s, const char* c){
    return strcmp(s->str, c) == 0;
}