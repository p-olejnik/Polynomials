/** @file
  Interfejs funkcji obsługujących stos wielomianów.

  @authors Paweł Olejnik <po417770@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdlib.h>
#include <stdio.h>

#include "poly_stack.h"
#include "poly.h"

/**
 * Sprawdza poprawną alokację pamięci.
 * Jeśli pamięć nie została poprawnie zaalokowana, kończy program z kodem 1.
 * @param p : wskaźnik na zaalokowaną pamięć
 */
static void CheckPtr(const void *p) {
    if (p == NULL) exit(1);
}

void InitStack(PolyStack *s) {
    s->size = 0;
    s->top = -1;
    s->arr = (Poly*) calloc(s->size, sizeof(Poly));
    CheckPtr(s->arr);
}

/**
 * Alokuje dodatkową pamięć dla stosu.
 * @@param s : stos wielomianów
 */
static void ResizeStack(PolyStack *s) {
    s->size = 1 + 2 * s->size;
    Poly *tmp_arr = (Poly*) realloc(s->arr, s->size * sizeof(Poly));
    CheckPtr(tmp_arr);
    s->arr = tmp_arr;
}

bool IsEmpty(PolyStack *s) {
    return s->top == -1;
}

size_t StackSize(PolyStack *s) {
    return s->top + 1;
}

void Push(PolyStack *s, Poly p) {
    if (s->top == (int) s->size - 1) ResizeStack(s);
    s->arr[++(s->top)] = p;
}

Poly* Top(PolyStack *s) {
    return &s->arr[s->top];
}

void Pop(PolyStack *s) {
    PolyDestroy(Top(s));
    s->top--;
}

void FreeStack(PolyStack *s) {
    while (!IsEmpty(s)) {
        Pop(s);
    }
    free(s->arr);
}

/**
 * Wypisuje na standardowe wyjście wielomian, jako stałą lub sumę jednomianów.
 * @param p : wielomian
 */
static void PrintPoly(const Poly *p);

/**
 * Wypisuje na standardowe wyjście jednomian w formacie (wielomian, wykładnik).
 * @param m : jednomian
 */
static void PrintMono(const Mono *m) {
    printf("(");
    PrintPoly(&m->p);
    printf(",%d)", m->exp);
}

static void PrintPoly(const Poly *p) {
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
        return;
    }

    for (size_t i = p->size-1; i > 0; i--) {
        PrintMono(&p->arr[i]);
        printf("+");
    }

    PrintMono(&p->arr[0]);
}

void PrintTop(PolyStack *s) {
    if (!IsEmpty(s)) {
        PrintPoly(Top(s));
        printf("\n");
    }
}

void Zero(PolyStack *s) {
    Push(s, PolyZero());
}

void IsCoeff(PolyStack *s) {
    printf("%d\n", PolyIsCoeff(Top(s)));
}

void IsZero(PolyStack *s) {
    printf("%d\n", PolyIsZero(Top(s)));
}

void Clone(PolyStack *s) {
    Push(s, PolyClone(Top(s)));
}

void Add(PolyStack *s) {
    Poly res = PolyAdd(Top(s), &s->arr[s->top-1]);
    Pop(s);
    Pop(s);
    Push(s, res);
}

void Mul(PolyStack *s) {
    Poly res = PolyMul(Top(s), &s->arr[s->top-1]);
    Pop(s);
    Pop(s);
    Push(s, res);
}

void Neg(PolyStack *s) {
    Poly res = PolyNeg(Top(s));
    Pop(s);
    Push(s, res);
}

void Sub(PolyStack *s) {
    Poly res = PolySub(Top(s), &s->arr[s->top-1]);
    Pop(s);
    Pop(s);
    Push(s, res);
}

void IsEq(PolyStack *s) {
    bool res = PolyIsEq(Top(s), &(s->arr[s->top-1]));
    printf("%d\n", res);
}

void Deg(PolyStack *s) {
    poly_exp_t res = PolyDeg(Top(s));
    printf("%d\n", res);
}

void DegBy(PolyStack *s, size_t var_idx) {
    poly_exp_t res = PolyDegBy(Top(s), var_idx);
    printf("%d\n", res);
}

void At(PolyStack *s, poly_coeff_t x) {
    Poly res = PolyAt(Top(s), x);
    Pop(s);
    Push(s, res);
}

void Compose(PolyStack *s, size_t k) {
    Poly p = PolyClone(Top(s));
    Pop(s);

    // tworzenie tablicy wielomianów
    Poly *arr = (Poly*) calloc(k, sizeof(Poly));
    CheckPtr(arr);
    for (size_t i = 0; i < k; i++) {
        arr[k-i-1] = PolyClone(Top(s));
        Pop(s);
    }
    
    Push(s, PolyCompose(&p, k, arr));

    // zwalnianie tablicy wielomianów
    for (size_t i = 0; i < k; i++) {
        PolyDestroy(&arr[i]);
    }
    free(arr);
    PolyDestroy(&p);
}
