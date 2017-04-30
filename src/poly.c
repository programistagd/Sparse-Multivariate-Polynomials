#include "poly.h"

#include <stdlib.h>

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

}

Poly PolyAddMonos(unsigned count, const Mono monos[]){

}

Poly PolyMul(const Poly *p, const Poly *q){

}

Poly PolyNeg(const Poly *p){

}

Poly PolySub(const Poly *p, const Poly *q){//simple implementation, consider optimizing???
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
    
}
