/** @file
   Interfejs klasy wielomianów

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>, Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-04-24, 2017-05-10
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <stdbool.h>
#include <stddef.h>

/** Typ współczynników wielomianu */
typedef long poly_coeff_t;

/** Typ wykładników wielomianu */
typedef int poly_exp_t;

/* Forward-declaration, bo mamy cykliczne odwołania Mono<->Poly */
typedef struct Mono Mono;

/**
 * Struktura przechowująca wielomian
 * Zawiera posortowaną rosnąco tablicę jednomianów o różnych wykładnikach.
 * Reprezentowany wielomian jest sumą tych jednomianów.
 * Jeśli tablica jest pusta (length == 0), to wielomian jest wielomianem stałym i jego postać jest określona przez współczynnik coeff.
 * Coeff i monos są w unii, gdyż jednocześnie tylko jedna z tych dwóch zmiennych jest w użyciu (zależnie od tego czy length jest zerem czy nie), więc można oszczędzić pamięć.
 */
typedef struct Poly
{
    union {
        poly_coeff_t coeff; ///< współczynnik (jeśli length == 0)
        Mono *monos;        ///< tablica jednomianów
    };
    unsigned int length; ///< ilość jednomianów (lub 0 gdy wielomian stały)
} Poly;

/**
  * Struktura przechowująca jednomian
  * Jednomian ma postać `p * x^e`.
  * Współczynnik `p` może też być wielomianem.
  * Będzie on traktowany jako wielomian nad kolejną zmienną (nie nad x).
  */
typedef struct Mono
{
    Poly p;         ///< współczynnik
    poly_exp_t exp; ///< wykładnik
} Mono;

/**
 * Tworzy wielomian, który jest współczynnikiem.
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c)
{
    Poly p;
    p.coeff = c;
    p.length = 0;
    return p;
}

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian
 */
static inline Poly PolyZero()
{
    return PolyFromCoeff(0);
}

/**
 * Tworzy jednomian `p * x^e`.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] e : wykładnik
 * @return jednomian `p * x^e`
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e)
{
    return (Mono){.p = *p, .exp = e};
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p)
{
    return (p->length == 0);
}

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
static inline bool PolyIsZero(const Poly *p)
{
    return PolyIsCoeff(p) && p->coeff == 0;
}

/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
static inline void MonoDestroy(Mono *m)
{
    PolyDestroy(&m->p);
}

/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Robi pełną, głęboką kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m)
{
    Mono clone;
    clone.exp = m->exp;
    clone.p = PolyClone(&m->p);
    return clone;
}

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru).
 * Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianem
 * i zmniejszane są indeksy zmiennych w takim wielomianie o jeden.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Wypisuje wielomian na stdout.
 *
 * Wypisywany wielomian ma postać pojedynczej liczby (gdy współczynnik) lub (p1,e1)+(p2,e2)+...+(pn,en) gdy złożony wielomian (gdzie p1 to zagnieżdżone wielomiany)
 * @param[in] p : wielomian
 */
void PolyPrint(const Poly *p);

/**
 * Podnosi wielomian `p` do `k`-tej potęgi
 * @param[in] p : wielomian (podstawa)
 * @param[in] k : wykładnik
 * @return `p ^ k`
 */
Poly PolyExp(const Poly *p, unsigned int k);

/**
 * Podstawia kolejne wielomiany z `x` pod zmienne wielomianu `p`, jeśli count jest mniejszy niż liczba zmiennych w wielomianie, pod pozostałe zmienne podstawiane jest `0`
 * @param[in] p : wielomian, w którego będziemy podstawiać
 * @param[in] count : liczba wielomianów do podstawienia pod zmienne
 * @param[in] x : tablica wielomianów do podstawiania
 * @return wielomian, w ktorym na kolejne zmienne podstawione są wielomiany z `x` (lub 0 jeśli brakło wielomianów w tablicy)
 */
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);

#endif /* __POLY_H__ */
