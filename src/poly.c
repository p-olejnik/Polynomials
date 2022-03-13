/** @file
  Implementacja funkcji z interfejsu

  @authors Paweł Olejnik <po417770@students.mimuw.edu.pl>
  @date 2021
*/

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "poly.h"

/**
 * Sprawdza poprawną alokację pamięci.
 * Jeśli pamięć nie została poprawnie zaalokowana, kończy program z kodem 1.
 * @param p : wskaźnik na zaalokowaną pamięć
 */ 
static void CheckPtr(const void *p) {
    if (p == NULL) exit(1);
}

/**
 * Porównuje wykładniki jednomianów.
 * Wartości wykładników są porównywane w taki sposób, aby funkcja qsort
 * sortowała je w porządku malejącym.
 *
 * @param[in] a : jednomian @f$a@f$
 * @param[in] b : jednomian @f$b@f$
 *
 * @return : 1 jeśli @f$a<b@f$, 0 jeśli @f$a=b@f$, -1 jeśli @f$a>b@f$
 */ 
static int CompareMonosExp(const void *a, const void *b) {
    if (((Mono*)a)->exp <  ((Mono*)b)->exp) return 1;
    if (((Mono*)a)->exp == ((Mono*)b)->exp) return 0;
    return -1;
}

/**
 * Sortuje tabilcę jendomianow malejąco ze względu na wykladniki,
 * używajac funkcji qsort.
 * 
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 */ 
static void SortMonosArray(size_t count, const Mono monos[]) {
    qsort((void*)monos, count, sizeof(Mono), CompareMonosExp);
}

/**
 * Sprawdza, czy tablica jednomianów jest posortowana malejąco
 * względem wykładnika.
 * 
 * @param[in] arr : tablica jednomianow
 * @param[in] size : liczba elementow tablicy jednomianow
 * @return czy tablica jest posortowana?
 */
static inline bool MonoArrayIsSorted(const Mono *arr, size_t size) {
    for (size_t i = 1; i < size; i++) {
        if (MonoGetExp(&arr[i-1]) < MonoGetExp(&arr[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Sprawdza, czy tablica jednomianów jest uproszczona: nie zawiera kilku
 * wielomianów o tym samym wykładniku, oraz nie zawiera jednomianów zerowych.
 *
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba jednomianów
 * @return : czy tablica jednomianów jest uproszczona?
 */ 
static inline bool MonoArrayIsSimplified(const Mono *monos, size_t count) {
    if (PolyIsZero(&monos[0].p)) return false;

    for (size_t i = 1; i < count; i++) {
        if (PolyIsZero(&monos[i].p) ||
            MonoGetExp(&monos[i-1]) == MonoGetExp(&monos[i])) {
            return false;
        }
    }
    
    return true;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsCoeff(p)) {
        return PolyIsZero(p) ? -1 : 0;
    }

    assert(MonoArrayIsSorted(p->arr, p->size));

    if (var_idx == 0) {
        return MonoGetExp(&p->arr[0]);
    }

    poly_exp_t max_deg = 0;

    for (size_t i = 0; i < p->size ; i++) {
        poly_exp_t tmp_deg = PolyDegBy(&p->arr[i].p, var_idx-1);
        if (tmp_deg > max_deg) max_deg = tmp_deg;
    }

    return max_deg;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsCoeff(p)) {
        return PolyIsZero(p) ? -1 : 0;
    }

    poly_exp_t max_deg = 0;

    for (size_t i = 0; i < p->size; i++) {
        poly_exp_t tmp_deg = PolyDeg(&p->arr[i].p) + MonoGetExp(&p->arr[i]);
        if (tmp_deg > max_deg) max_deg = tmp_deg;
    }

    return max_deg;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return p->coeff == q->coeff;
    }

    if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        return false;
    }

    if (p->size != q->size) {
        return false;
    }

    assert(
            MonoArrayIsSorted(p->arr, p->size) &&
            MonoArrayIsSimplified(p->arr, p->size)
    );

    for (size_t i = 0; i < p->size; i++) {
        if (MonoGetExp(&p->arr[i]) != MonoGetExp(&q->arr[i]) ||
            !PolyIsEq(&p->arr[i].p, &q->arr[i].p))
        {
            return false;
        }
    }

    return true;
}

Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }

    Mono *new_arr = (Mono*) calloc(p->size, sizeof(Mono));
    CheckPtr(new_arr);

    for (size_t i = 0; i < p->size; i++) {
        Poly new_poly = PolyClone(&(p->arr[i].p));
        new_arr[i] = MonoFromPoly(&new_poly, MonoGetExp(&p->arr[i]));
    }

    return (Poly) {.size = p->size, .arr = new_arr};
}

