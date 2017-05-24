/** @file
   Obsługa błędów parsowania wielomianów i liczb (współczynnikó)
   
   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#ifndef __ERRORS_H__
#define __ERRORS_H__

#include "poly.h"

/**
 * Wynik parsowania
 * Może to być liczba różnego typu, wielomian lub błąd
 * Unia result trzyma wynik, którego typ jest określony przez enum type
 */
typedef struct ParsingResult{
    union{
        poly_coeff_t coeff;///< wynik parsowania współczynnika
        poly_exp_t exp;///< wynik parsowania wykładnika
        unsigned int deg;///< wynik parsowania numeru zmiennej wielomianu
        Poly poly;///< wynik parsowania wielomianu
        struct{
            unsigned int line;///< numer linii, w której nastąpił błąd
            unsigned int column;;///< numer wiersza, który powoduje, że parsowane dane są nieprawidłowe
        } error;///< opakowanie na błąd parsowania
    } result;///< pojemnik na wynik
    enum {
        PRT_ERROR,
        PRT_COEFF,
        PRT_EXP,
        PRT_DEG,
        PRT_POLY
    } type; ///< rodzaj trzymanego wyniku
} ParsingResult;

/**
 * Zwraca błąd parsowania z ustawionym miejscem (nr wiersza i linii) wystąpienia na aktualnie przetwarzany znak.
 * @return wynik parsowania będący błędem
 */
ParsingResult ParsingError();

/**
 * Sprawdza czy wynik jest błędem.
 * @param[in] r : wynik parsowania
 * @return true jeśli jest to błąd
 */
bool IsError(ParsingResult r);

/**
 * Tworzy wynik parsowania będący współczynnikiem.
 * @param[in] x : współczynnik
 * @return wynik parsowania
 */
ParsingResult PackCoeff(poly_coeff_t x);

/**
 * Wyłuskuje z wyniku współczynnik.
 * @param[in] r : wynik parsowania będący współczynnikiem
 * @return współczynnik
 */
poly_coeff_t UnpackCoeff(ParsingResult r);

/**
 * Tworzy wynik parsowania będący wykładnikiem.
 * @param[in] x : wykładnik
 * @return wynik parsowania
 */
ParsingResult PackExp(poly_exp_t x);

/**
 * Wyłuskuje z wyniku wykładnik.
 * @param[in] r : wynik parsowania będący wykładnikiem
 * @return wykładnik
 */
poly_exp_t UnpackExp(ParsingResult r);

/**
 * Tworzy wynik parsowania będący numerem zmiennej wielomianu.
 * @param[in] x : numer zmiennej
 * @return wynik parsowania
 */
ParsingResult PackDeg(unsigned int x);

/**
 * Wyłuskuje z wyniku numer zmiennej.
 * @param[in] r : wynik parsowania będący numerem zmiennej
 * @return numer zmiennej
 */
unsigned int UnpackDeg(ParsingResult r);

/**
 * Tworzy wynik parsowania będący wielomianem (i przejmuje go na własność).
 * @param[in] p : wielomian
 * @return wynik parsowania
 */
ParsingResult PackPoly(Poly p);

/**
 * Wyłuskuje z wyniku wielomian.
 * @param[in] r : wynik parsowania będący wielomianem
 * @return wielomian
 */
Poly UnpackPoly(ParsingResult r);

/**
 * Wypisuje błąd parsowania.
 * @param[in] r : wynik parsowania będący błędem
 */
void PrintParsingError(ParsingResult r);


#endif