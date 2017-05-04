#include "poly.h"

#include <stdlib.h>
#include <string.h> //memcpy

void PolyDestroy(Poly *p){
    if(p->length != 0){
        for(unsigned int i = 0; i < p->length; ++i){
            MonoDestroy(&p->monos[i]);
        }
        free(p->monos);
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

Poly PolyAdd(const Poly *p, const Poly *q){
    if(PolyIsCoeff(p) && PolyIsCoeff(q)){//szczególny przypadek - dodanie dwóch zwykłych liczb - nie bawimy się w ogóle zarządzaniem pamięcią
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    Poly r;
    r.monos = malloc(sizeof(Mono) * (p->length + q->length + 1));

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
        }
        else if(pm->exp < qm->exp){//przerzucamy mniejszy współczynnik (większy czeka na potencjalną parę)
            r.monos[r.length++] = MonoClone(pm);
            pi++;
        }
        else /*if(pm->exp > qm->exp)*/{
            r.monos[r.length++] = MonoClone(qm);
            qi++;
        }
    }

    //przerzucamy resztę
    while(pi < myp->length){
        r.monos[r.length++] = MonoClone(&myp->monos[pi++]);
    }
    while(qi < myq->length){
        r.monos[r.length++] = MonoClone(&myq->monos[qi++]);
    }

    //sprawdzamy uproszczenia
    if(r.length == 0){//jeśli wszystko się wyzerowało -> wielomian zerowy
        PolyDestroy(&r);//de facto free(r.monos)
        return PolyZero();
    }

    if(r.length == 1 && r.monos[0].exp == 0 && PolyIsCoeff(&r.monos[0].p)){//jeśli wielomian jest tak naprawdę stały, upraszczamy do współczynnika
        Poly c = r.monos[0].p;//przejmujemy wielomian zagnieżdżony
        free(r.monos);//zwalniamy pamięć "ręcznie" (nie za pomocą PolyDestroy) bo ukradliśmy tamten wielomian a to też jedyna rzecz którą jeszcze trzeba było zwolnić
        return c;
    }

    r.monos = realloc(r.monos, r.length * sizeof(Mono));

    return r;
}

int compare_monos(const void* a, const void* b){
    const Mono* ma = a;
    const Mono* mb = b;

    return ma->exp - mb->exp;
}

static void swap_monos(Mono* a, Mono* b){
    Mono c = *a;
    *a = *b;
    *b = c;
}

Poly PolyAddMonos(unsigned count, const Mono monos[]){
    if(count == 0) return PolyZero();
    Poly p;
    p.monos = malloc(count * sizeof(Mono));
    memcpy(p.monos, monos, count * sizeof(Mono));
    //sortujemy jednomiany po niemalejącym wykładniku
    qsort((void*) p.monos, count, sizeof(Mono), compare_monos);//TODO zrobić coś z tym const?
    p.length = 0;
    for(unsigned int i = 0; i < count; ++i){
        if(p.length > 0){
            Mono* prev = &p.monos[p.length - 1];
            if(prev->exp == monos[i].exp){
                Poly sum = PolyAdd(&prev->p, &p.monos[i].p);
                PolyDestroy(&prev->p);//stary współczynnik
                MonoDestroy(&p.monos[i]);//dodany (zjedzony) jednomian
                prev->p = sum;
                if(PolyIsZero(&prev->p)){
                    MonoDestroy(prev);
                    p.length--;
                }
            }
            else{
                //p.monos[p.length++] = monos[i];
                swap_monos(&p.monos[p.length++], &p.monos[i]);
            }
        }
        else{
            //p.monos[p.length++] = monos[i];
            swap_monos(&p.monos[p.length++], &p.monos[i]);//wielomian jest pusty, więc wrzucamy pierwszy jednomian
        }
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
    p.monos = realloc(p.monos, p.length * sizeof(Mono));

    return p;
}

Poly PolyMul(const Poly *p, const Poly *q){
    (void) p;
    (void) q;
    exit(0);//TODO
}

Poly PolyNeg(const Poly *p){
    Poly neg1 = PolyFromCoeff(-1);//TODO more efficient impl.
    return PolyMul(p, &neg1);
}

Poly PolySub(const Poly *p, const Poly *q){//simple implementation, TODO consider optimizing???
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
    if(p->length != q->length) return false;
    if(p->length == 0) return p->coeff == q->coeff;
    
    for(unsigned int i = 0; i < p->length; ++i){
        if(p->monos[i].exp != q->monos[i].exp) return false;
        if(!PolyIsEq(&p->monos[i].p, &q->monos[i].p)) return false;
    }

    return true;
}

Poly PolyAt(const Poly *p, poly_coeff_t x){
    (void) p;
    (void) x;
    exit(0);//TODO
}

//debugging functions
#include <stdio.h>
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
        if(p->length > 1){
            printf("(");
        }

        for(unsigned int i = 0; i < p->length; ++i){
            if(i > 0){
                printf(" + ");
            }
            PolyPrint(&p->monos[i].p, var+1);
            
            if(p->monos[i].exp >= 1 ){
                printf("%c", chars[var]);
                if(p->monos[i].exp > 1 ){
                    printf("^%d",p->monos[i].exp);
                }
            }
            else{
                //do nothing, printing coef is handled by PolyPrint run reursively
            }
        }

        if(p->length > 1){
            printf(")");
        }
    }
    if(var == 0){
        printf("\n");
    }
}