void DestroyMonoArray(Mono *arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        MonoDestroy(&arr[i]);
    }
    free(arr);
}

void PolyDestroy(Poly *p) {
    if (!PolyIsCoeff(p)) {
        DestroyMonoArray(p->arr, p->size);
    }
}

/**
 * Przesuwa elementy tablicy jednomianów w lewo o 1, rozpoczynając od zadanego
 * indeksu @p pos . Usuwa z pamięci jednomian na tej pozycji.
 *
 * @param[in] arr : tablica jednomianów
 * @param[in] size : liczba jednomianów
 * @param[in] pos : indeks tablicy, od którego rozpoczyna się przesunięcie
 */
static void ShiftMonoArray(Mono *arr, size_t size, size_t pos) {
    MonoDestroy(&arr[pos]);

    for (size_t i = pos; i < size-1; i++) {
        arr[i] = arr[i+1];
    }
}

/**
 * Upraszcza tablicę jednomianów, sumując współczynniki przy jednomianach o
 * tym samym wykładniku. W razie potrzeby usuwa z pamięci zbędne jednomiany,
 * modyfikuje tablicę @p arr i zmienia wartość zmiennej @p size .
 *
 * @param[in] arr : tablica jednomianów
 * @param[in] size : liczba jednomianów
 */
static void SimplifyMonoArray(Mono *arr, size_t *size) {
    SortMonosArray(*size, arr);

    // dodawanie do siebie jednomianów o tym samym wykładniku
    size_t i = 1;
    while (i < *size) {
        if (MonoGetExp(&arr[i]) == MonoGetExp(&arr[i-1])) {
            Poly tmp_poly = PolyAdd(&arr[i-1].p, &arr[i].p);
            PolyDestroy(&arr[i-1].p);
            arr[i-1].p = tmp_poly;
            ShiftMonoArray(arr, *size, i);
            (*size)--;
        } else {
            i++;
        }
    }

    // usuwanie zerowych jednomianów z tablicy
    size_t j = 0;
    while (j < *size) {
        if (PolyIsZero(&arr[j].p)) {
            MonoDestroy(&arr[j]);
            ShiftMonoArray(arr, *size, j);
            (*size)--;
        } else {
            j++;
        }
    }
}

/**
 * Funkcja generująca wielomian z listy jednomianów z założeniem,
 * że lista ta jest posortowana malejąco ze względu na wykładniki
 * oraz nie zawiera jednomianów zerowych.
 *
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return : wielomian będący sumą jednomianów
 */ 
