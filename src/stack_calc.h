/** @file
   Interfejs kalkulatora stosowego

   Wszystkie funkcje w tym module sprawdzają czy na stosie jest wystarczająca liczba elementów i wypisują błąd oraz anulują swoje działanie, gdy brakuje.
   
   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#ifndef __STACK_CALC_H__
#define __STACK_CALC_H__

#include "dynamics.h"
#include "poly.h"

/**
 * Wrzuca na stos wielomian zerowy.
 * @param[in,out] stack : stos
 */
void CalcZero(PolyStack *stack);

/**
 * Sprawdza czy wielomian na szczycie stosu jest współczynnikiem i wypisuje 1 gdy jest lub 0 wpp.
 * @param[in,out] stack : stos
 */
void CalcIsCoeff(PolyStack *stack);

/**
 * Sprawdza czy wielomian na szczycie stosu jest zerowy i wypisuje 1 gdy jest lub 0 wpp.
 * @param[in,out] stack : stos
 */
void CalcIsZero(PolyStack *stack);

/**
 * Wrzuca na stos kopię wielomianu ze szczytu
 * @param[in,out] stack : stos
 */
void CalcClone(PolyStack *stack);

/**
 * Zdejmuje ze stosu dwa wielomiany i wrzuca z powrotem ich sumę
 * @param[in,out] stack : stos
 */
void CalcAdd(PolyStack *stack);

/**
 * Zdejmuje ze stosu dwa wielomiany i wrzuca z powrotem ich iloczyn
 * @param[in,out] stack : stos
 */
void CalcMul(PolyStack *stack);

/**
 * Zdejmuje ze stosu wielomian i wrzuca z powrotem wielomian o przeciwnym znaku
 * @param[in,out] stack : stos
 */
void CalcNeg(PolyStack *stack);

/**
 * Zdejmuje ze stosu dwa wielomiany i wrzuca z powrotem ich różnicę
 * @param[in,out] stack : stos
 */
void CalcSub(PolyStack *stack);

/**
 * Sprawdza czy dwa wielomiany na szczycie stosu są sobie równe i wypisuje 1 gdy są lub 0 wpp.
 * @param[in,out] stack : stos
 */
void CalcIsEq(PolyStack *stack);

/**
 * Wypisuje stopień wielomianu na szczycie stosu.
 * @param[in,out] stack : stos
 */
void CalcDeg(PolyStack *stack);

/**
 * Wypisuje stopień wielomianu na szczycie stosu względem zmiennej idx.
 * @param[in,out] stack : stos
 * @param[in] idx : numer zmiennej
 */
void CalcDegBy(PolyStack *stack, unsigned int idx);

void CalcCompose(PolyStack *stack, unsigned int count);

/**
 * Zdejmuje wielomian ze szczytu stosu, oblicza jego wartość w punkcie x i wrzuca wynik na stos.
 * @param[in,out] stack : stos
 * @param[in] x : punkt, w którym liczymy wartość
 */
void CalcAt(PolyStack *stack, poly_coeff_t x);

/**
 * Wypisuje wielomian ze szczytu stosu.
 * @param[in,out] stack : stos
 */
void CalcPrint(PolyStack *stack);

/**
 * Usuwa wielomian ze szczytu stosu.
 * @param[in,out] stack : stos
 */
void CalcPop(PolyStack *stack);

/**
 * Wrzuca wielomian na szczyt stosu (i przejmuje go na własność).
 * @param[in,out] stack : stos
 * @param[in] p : wielomian
 */
void CalcPush(PolyStack *stack, Poly p);

#endif