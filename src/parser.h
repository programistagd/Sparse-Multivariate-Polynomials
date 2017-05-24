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

/**
 * Inicjalizuje parser.
 * Ustawia liczniki wierszy, kolumn i przygotowuje jednoznakowy bufor.
 */
void Initialize();

/**
 * Sprawdza czy na wejściu są jeszcze niewczytane linie.
 * @return true jeśli są nowe linie
 */
bool HasMoreLines();

/**
 * Przetwarza jedną linijkę z wejścia i wykonuje na niej odpowiednie operacje związane ze stosem podanym w argumencie.
 * @param[in,out] stack : stos
 */
void ParseLine(PolyStack *stack);

/**
 * Zwraca numer aktualnie przetwarzanej linii.
 * @return numer linii
 */
unsigned int GetCurrentLine();

/**
 * Zwraca numer aktualnie przetwarzanej kolumny.
 * @return numer kolumny
 */
unsigned int GetCurrentColumn();

/**
 * Wypisuje nazwany błąd parsowania.
 * Błąd postaci: ERROR {nr kolumny} {text}
 * @param[in] text : tekst błędu
 */
void PrintError(const char *text);

/**
 * Sprawdza czy są jeszcze dane na wejściu.
 * @return true jeśli są dane na wejściu
 */
bool EndOfStream();

/**
 * Zjada jeden znak z bufora (i wczytuje następny).
 */
void PopChar();

/**
 * Podgląda następny znak w buforze.
 * @return znak
 */
char PeekChar();

/**
 * Zjada jeden znak z bufora i go zwraca.
 * @return znak
 */
char GetChar();

/**
 * Sprawdza czy podany znak jest literą.
 * @param[in] c : znak
 * @return true jeśli jest literą
 */
bool IsLetter(char c);

/**
 * Sprawdza czy podany znak może znaleźć się w komendzie (jest literą lub podkreśleniem).
 * @param[in] c : znak
 * @return true jeśli może znaleźć się w komendzie
 */
bool IsCommandLetter(char c);

/**
 * Sprawdza czy podany znak jest cyfrą.
 * @param[in] c : znak
 * @return true jeśli jest cyfrą
 */
bool IsDigit(char c);

/**
 * Sprawdza czy podany znak jest znakiem końca linii.
 * @param[in] c : znak
 * @return true jeśli jest końcem linii
 */
bool IsEnding(char c);

/**
 * Zjada wszystkie znaki w aktualnej linijce i przechodzi do nowej.
 * Używana np. gdy wystąpił błąd i anulujemy przetwarzanie aktualnej linijki.
 */
void ConsumeLine();

/**
 * Próbuje sparsować współczynnik.
 * @return wynik parsowania będący współczynnikiem lub błędem parsowania.
 */
ParsingResult ReadCoeff();

/**
 * Próbuje sparsować wykładnik.
 * @return wynik parsowania będący wykładnikiem lub błędem parsowania.
 */
ParsingResult ReadExp();

/**
 * Próbuje sparsować numer zmiennej.
 * @return wynik parsowania będący numerem zmiennej lub błędem parsowania.
 */
ParsingResult ReadDeg();

/**
 * Próbuje sparsować pojedynczy wielomian.
 * @return wynik parsowania będący wielomianem lub błędem parsowania.
 */
ParsingResult ReadPoly();

/**
 * Próbuje sparsować całą linijkę zawierającą wielomian.
 * Oprócz tego co sprawdza ReadPoly upewnia się też, że po wczytaniu wielomianu jest natychmiast przejście do nowej linii.
 * @return wynik parsowania będący wielomianem lub błędem parsowania.
 */
ParsingResult ParsePoly();

/**
 * Próbuje sparsować polecenie i wykonać je na podanym stosie.
 * @param[in] stack : stos
 */
void ParseCommand(PolyStack *stack);

#endif