static Poly PolyFromSimplifiedMonosArray(size_t count, Mono monos[]) {
     if (count == 0) {
        free(monos);
        return PolyZero();
    }

    if (count == 1 &&
        MonoGetExp(&monos[0]) == 0 &&
        PolyIsCoeff(&monos[0].p)
        ) {
        Poly res = PolyFromCoeff(monos[0].p.coeff);
        DestroyMonoArray(monos, count);
        return res;
    }

    assert(
            MonoArrayIsSorted(monos, count) &&
            MonoArrayIsSimplified(monos, count)
    );

    monos = (Mono*) realloc(monos, count * sizeof(Mono));
    CheckPtr(monos);

    return (Poly) {.arr = monos, .size = count};
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsZero(p)) return PolyClone(q);
    if (PolyIsZero(q)) return PolyClone(p);

    bool p_is_coeff = PolyIsCoeff(p);
    bool q_is_coeff = PolyIsCoeff(q);

    if (q_is_coeff && p_is_coeff) {
        return PolyFromCoeff(p->coeff + q->coeff);
    }

    if (p_is_coeff) {
        return PolyAdd(q, p);
    }

    // kopiowanie zawartosci wielomianow p i q do nowej tablicy
    size_t new_size = p->size + (q_is_coeff ? 1 : q->size);
    Mono *new_array = (Mono*) calloc(new_size, sizeof(Mono));
    CheckPtr(new_array);

    for (size_t i = 0; i < p->size; i++) {
        new_array[i] = MonoClone(&(p->arr[i]));
    }
    
    if (q_is_coeff) {
        Poly q_clone = PolyClone(q);
        new_array[p->size] = MonoFromPoly(&q_clone, 0);
    } else {
        for (size_t i = 0; p->size+i < new_size; i++) {
            new_array[p->size + i] = MonoClone(&q->arr[i]);
        }
    }

    SimplifyMonoArray(new_array, &new_size);

    return PolyFromSimplifiedMonosArray(new_size, new_array);
}


Poly PolySub(const Poly *p, const Poly *q) {
    Poly q_neg = PolyNeg(q);
    Poly res = PolyAdd(p, &q_neg);
    PolyDestroy(&q_neg);
    return res;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0) {
        return PolyZero();
    }

    Mono *new_arr = (Mono*) calloc(count, sizeof(Mono));
    CheckPtr(new_arr);
    memcpy(new_arr, monos, count * sizeof(Mono));

    size_t new_size = count;
    SimplifyMonoArray(new_arr, &new_size);

    return PolyFromSimplifiedMonosArray(new_size, new_arr);
}

Poly PolyOwnMonos(size_t count, Mono *monos) {
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }

    SimplifyMonoArray(monos, &count);
    return PolyFromSimplifiedMonosArray(count, monos);
}

/**
 * Robi głęboką kopię tablicy jednomianów.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return skopiowana tablica jednomianów
 */
static Mono* CloneMonoArray(size_t count, const Mono monos[]) {
    Mono *new_arr = (Mono*) calloc(count, sizeof(Mono));
    for (size_t i = 0; i < count; i++) {
        new_arr[i] = MonoClone(&monos[i]);
    }
    return new_arr;
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }

    Mono *monos_clone = CloneMonoArray(count, monos);
    return PolyOwnMonos(count, monos_clone);
}

/**
 * Modyfikuje rekurencyjnie współczynniki wielomianu, mnożąc współczynniki
 * o wartościach liczbowych przez stałą. Sprawdza, czy przy mnożeniu nie
 * nastąpił błąd.
 *
 * @param[in] p : wielomian
 * @param[in] c : stała
 * @return : czy modyfikacja wielomianu zakończyła się powodzeniem?
 */ 
static bool PolyMulCoeffs(Poly *p, poly_coeff_t c) {
    if (PolyIsCoeff(p)) {
        p->coeff = c * p->coeff;
        return p->coeff != 0;
    } 

    for (size_t i = 0; i < p->size; i++) {
        if (!PolyMulCoeffs(&p->arr[i].p, c)) {
            return false;
        }
    }

    return true;
}

/**
 * Mnoży wielomian przez stałą.
 *
 * @param[in] p : wielomian @f$p@f$
 * @param[in] c : stała @f$c@f$
 * @return : wielomian @f$cp@f$
 */ 
static Poly PolyMulByCoeff(const Poly *p, poly_coeff_t c) {
    if (c == 0) return PolyZero();
    if (c == 1) return PolyClone(p);
    if (PolyIsCoeff(p)) return PolyFromCoeff(p->coeff * c);


    Poly new_poly = PolyClone(p);
    if (!PolyMulCoeffs(&new_poly, c)) {
        PolyDestroy(&new_poly);
        return PolyZero();
    } 
    
    return new_poly;
}

