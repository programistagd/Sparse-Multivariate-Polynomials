/** @file
   Interfejs parsera wejścia

   Pobiera dane z stdin i próbuje interpretować polecenia lub budować wielomian

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>
#include "poly.h"
#include "dynamics.h"
#include "errors.h"

void Initialize();

bool HasMoreLines();

void ParseLine(PolyStack* stack);

unsigned int GetCurrentLine();

unsigned int GetCurrentColumn();

void PrintError(const char* text);


bool EndOfStream();

void PopChar();

char PeekChar();

char GetChar();

bool IsLetter(char c);

bool IsCommandLetter(char c);

bool IsDigit(char c);

bool IsEnding(char c);

void ConsumeLine();

ParsingResult ReadCoeff();

ParsingResult ReadExp();

ParsingResult ReadDeg();

ParsingResult ReadPoly();

void ParseCommand(PolyStack* stack);

#endif