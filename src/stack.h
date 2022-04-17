/** @file
  Interfejs stosu przechowującego wielomiany

  @authors Paweł Olejnik <po417770@students.mimuw.edu.pl>
  @date 2021
*/

#ifndef POLY_STACK_H
#define POLY_STACK_H

#include "poly.h"

/**
 * Struktura reprezentująca stos wielomianów
 */
typedef struct PolyStack {
    size_t size;    ///< rozmiar zaalokowanej tabilcy `arr`
    size_t top;     ///< indeks w tablicy `arr` szczytu stosu
    Poly *arr;      ///< tablica przechowująca wielomiany na stosie
} PolyStack;

/**
 * Inicjalizuje pusty stos wielomianów.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void InitStack(PolyStack *s);

/**
 * Sprawdza, czy stos jest pusty.
 * @param[in] s : wskaźnik na stos wielomianów
 * @return stos `s` jest pusty
 */
bool IsEmpty(PolyStack *s);

/**
 * Zwraca liczbę wielomianów na stosie.
 * @param[in] s : wskaźnik na stos wielomianów
 * @return liczba wielomianów na stosie
 */
size_t StackSize(PolyStack *s);

/**
 * Umieszcza na stosie wielomian.
 * @param[in] s : wskaźnik na stos wielomianów
 * @param[in] p : wielomian
 */
void Push(PolyStack *s, Poly p);

/**
 * Zwraca wskaźnik do wielomianu na szczycie stosu.
 * @param[in] s : wskaźnik na stos wielomianów
 * @return wskaźnik na wielomian na szczycie stosu
 */
Poly* Top(PolyStack *s);

/**
 * Usuwa zwielomian ze szczytu stosu i zwalnia go z pamięci.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Pop(PolyStack *s);

/**
 * Usuwa wszystkie wielomiany ze stosu i zwalnia uprzednio zaalokowaną pamięć.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void FreeStack(PolyStack *s);

/**
 * Wyświetla na standardowe wyjście wielomian znajdujący się na szczycie stosu.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void PrintTop(PolyStack *s);

/**
 * Umieszcza na szczycie stosu wielomian tożsamościowo równy zero.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Zero(PolyStack *s);

/**
 * Wyświetla na standardowe wyjście `1` jeśli wielomian na szczycie stosu
 * jest współczynnikiem (jest wielomianem stałym), w przeciwnym przypadku
 * wyświetla `0`.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void IsCoeff(PolyStack *s);

/**
 * Wyświetla na standardowe wyjście `1` jeśli wielomian na szczycie stosu
 * jest tożsamościowo równy zero, w przeciwnym przypadku wyświetla `0`.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void IsZero(PolyStack *s);

/**
 * Umieszcza na stosie kopię wielomianu znajdującego się na szczycie.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Clone(PolyStack *s);

/**
 * Usuwa ze stosu dwa wielomiany znajdujące się na szczycie, oblicza wielomian
 * będący ich sumą i umieszcza go na stosie.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Add(PolyStack *s);

/**
 * Usuwa ze stosu dwa wielomiany znajdujące się na szczycie, oblicza wielomian
 * będący ich iloczynem i umieszcza go na stosie.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Mul(PolyStack *s);

/**
 * Neguje wielomian znajdujący się na szczycie stosu (mnoży go przez @f$-1@f$).
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Neg(PolyStack *s);

/**
 * Oblicza wartość wielomianu @f$p-q@f$ będącego różnicą wielomianu @f$p@f$
 * znajdującego się na szczycie stosu i wielomianu @f$q@f$ znajdującego się pod
 * nim, usuwa je ze stosu i umieszcza na stosie obliczony wielomian @f$p-q@f$.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Sub(PolyStack *s);

/**
 * Wyświetla na standardowe wyjście `1` jeśli wielomian na szczycie stosu
 * jest równy wielomianowi znajdującemu się na stosie pod nim, w przeciwnym
 * przypadku wyświetla `0`.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void IsEq(PolyStack *s);

/**
 * Wyświetla na standardowe wyjście stopień wielomianu znajdującego się na
 * szczycie stosu.
 * @param[in] s : wskaźnik na stos wielomianów
 */
void Deg(PolyStack *s);

/**
 * Wyświetla na standardowe wyjście stopień wielomianu znajdującego się na
 * szczycie stosu względem zmiennej o indeksie `var_idx`.
 * @param[in] s : wskaźnik na stos wielomianów
 * @param[in] var_idx : indeks zmiennej, względem której obliczany jest stopień
 */
void DegBy(PolyStack *s, size_t var_idx);

/**
 * Oblicza wielomian @f$p(x)@f$, będącego wartością wielomianu @f$p@f$
 * znajdującego się na szczycie stosu w punkcie @f$x@f$.
 * @param[in] s : wskaźnik na stos wielomianów
 * @param[in] x : punkt, w którym obliczana jest wartość wielomianu
 */
void At(PolyStack *s, poly_coeff_t x);

/**
 * Zdejmuje z wierzchołka sostu wielomian @f$p@f$, a następnie kolejno
 * @f$k@f$ wielomianów @f$q_{k-1}, \ldots, q_0@f$ i umieszcza na stosie wynik
 * operacji złożenia @f$p(q_0, \ldots, q_{k-1})@f$.
 * @param[in] s : wskaźnik na stos wielomianów
 * @param[in] k : liczba wielomianów @f$q_i@f$
 */ 
void Compose(PolyStack *s, size_t k);

#endif /* POLY_STACK_H */