Poly PolyNeg(const Poly *p) {
    return PolyMulByCoeff(p, -1);
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p)) return PolyMulByCoeff(q, p->coeff);
    if (PolyIsCoeff(q)) return PolyMulByCoeff(p, q->coeff);

    // mnożenie dwóch wielomianów stopnia > 0
    Mono *new_arr = (Mono*) calloc(p->size * q->size, sizeof(Mono));
    CheckPtr(new_arr);

    size_t k = 0;
    for (size_t i = 0; i < p->size; i++) {
        for (size_t j = 0; j < q->size; j++) {
            Poly tmp_poly = PolyMul(&p->arr[i].p, &q->arr[j].p);
            poly_exp_t tmp_exp = MonoGetExp(&p->arr[i])+MonoGetExp(&q->arr[j]);

            if (PolyIsZero(&tmp_poly)) tmp_exp = 0;

            new_arr[k++] = MonoFromPoly(&tmp_poly, tmp_exp);
        }
    }

    Poly res = PolyAddMonos(p->size * q->size, new_arr);
    free(new_arr);

    return res;
}

/**
 * Pomocnicza funkcja do obliczania potęgi całkowitej 
 * 
 * @param[in] x : współczynnik wielomianu @f$x@f$
 * @param[in] p : wykładnik @f$p@f$
 * @return @f$x^p@f$
 */
static poly_coeff_t CoeffPow(poly_coeff_t x, poly_exp_t p) {
    if (p == 0) return 1;

    poly_coeff_t tmp = CoeffPow(x, p / 2);
    if (p % 2 == 0) {
        return tmp * tmp;
    } else {
        return x * tmp * tmp;
    }
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) return PolyClone(p);

    Poly res = PolyZero();

    for (size_t i = 0; i < p->size; i++) {
        Poly poly_x = PolyFromCoeff(CoeffPow(x, MonoGetExp(&p->arr[i])));
        Poly poly_coeff = PolyMul(&p->arr[i].p, &poly_x);

        Poly tmp_res = PolyAdd(&res, &poly_coeff);
        PolyDestroy(&poly_coeff);
        PolyDestroy(&res);
        res = tmp_res;
    }

    return res;
}

/**
 * Oblicza potęgę wielomianu.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] exp : wykładnik @f$exp@f$
 * @return @f$p^{exp}@f$
 */ 
static Poly PolyPow(const Poly *p, poly_exp_t exp) {
    if (exp == 0) return PolyFromCoeff(1);
    if (exp == 0) return PolyClone(p);

    Poly tmp = PolyPow(p, exp/2);

    if (exp % 2 == 0) {
        Poly res = PolyMul(&tmp, &tmp);
        PolyDestroy(&tmp);
        return res;
    } else {
        Poly tmp2 = PolyMul(&tmp, &tmp);
        Poly res = PolyMul(p, &tmp2);
        PolyDestroy(&tmp);
        PolyDestroy(&tmp2);
        return res; 
    }
}

/**
 * Podstawia pod wszystkie zmienne wielomianu @f$p@f$ zera.
 * @param[in] p : wielomian @f$p@f$
 * @return @f$p(0, \ldots,0)@f$
 */ 
static Poly PolyComposeZero(const Poly *p) {
    if (PolyIsCoeff(p)) return PolyClone(p);

    for (size_t i = 0; i < p->size; i++) {
        if (p->arr[i].exp == 0) {
            return PolyComposeZero(&p->arr[i].p);
        }
    }
    
    return PolyZero();
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
    if (k == 0) return PolyComposeZero(p);
    if (PolyIsCoeff(p)) return PolyClone(p);
 
    Poly res = PolyZero();

    for (size_t i = 0; i < p->size; i++) {
        Poly coeff_poly = PolyCompose(&p->arr[i].p, k-1, q+1);
        Poly exp_poly = PolyPow(q, p->arr[i].exp);
        Poly mul_res = PolyMul(&coeff_poly, &exp_poly);
        Poly tmp = PolyAdd(&res, &mul_res);

        PolyDestroy(&coeff_poly);
        PolyDestroy(&exp_poly);
        PolyDestroy(&mul_res);
        PolyDestroy(&res);

        res = tmp;
    }
    
    return res;
}
