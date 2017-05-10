/** @file
   Implementacja interfejsu klasy wielomianów

   @author Jakub Pawlewicz <pan@mimuw.edu.pl>, Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-04-24, 2017-05-10
*/
#include "poly.h"

#include <stdlib.h>
#include <string.h> //memcpy

void PolyDestroy(Poly *p){
    if(p->length != 0){
        for(unsigned int i = 0; i < p->length; ++i){
            MonoDestroy(&p->monos[i]);
        }
        free(p->monos);
        p->monos = NULL;//na wypadek gdyby ktoś próbował destroyować ten wielomian po raz drugi, wyzeruj dane
        p->length = 0;
    }
}

Poly PolyClone(const Poly *p){
    Poly n;
    n.length = p->length;
    if(n.length == 0){
        n.coeff = p->coeff;
    }
    else{
        n.monos = malloc(sizeof(Mono) * n.length);
        for(unsigned int i = 0; i < n.length; ++i){
            n.monos[i] = MonoClone(&p->monos[i]);
        }
    }
    return n;
}

/**
 * Próbuje zmniejszyć zaalokowaną tablicę jednomianów do nowej wielkości
 * @param[in] m : wskaźnik do poczatku tablicy jednomianów
 * @param[in] new_len : nowa zadana długość (może być 0, wtedy tablica jest zwalniana)
 * @return nowy wskaźnik na tablicę jednomianów (potencjalnie pomniejszoną lub niezmienioną)
 */
static inline Mono* try_shrink_array(Mono* m, unsigned int new_len, unsigned int old_len){
    if(new_len == old_len) return m;
    
    if(new_len == 0){
        free(m);
        return NULL;
    }
    Mono* newm = realloc(m, sizeof(Mono) * new_len);
    if(newm == NULL) return m;
    return newm;
}

