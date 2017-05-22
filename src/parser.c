/** @file
   Implementacja parsera poleceń

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

#include "parser.h"
#include "stack_calc.h"
#include "poly.h"

static bool initialized = false;
static char next = ' ';
static int lineno;
static int columnno;

typedef struct String{
    char* str;
    int length;
    int capacity;
} String;

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


//TODO move declarations up
char PeekChar();
#define DEBUGPEEK printf("|%c|\n",PeekChar());

bool EndOfStream(){
    assert(initialized);
    return PeekChar() == EOF;
}

void PopChar(){
    if(next == '\n'){
        lineno += 1;
        columnno = 0;
    }

    if(!EndOfStream()){
        next = getchar();
        columnno += 1;
    }
}

char PeekChar(){
    assert(initialized);
    return next;
}

char GetChar(){
    char c = PeekChar();
    PopChar();
    return c;
}

void Initialize(){
    assert(!initialized);
    initialized = true;
    lineno = 0;
    columnno = 0;
    next = getchar();

    //TODO prepare stack calc
}

bool HasMoreLines(){
    return !EndOfStream();
}

bool IsLetter(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsWhiteSpace(char c){
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

void ConsumeWhitespace(){
    while(IsWhiteSpace(PeekChar())){
        GetChar();
    }
}

void PrintError(const char* text){
    printf("ERROR %d %s\n", lineno, text);
}

void ParseCommand(){
    String s = StringEmpty();
    while(!IsWhiteSpace(PeekChar())){
        StringAppend(&s, GetChar());
    }
    ConsumeWhitespace();   

    printf("'%s'\n",StringCStr(&s));
    if(StringCmp(&s, "ZERO")){
        //TODO
    }
    else if(StringCmp(&s, "IS_COEFF")){
        //TODO
    }
    else if(StringCmp(&s, "IS_ZERO")){
        //TODO
    }
    else if(StringCmp(&s, "CLONE")){
        //TODO
    }
    else if(StringCmp(&s, "ADD")){
        //TODO
    }
    else if(StringCmp(&s, "MUL")){
        //TODO
    }
    else if(StringCmp(&s, "NEG")){
        //TODO
    }
    else if(StringCmp(&s, "SUB")){
        //TODO
    }
    else if(StringCmp(&s, "IS_EQ")){
        //TODO
    }
    else if(StringCmp(&s, "DEG")){
        //TODO
    }
    else if(StringCmp(&s, "DEG_BY")){
        //TODO idx
        //TODO
    }
    else if(StringCmp(&s, "AT")){
        //poly_coeff_t x = ReadCoeff();//TODO!
        ConsumeWhitespace();
        //TODO
    }
    else if(StringCmp(&s, "PRINT")){
        //TODO
    }
    else if(StringCmp(&s, "POP")){
        //TODO
    }
    else{
        PrintError("WRONG COMMAND");
    }
}

static bool polyparsingerror;

void PolyParsingError(){
    polyparsingerror = true;
    printf("ERROR %d %d\n", lineno, columnno);
}

bool IsDigit(char c){
    return c >= '0' && c <= '9';
}

poly_coeff_t ReadCoeff(){
    poly_coeff_t x = 0;
    bool adding = true;
    int digits = 0;

    if(PeekChar() == '-'){
        PopChar();
        adding = false;
    }

    while(IsDigit(PeekChar())){
        int d = GetChar() - '0';
        if(x > LONG_MAX / 10 || x < LONG_MIN / 10){
            PolyParsingError();
            return 0;
        }
        x *= 10;
        if(adding){
            if(LONG_MAX - x < d){
                PolyParsingError();
                return 0;
            }
            else{
                x += d;
            }
        }
        else{
            if(x - LONG_MIN < d){
                PolyParsingError();
                return 0;
            }
            else{
                x -= d;
            }
        }

        digits++;
    }

    if(digits == 0){
        PolyParsingError();
        return 0;
    }

    printf("coef: %ld\n",x);
    return x;
}

poly_coeff_t ReadExp(){
    poly_exp_t x = 0;
    int digits = 0;

    while(IsDigit(PeekChar())){
        int d = GetChar() - '0';
        if(x > INT_MAX / 10){
            PolyParsingError();
            return 0;
        }
        x *= 10;

        if(INT_MAX - x < d){
            PolyParsingError();
            return 0;
        }
        else{
            x += d;
        }
        digits++;
    }

    if(digits == 0){
        PolyParsingError();
        return 0;
    }

    printf("exp: %d\n",x);
    return x;
}

typedef struct Monos{
    Mono* monos;
    unsigned int length;
    unsigned int cap;
} Monos;

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

Poly ReadPoly(){
    if(PeekChar() == '('){//jednomian lub suma jednomianów
        Monos monos = MonosEmpty();

        #define FAIL MonosDestroy(&monos);\
                     return PolyZero(); 

        do{
            if(PeekChar() == '+'){
                PopChar();
            }

            if(GetChar() != '('){
                PolyParsingError();
                FAIL
            }
            printf("(\n");
            
            Mono inner;
            inner.p = ReadPoly();
            if(polyparsingerror){//nie trzeba sprzątać po tym Poly gdy był parsing error
                FAIL
            }

            if(GetChar() != ','){
                PolyParsingError();
                MonosDestroy(&monos);
                return PolyZero();
            }
            printf(",\n");
            inner.exp = ReadExp();
            //TODO if polyparsing error here??
            MonosAppend(&monos, inner);
            if(polyparsingerror){
                FAIL
            }

            if(GetChar() != ')'){
                PolyParsingError();
                FAIL
            }
            printf(")\n");
        }
        while(PeekChar() == '+');
        
        #undef FAIL

        return MonosMergeIntoPoly(&monos);
    }
    else{//stała
        return PolyFromCoeff(ReadCoeff());
    }
}

void PrintPoly(const Poly* p){
    if(PolyIsCoeff(p)){
        printf("%ld", p->coeff);
    }
    else{
        for(unsigned int i = 0; i < p->length; ++i){
            if(i > 0){
                printf("+");
            }
            printf("(");
            PrintPoly(&p->monos[i].p);
            printf(",%d)", p->monos[i].exp);
        }
    }
}

void ParseLine(){
    char first = PeekChar();
    if(IsLetter(first)){
        ParseCommand();
    }
    else{
        polyparsingerror = false;
        Poly p = ReadPoly();
        ConsumeWhitespace();
        if(!polyparsingerror){
            //testing
            PrintPoly(&p);printf("\n");
            //TODO stack calc add
        }
        else{
            next = EOF;
        }
    }
}