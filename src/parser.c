/** @file
   Implementacja parsera poleceń

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "parser.h"
#include "stack_calc.h"
#include "poly.h"

static bool initialized = false;
static char next = ' ';
static int lineno = 1;

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

bool EndOfStream(){
    assert(initialized);
    return PeekChar() == EOF;
}

void PopChar(){
    if(next == '\n'){
        lineno += 1;
    }

    if(!EndOfStream()){
        next = getchar();
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

poly_coeff_t ReadCoeff(){

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
        poly_coeff_t x = ReadCoeff();
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

Poly ReadPoly(){
    //TODO
}

void ParseLine(){
    char first = PeekChar();
    if(IsLetter(first)){
        ParseCommand();
    }
    else{
        Poly p = ReadPoly();
        //TODO stack calc add
    }
}