Poly PolyAdd(const Poly *p, const Poly *q){
    if(PolyIsCoeff(p) && PolyIsCoeff(q)){//szczególny przypadek - dodanie dwóch zwykłych liczb - nie bawimy się w ogóle zarządzaniem pamięcią
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    Poly r;
    unsigned int count = p->length + q->length;
    if(p->length == 0 || q->length == 0) count+=1;
    r.monos = malloc(sizeof(Mono) * count);

    const Poly* myp = p;
    const Poly* myq = q;

    //w tym miejscu conajwyżej jeden wielomian będzie stały (bo if na początku)
    //specjalny wielomian tymczasowy, wyjątkowo alokowany na stosie (więc nie trzeba PolyDestroy)
    Mono tmp_const;
    Poly tmp;
    tmp_const.exp = 0;
    tmp.monos = &tmp_const;
    tmp.length = 1;

    if(p->length == 0){
        tmp_const.p = PolyFromCoeff(p->coeff);
        myp = &tmp;
    }

    if(q->length == 0){
        tmp_const.p = PolyFromCoeff(q->coeff);
        myq = &tmp;
    }

    unsigned int pi = 0;
    unsigned int qi = 0;
    r.length = 0;

    //dodawanie i przerzucanie
    while(pi < myp->length && qi < myq->length){
        const Mono* pm = &myp->monos[pi];
        const Mono* qm = &myq->monos[qi];

        if(pm->exp == qm->exp){//równe wykładniki, dodajemy
            Mono m;
            m.exp = pm->exp;
            m.p = PolyAdd(&pm->p, &qm->p);

            if(PolyIsZero(&m.p)){//jeśli się wyzerowało
                PolyDestroy(&m.p);//czyścimy
            }
            else{//a jeśli nie, to wrzucamy do wyniku
                r.monos[r.length++] = m;
            }

            pi++;
            qi++;
        }
        else if(pm->exp < qm->exp){//przerzucamy mniejszy współczynnik (większy czeka na potencjalną parę)
            if(!PolyIsZero(&pm->p)){//jeśli ten współczynnik coś wnosi, kopiujemy
                r.monos[r.length++] = MonoClone(pm);
            }
            pi++;
        }
        else /*if(pm->exp > qm->exp)*/{
            if(!PolyIsZero(&pm->p)){
                r.monos[r.length++] = MonoClone(qm);
            }
            qi++;
        }
    }

    //przerzucamy resztę
    while(pi < myp->length){
        if(!PolyIsZero(&myp->monos[pi].p)){
            r.monos[r.length++] = MonoClone(&myp->monos[pi]);
        }
        pi++;
    }
    while(qi < myq->length){
        if(!PolyIsZero(&myq->monos[qi].p)){
            r.monos[r.length++] = MonoClone(&myq->monos[qi]);
        }
        qi++;
    }

    //sprawdzamy uproszczenia
    if(r.length == 0){//jeśli wszystko się wyzerowało -> wielomian zerowy
        free(r.monos);
        return PolyZero();
    }

    if(r.length == 1 && r.monos[0].exp == 0 && PolyIsCoeff(&r.monos[0].p)){//jeśli wielomian jest tak naprawdę stały, upraszczamy do współczynnika
        Poly c = r.monos[0].p;//przejmujemy wielomian zagnieżdżony
        free(r.monos);//zwalniamy pamięć "ręcznie" (nie za pomocą PolyDestroy) bo ukradliśmy tamten wielomian a to też jedyna rzecz którą jeszcze trzeba było zwolnić
        return c;
    }

    if(r.length == 1 && PolyIsZero(&r.monos[0].p)){
        PolyDestroy(&r);
        return PolyZero();
    }

    r.monos = try_shrink_array(r.monos, r.length, count);

    return r;
}

/**
 * Porównuje dwa jednomiany względem ich wykładnika. Używana do posortowania jednomianów w PolyAddMonos
 * @param[in] a : wskaźnik na pierwszy jednomian
 * @param[in] b : wskaźnik na drugi jednomian
 * @return różnica wykładników
 */
static int compare_monos(const void* a, const void* b){
    const Mono* ma = a;
    const Mono* mb = b;

    return ma->exp - mb->exp;
}

static inline void mysort(Mono* monos, int count){
    if(count == 1) return;
    if(count == 2){
        if(monos[0].exp > monos[1].exp){
            Mono m = monos[0];
            monos[0] = monos[1];
            monos[1] = m;
        }
        return;
    }
    qsort((void*) monos, count, sizeof(Mono), compare_monos);
}

Mono AddEqualMonos(Mono* monos, unsigned int count){
    if(count == 1){
        return monos[0];
    }
    //zakłada że monos[0]...monos[count-1] mają równe wykładniki
    unsigned int polys = 1;//coef
    for(unsigned int i = 0; i < count; ++i){
        polys += monos[i].p.length;
    }
    Mono* inside = malloc(sizeof(Mono) * polys);
    inside[0].p = PolyZero();
    inside[0].exp = 0;
    int p = 1;
    for(unsigned int i = 0; i < count; ++i){
        if(PolyIsCoeff(&monos[i].p)){
            inside[0].p.coeff += monos[i].p.coeff;
        }
        else{
            for(unsigned int j = 0; j < monos[i].p.length; ++j){
                inside[p++] = monos[i].p.monos[j];
            }
        }
    }
    Mono result;
    result.exp = monos[0].exp;
    int shift = 0;
    if(PolyIsZero(&inside[0].p)){
        shift = 1;
    }
    result.p = PolyAddMonos(polys - shift, inside + shift);
    free(inside);
    return result;
}

Poly PolyAddMonos(unsigned count, const Mono monos[]){
    if(count == 0) return PolyZero();
    Poly p;
    p.monos = malloc(count * sizeof(Mono));

    //sortujemy jednomiany po niemalejącym wykładniku
    mysort((Mono*)monos, count);

    p.length = 0;
    unsigned int start = 0;
    while(start < count){
        unsigned int equal_count = 1;
        
        while(start + equal_count < count && monos[start].exp == monos[start + equal_count].exp) equal_count++;

        Mono sum = AddEqualMonos((Mono*)&monos[start], equal_count);
        if(!PolyIsZero(&sum.p)){
            p.monos[p.length++] = sum;
        }
        else{
            MonoDestroy(&sum);
        }

        start += equal_count;
        //////
        /*
        if(p.length > 0){
            Mono* prev = &p.monos[p.length - 1];
            Mono* m = (Mono*)&monos[i];
            if(prev->exp == monos[i].exp){
                Poly sum = PolyAdd(&prev->p, &m->p);
                PolyDestroy(&prev->p);//stary współczynnik
                MonoDestroy(m);//dodany (zjedzony) jednomian
                prev->p = sum;
                if(PolyIsZero(&prev->p)){
                    MonoDestroy(prev);
                    p.length--;
                }
            }
            else{
                p.monos[p.length++] = monos[i];
            }
        }
        else{
            p.monos[p.length++] = monos[i];
        }*/
    }

    if(p.length == 0){//jeśli wszystko się wyzerowało -> wielomian zerowy
        PolyDestroy(&p);
        return PolyZero();
    }

    if(p.length == 1 && p.monos[0].exp == 0 && PolyIsCoeff(&p.monos[0].p)){//jeśli wielomian jest tak naprawdę stały, upraszczamy do współczynnika
        Poly c = p.monos[0].p;//przejmujemy wielomian zagnieżdżony
        free(p.monos);//zwalniamy pamięć "ręcznie" (nie za pomocą PolyDestroy) bo ukradliśmy tamten wielomian a to też jedyna rzecz którą jeszcze trzeba było zwolnić
        return c;
    }

    //jeśli nie używamy całej tablicy, zmniejsz jej rozmiar do minimum
    p.monos = try_shrink_array(p.monos, p.length, count);

    return p;
}

/**
 * Tworzy wielomian składający się z pojedynczego jednomianu
 * @param[in] m : jednomian (zostaje przejęty na własność przez utworzony wielomian)
 * @return wielomian składający się z `m`
 */
static Poly PolyFromMono(Mono* m){
    Poly p;
    p.monos = malloc(sizeof(Mono));
    p.monos[0] = *m;
    p.length = 1;
    return p;
}

/**
 * Mnoży pojedynczy jednomian przez wielomian (funkcja pomocnicza dla mnożenia)
 * @param[in] m : jednomian
 * @param[in] p : wielomian
 * @return `m * p`
 */
static Poly MonoMul(const Mono* m, const Poly* p){
    if(PolyIsCoeff(p)){//jeśli p jest współczynnikiem, mnożymy wnętrze jednomianu przez niego
        Mono mc;
        mc.p = PolyMul(&m->p, p);
        mc.exp = m->exp;
        if(PolyIsZero(&mc.p)){
            MonoDestroy(&mc);
            return PolyZero();
        }
        Poly r = PolyFromMono(&mc);
        return r;
    }
    
    //jeśli p jest sumą jednomianów, mnożymy każdy z nich oddzielnie i łączymy w wielomian
    Poly r;
    r.length = 0;
    r.monos = malloc(sizeof(Mono) * p->length);

    for(unsigned int i = 0; i < p->length; ++i){
        //r[i] = p[i] * m
        Poly mul = PolyMul(&m->p, &p->monos[i].p);//mnożymy współczynniki (potencjalnie wielomiany kolejnej zmiennej)
        if(!PolyIsZero(&mul)){
            r.monos[r.length].exp = m->exp + p->monos[i].exp;//dodajemy wykładniki
            r.monos[r.length].p = mul;
            r.length++;
        }
        else{
            PolyDestroy(&mul);
        }
    }

    r.monos = try_shrink_array(r.monos, r.length, p->length);

    return r;
}

Poly PolyMul(const Poly *p, const Poly *q){
    if(PolyIsCoeff(p) && PolyIsCoeff(q)){
        return PolyFromCoeff(p->coeff * q->coeff);
    }

    //conajmniej jeden z wielomianów nie jest współczynnikiem
    if(p->length == 0){//mnożenie jest przemienne, więc jeśli to wielomian p był stały, zamieniamy argumenty miejscami
        return PolyMul(q, p);
    }

    //od tego miejsca możemy (jakby bez straty ogólności) założyć, że wielomian p składa się z conajmniej jednego jednomianu

    //mnożymy poszczególne jednomiany przez drugi wielomian i sukcesywnie sumujemy wynik
    Poly acc = MonoMul(&p->monos[0], q);
    for(unsigned int i = 1; i < p->length; ++i){
        Poly m = MonoMul(&p->monos[i], q);
        Poly sum = PolyAdd(&acc, &m);//dodajemy do wyniku kolejną składową

        PolyDestroy(&m);//sprzątamy już niepotrzebne wielomiany
        PolyDestroy(&acc);//niszczymy stary akumulator

        acc = sum;//nowy akumulator to policzona suma
    }

    return acc;
}

Poly PolyNeg(const Poly *p){
    if(PolyIsCoeff(p)){
        return PolyFromCoeff(-p->coeff);
    }

    Poly r;
    r.monos = malloc(sizeof(Mono) * p->length);
    r.length = p->length;
    for(unsigned int i = 0; i < p->length; ++i){
        r.monos[i].exp = p->monos[i].exp;
        r.monos[i].p = PolyNeg(&p->monos[i].p);
    }
    return r;
}

Poly PolySub(const Poly *p, const Poly *q){
    Poly qneg = PolyNeg(q);
    Poly sub = PolyAdd(p, &qneg);
    PolyDestroy(&qneg);
    return sub;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx){
    if(PolyIsZero(p)) return -1;
    if(PolyIsCoeff(p)) return 0;
    if(var_idx == 0){
        return p->monos[p->length - 1].exp;
    }
    else{
        int m = 0;
        
        for(unsigned int i = 0; i < p->length; ++i){
            int deg = PolyDegBy(&p->monos[i].p, var_idx - 1);
            if(deg > m) m = deg;
        }

        return m;
    }
}

poly_exp_t PolyDeg(const Poly *p){
    if(PolyIsZero(p)) return -1;
    
    int m = 0;
    for(unsigned int i = 0; i < p->length; ++i){
        int d = p->monos[i].exp + PolyDeg(&p->monos[i].p);
        if(d > m) m = d;
    }

    return m;
}

bool PolyIsEq(const Poly *p, const Poly *q){
    /*Poly diff = PolySub(p, q);
    bool eq = PolyIsZero(&diff);
    PolyDestroy(&diff);
    return eq;*/
    
    if(p->length != q->length) return false;
    if(p->length == 0) return p->coeff == q->coeff;
    
    for(unsigned int i = 0; i < p->length; ++i){
        if(p->monos[i].exp != q->monos[i].exp) return false;
        if(!PolyIsEq(&p->monos[i].p, &q->monos[i].p)) return false;
    }

    return true;
}

/**
 * Podnosi x do potęgi k
 * @param[in] x : podstawa potęgowania
 * @param[in] k : wykładnik
 * @return `x ^ k`
 */
static poly_coeff_t Exp(poly_coeff_t x, poly_exp_t k){
    //TODO fast exponentiation in log(k)
    poly_coeff_t r = 1;
    while(k-- > 0) r*= x;
    return r;
}

Poly PolyAt(const Poly *p, poly_coeff_t x){
    if(PolyIsCoeff(p)) return PolyClone(p);//wielomain będący tylko współczynnikiem pozostaje niezmieniony (wprawdzie współczynnik nie używa dynamicznej pamięci ale na wszelki wypadek zwracamy kopię, gdyby coś się zmieniło)

    poly_exp_t prevk = p->monos[0].exp;
    poly_coeff_t xk = Exp(x, prevk);

    Poly coeff = PolyFromCoeff(xk);
    Poly r = PolyMul(&p->monos[0].p, &coeff);

    for(unsigned int i = 1; i < p->length; ++i){
        poly_exp_t k = p->monos[i].exp;
        xk = xk * Exp(x, k - prevk);
        prevk = k;

        Poly coeff = PolyFromCoeff(xk);//tego wielomianu nie niszczę bo nie alokuje on pamięci
        Poly m = PolyMul(&p->monos[i].p, &coeff);

        Poly sum = PolyAdd(&m, &r);

        PolyDestroy(&m);//niszczymy wielomian pomocniczy
        PolyDestroy(&r);//niszczymy stary wynik (już mamy nowy)

        r = sum;//podmieniamy
    }

    return r;
}

//debugging functions
#include <stdio.h>
/**
 * Funkcja do testowania. Wypisuje wielomian w postaci czytelnej dla użytkownika. Dla dużych wielomianów nie musi wypisać całości.
 * @param[in] p : wielomian
 * @param[in] var : indeks zmiennej po której jest ten wielomian (domyślnie 0)
 */
void PolyPrint(const Poly* p, int var){
    if(var >= 11){
        printf("[...]");
        return;
    }

    const char* chars = "xyzwpqrstuv";

    if(PolyIsCoeff(p)){
        printf("%ld",p->coeff);
    }
    else{
        if(p->length > 1 && var != 0){
            printf("(");
        }

        for(unsigned int i = 0; i < p->length; ++i){
            if(i > 0){
                printf(" + ");
            }
            PolyPrint(&p->monos[i].p, var+1);
            
            if(p->monos[i].exp >= 1 || true){//for testing FIXME
                printf("%c", chars[var]);
                if(p->monos[i].exp > 1 || true){
                    printf("^%d",p->monos[i].exp);
                }
            }
            else{
                //do nothing, printing coef is handled by PolyPrint run reursively
            }
        }

        if(p->length > 1 && var != 0){
            printf(")");
        }
    }
    if(var == 0){
        printf("\n");
    }
}