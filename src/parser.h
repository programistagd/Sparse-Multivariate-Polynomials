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

void Initialize();

bool HasMoreLines();

void ParseLine(PolyStack* stack);

unsigned int GetCurrentLine();

unsigned int GetCurrentColumn();

void PrintPoly(const Poly* p);//TODO not sure if we want to export it

#endif