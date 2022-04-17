/** @file
  Interfejs funkcji parsujących wielomiany.

  @authors Paweł Olejnik <po417770@students.mimuw.edu.pl>
  @date 2021
*/

#ifndef POLY_PARSER_H
#define POLY_PARSER_H

#include "poly.h"

/**
 * Wczytuje jednomian postaci (wielomian, współczynnik).
 * @param[in] s : tablica typu `char`, z której czytany jest jednomian
 * @param[in] endptr : adres wskaźnika na zmienną typu `char`, któremu po
 * przeczytaniu jednomianu zostaje przypisany adres następnego znaku po
 * wczytanym jednomianie
 * @param[in] m : adres struktury `Mono`, której po poprawnym wczytaniu zostaje
 * przypisany wczytany jednomian
 * @return czy jednomian został wczytany poprawnie?
 */
bool ParseMono(char *s, char **endptr, Mono *m);

/**
 * Wczytuje wielomian, który jest współczynnikiem (wielomian stały).
 * @param[in] s : tablica typu `char`, z której czytany jest wielomian
 * @param[in] endptr : adres wskaźnika na zmienną typu `char`, któremu po
 * przeczytaniu wielomianu zostaje przypisany adres następnego znaku po
 * wczytanym wielomianie
 * @param[in] p : adres struktury `Poly`, której po poprawnym wczytaniu zostaje
 * przypisany wczytany wielomian
 * @return czy wielomian został wczytany poprawnie?
 */
bool ParseCoeff(char *s, char **endptr, Poly *p);

/**
 * Wczytuje wykładnik jednoimanu.
 * @param[in] s : tablica typu `char`, z której czytany jest wykładnik
 * @param[in] endptr : adres wskaźnika na zmienną typu `char`, któremu po
 * przeczytaniu wielomianu zostaje przypisany adres następnego znaku po
 * wczytanym wykładniku
 * @param[in] val : adres zmiennej typu `poly_exp_t`, której po poprawnym wczytaniu
 * zostaje przypisana wartość wczytanego wykładnika
 * @return czy wykładnik został wczytany poprawnie?
 */
bool ParsePolyExp(char *s, char **endptr, poly_exp_t *val);

/**
 * Wczytuje wielomian, będący w postaci sumy jednomianów.
 * @param[in] s : tablica typu `char`, z której czytany jest wielomian
 * @param[in] endptr : adres wskaźnika na zmienną typu `char`, któremu po
 * przeczytaniu wielomianu zostaje przypisany adres następnego znaku po
 * wczytanym wielomianie
 * @param[in] p : adres struktury `Poly`, której po poprawnym wczytaniu zostaje
 * przypisany wczytany wielomian
 * @return czy wielomian został wczytany poprawnie?
 */
bool ParseMonoSum(char *s, char **endptr, Poly *p);

/**
 * Wczytuje wielomian.
 * @param[in] s : tablica typu `char`, z której czytany jest wielomian
 * @param[in] endptr : adres wskaźnika na zmienną typu `char`, któremu po
 * przeczytaniu wielomianu zostaje przypisany adres następnego znaku po
 * wczytanym wielomianie
 * @param[in] p : adres struktury `Poly`, której po poprawnym wczytaniu zostaje
 * przypisany wczytany wielomian
 * @return czy wielomian został wczytany poprawnie?
 */
bool ParsePoly(char *s, char **endptr, Poly *p);

#endif /* POLY_PARSER_H */
