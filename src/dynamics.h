/** @file
   Dynamiczne struktury danych

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#ifndef __DYNAMICS_H__
#define __DYNAMICS_H__

#include "poly.h"

/**
 * Rozszerzalna tablica jednomianów
 */
typedef struct Monos{
    Mono* monos; ///< tablica jednomianów
    unsigned int length; ///< ilość jednomianów
    unsigned int cap; ///< ile jest zaalokowanego miejsca
} Monos;

/**
 * Tworzy pustą tablicę.
 * @return Tablica jednomianów
 */
Monos MonosEmpty();

/**
 * Dodaje jednomian do tablicy.
 * @param[in,out] r : tablica
 * @param[in] m : jednomian
 */
void MonosAppend(Monos* r, Mono m);

/**
 * Zwalnia pamięć i niszczy jednomiany zawarte w tablicy.
 * @param[in,out] r : tablica
 */
void MonosDestroy(Monos* r);

/**
 * Zamienia tablicę jednomianów w wielomian.
 *
 * Utworzony wielomian przejmuje wielomiany na własność, a sama tablica jest zwalniana.
 * @param[in,out] m : tablica
 * @return wielomian
 */
Poly MonosMergeIntoPoly(Monos* m);

/**
 * Rozszerzalny łańcuch znaków
 */
typedef struct String{
    char* str; ///< łańcuch znaków
    int length; ///< ilość użytych znaków
    int capacity; ///< zaalokowane miejsce
} String;

/**
 * Zwalnia pamięć używaną przez łańcuch.
 * @param[in,out] s : łańuch
 */
void StringDestroy(String* s);

/**
 * Zwraca pusty łańcuch znaków.
 * @return utworzony łańcuch znaków
 */
String StringEmpty();

/**
 * Dopisuje jedną literę do łańcucha.
 * @param[in,out] s : łańcuch
 * @param[in] c : dopisywany znak
 */
void StringAppend(String* s, char c);

/**
 * Zwraca tymczasowy C-string równy łańcuchowi
 * @param[in] s : łańcuch
 * @return C-string
 */
const char* StringCStr(String* s);

/**
 * Zwraca długość łańcucha
 * @param[in] s : łańcuch
 * @return długość
 */
unsigned int StringLength(const String* s);

/**
 * Sprawdza czy zadany łańcuch jest równy zadanemu C-stringowi.
 * @param[in] s : dynamiczny łańcuch
 * @param[in] c : C-string
 * @return true wtw. łańcuchy reprezentują ten sam tekst
 */
bool StringCmp(const String* s, const char* c);

/**
 * Zwalnia pamięć używaną przez łańcuch.
 * @param[in,out] s : łańcuch
 */
void StringFree(String* s);

/**
 * Stos wielomianów
 */
typedef struct PolyStack{
    Poly* polys; ///< tablica przechowująca wielomiany
    unsigned int length; ///< ilość wielomianów na stosie
    unsigned int cap; ///< aktualna pojemność stosu 
} PolyStack;

/**
 * Tworzy pusty stos wielomianów.
 * @return stos
 */
PolyStack PolyStackEmpty();

/**
 * Zwraca liczbę wielomianów na stosie.
 * @param[in] stack : stos
 * @return liczba wielomianów
 */
unsigned int PolyStackSize(const PolyStack* stack);

/**
 * Sprawdza czy stos jest pusty.
 * @param[in] stack : stos
 * @return true jeśli stos jest pusty
 */
bool PolyStackIsEmpty(const PolyStack* stack);

/**
 * Podgląda wielomian ze szczytu stosu
 * @param[in] stack : stos
 * @return wskaźnik na wielomian na szczycie stosu (tylko do odczytu)
 */
const Poly* PolyStackPeek(PolyStack* stack);

/**
 * Usuwa wielomian ze szczytu stosu i zwraca go. 
 * Wołający przejmuje wielomian na własność i ma obowiązek go zwolnić.
 * @param[in,out] stack : stos
 * @return wielomian ze szczytu
 */
Poly PolyStackPop(PolyStack* stack);

/**
 * Wrzuca wielomian na stos. 
 * Do czasu zrzucenia ze stosu przejmuje wielomian na własność.
 * @param[in,out] stack : stos
 * @param[in] p : wielomian
 */
void PolyStackPush(PolyStack* stack, Poly p);

/**
 * Zwalnia pamięć użytą przez stos i niszczy pozostałe na nim wielomiany.
 * @param[in,out] stack : stos
 */
void PolyStackDestroy(PolyStack* stack);

#endif