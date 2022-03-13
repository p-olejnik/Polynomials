/** @file
  Główne funkcje odpowiadające za wykonywanie obliczeń na wielomianach

  @authors Paweł Olejnik <po417770@students.mimuw.edu.pl>
  @date 2021
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "poly.h"
#include "poly_stack.h"
#include "poly_parser.h"

/**
 * Wypisuje na standardowe wyjście błędów komunikat, że na stosie jest za mało
 * wielomianów, aby wykonać polecenie. Wypisuje, w której linii miał miejsce
 * błąd.
 * @param[in] line_number : numer linii, w której miał miejsce błą∂
 */
static void UnderflowError(size_t line_number) {
    fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", line_number);
}

/**
 * Wypisuje na standardowe wyjście błędów komunikat, że wystąpił błąd podczas
 * parsowania wielomianu. Wypisuje, w której linii miał miejsce błąd.
 * @param[in] line_number : numer linii, w której miał miejsce błą∂
 */
static void WrongPolyError(size_t line_number) {
    fprintf(stderr, "ERROR %zu WRONG POLY\n", line_number);
}

/**
 * Wypisuje na standardowe wyjście błędów komunikat, że została podana
 * niepoprawna nazwa polecnia. Wypisuje, w której linii miał miejsce błąd.
 * @param[in] line_number : numer linii, w której miał miejsce błą∂
 */
static void WrongCommandError(size_t line_number) {
    fprintf(stderr, "ERROR %zu WRONG COMMAND\n", line_number);
}

/**
 * Wypisuje na standardowe wyjście błędów komunikat, że w poleceniu `DEG_BY`
 * nie podano argumentu lub jest on niepoprawny.
 * Wypisuje, w której linii miał miejsce błąd.
 * @param[in] line_number : numer linii, w której miał miejsce błą∂
 */
static void DegByWrongVarError(size_t line_number) {
    fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", line_number);
}

/**
 * Wypisuje na standardowe wyjście błędów komunikat, że w poleceniu `AT`
 * nie podano argumentu lub jest on niepoprawny.
 * Wypisuje, w której linii miał miejsce błąd.
 * @param[in] line_number : numer linii, w której miał miejsce błą∂
 */
static void AtWrongValueError(size_t line_number) {
    fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", line_number);
}

/**
 * Wypisuje na standardowe wyjście błędów komunikat, że w poleceniu `COMPOSE`
 * nie podano parametru lub jest on niepoprawny.
 * Wypisuje, w której linii miał miejsce błąd.
 * @param[in] line_number : numer linii, w której miał miejsce błą∂
 */
static void ComposeWrongParameterError(size_t line_number) {
    fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", line_number);
}

/**
 * Sprawdza, czy linia powinna zostać zignorowana (jest pusta lub zaczyna się
 * od znaku '#').
 * @param[in] line : tablica znaków reprezentująca linię
 * @param[in] len : liczba znaków w linii
 * @return : czy linia powinna być zignorowana?
 */
static bool LineIsIgnored(char *line, size_t len) {
    return len == 0 || line[0] == '#';
}

/**
 * Sprawdza, czy linia reprezentuje polecenie (czy pierwszy znak jest literą
 * alfabetu angielskiego).
 * @param[in] line : tablica znaków reprezentująca linię
 * @return : czy linia reprezentuje polecenie?
 */
static bool LineIsCommand(char *line) {
    return isalpha(line[0]) != 0;
}

/**
 * Wczytuje argument polecenia `DEG_BY`.
 * @param[in] s : tablica znaków reprezentująca argument
 * @param[in] endptr : adres wskaźnika, któremu po wczytaniu argumentu zostaje
 * przypisany adres następnego znaku w tablicy po wczytanym argumencie
 * @param[in] var_idx : adres zmiennej typu `size_t`, któremu po poprawnym 
 * wczytaniu argumentu zostaje przypisana jego wartość.
 * @return czy argument został wczytany poprawnie?
 */
static bool ParseDegByVar(char *s, char **endptr, size_t *var_idx) {
    if (isdigit(*s) == 0 && *s != '0') return false;

    errno = 0;
    *var_idx = strtoul(s, endptr, 10);
    if (*endptr == s || errno == ERANGE) return false;

    return true;
}

/**
 * Wczytuje argument polecenia `AT`.
 * @param[in] s : tablica znaków reprezentująca argument
 * @param[in] endptr : adres wskaźnika, któremu po wczytaniu argumentu zostaje
 * przypisany adres następnego znaku w tablicy po wczytanym argumencie
 * @param[in] x : adres zmiennej typu `poly_coeff_t`, któremu po poprawnym 
 * wczytaniu argumentu zostaje przypisana jego wartość.
 * @return czy argument został wczytany poprawnie?
 */
static bool ParseAtVal(char *s, char **endptr, poly_coeff_t *x) {
    if (isdigit(*s) == 0 && *s != '0' && *s != '-') return false;

    errno = 0;
    *x = strtol(s, endptr, 10);
    if (*endptr == s || errno == ERANGE) return false;

    return true;
}

/**
 * Wczytuje argument polecenia `COMPOSE`.
 * @param[in] s : tablica znaków reprezentująca argument
 * @param[in] endptr : adres wskaźnika, któremu po wczytaniu argumentu zostaje
 * przypisany adres następnego znaku w tablicy po wczytanym argumencie
 * @param[in] k : adres zmiennej typu `size_t`, któremu po poprawnym 
 * wczytaniu argumentu zostaje przypisana jego wartość.
 * @return czy argument został wczytany poprawnie?
 */
static bool ParseComposeParameter(char *s, char **endptr, size_t *k) {
    if (isdigit(*s) == 0 && *s != '0') return false;

    errno = 0;
    *k = strtoull(s, endptr, 10);
    if (*endptr == s || errno == ERANGE) return false;
    return true;
}

/**
 * Wczytuje linię, jeśli zawiera ona jedną ze zdefiniowanych instrukcji,
 * wykonuje ją.
 * @param[in] line : tablica znaków reprezentująca polecenie
 * @param[in] s : stos wielomianów
 * @param line_number : numer aktualnej wczytanej linii
 */
static void ParseCommand(char *line, PolyStack *s, size_t line_number) {
    if (strcmp(line, "ZERO") == 0) {
        Zero(s);
        return;
    }

    if (strcmp(line, "IS_COEFF") == 0) {
        if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            IsCoeff(s);
        }
        return;
    }

    if (strcmp(line, "IS_ZERO") == 0) {
        if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            IsZero(s);
        }
        return;
    }

    if (strcmp(line, "CLONE") == 0) {
        if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            Clone(s);
        }
        return;
    }

    if (strcmp(line, "ADD") == 0) {
        if (StackSize(s) < 2) {
            UnderflowError(line_number);
        } else {
            Add(s);
        }
        return;
    }

    if (strcmp(line, "MUL") == 0) {
        if (StackSize(s) < 2) {
            UnderflowError(line_number);
        } else {
            Mul(s);
        }
        return;
    }

    if (strcmp(line, "NEG") == 0) {
        if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            Neg(s);
        }
        return;
    }

    if (strcmp(line, "SUB") == 0) {
        if (StackSize(s) < 2) {
            UnderflowError(line_number);
        } else {
            Sub(s);
        }
        return;
    }

    if (strcmp(line, "IS_EQ") == 0) {
        if (StackSize(s) < 2) {
            UnderflowError(line_number);
        } else {
            IsEq(s);
        }
        return;
    }

    if (strcmp(line, "DEG") == 0) {
        if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            Deg(s);
        }
        return;
    }

    if (strcmp(line, "PRINT") == 0) {
        if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            PrintTop(s);
        }
        return;
    }

    if (strcmp(line, "POP") == 0) {
        if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            Pop(s);
        };
        return;
    }

    if (strncmp(line, "DEG_BY ", 7) == 0) {
        size_t var_idx;
        char *endptr;
        if (!ParseDegByVar(line+7, &endptr, &var_idx) || *endptr != '\0') {
            DegByWrongVarError(line_number);
        } else if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            DegBy(s, var_idx);
        }
        return;
    }

    if (strncmp(line, "AT ", 3) == 0) {
        poly_coeff_t x;
        char *endptr;
        if (!ParseAtVal(line + 3, &endptr, &x) || *endptr != '\0') {
            AtWrongValueError(line_number);
        } else if (IsEmpty(s)) {
            UnderflowError(line_number);
        } else {
            At(s, x);
        }
        return;
    }

    if (strncmp(line, "COMPOSE ", 8) == 0) {
        size_t k;
        char *endptr;
        if (!ParseComposeParameter(line + 8, &endptr, &k) || *endptr != '\0') {
            ComposeWrongParameterError(line_number);
        } else if (StackSize(s) <= k) {
            UnderflowError(line_number);
        } else {
            Compose(s, k);
        }
        return;
    }

    WrongCommandError(line_number);
}

/**
 * Sprawdza, czy linia zawiera niedozwolone dla polecenia znaki.
 * @param[in] s : tablica znaków reprezentująca linię
 * @param[in] len : liczba znaków w tablicy `s`
 * @return czy linia zawiera znaki niedozwolone dla polecenia?
 */
static bool CommandHasInvalidChars(char *s, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (isascii(s[i]) == 0 || s[i] == '\0') return true;
    }
    return false;
}

/**
 * Sprawdza, czy tablica znaków zawiera niedozwolone dla wielomianu znaki.
 * @param[in] s : tablica typu `char` reprezentująca wielomian
 * @param[in] len : lizcba znaków w tablicy `s`
 * @return czy linia zawiera znaki niedozwolone w wielomianie?
 */
static bool PolyHasInvalidChars(char *s, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (isdigit(s[i]) == 0 &&
            !(s[i] == '(' || s[i] == ')' || s[i] == ',' ||
                s[i] == '+' || s[i] == '-' || s[i] == '0')
        ) {
            return true;
        }
    }
    return false;
}

/**
 * Wczytuje linię, która może składać się z wielomianu lub polecenia.
 * @param[in] line : tablica typu `char` reprezentująca linię
 * @param[in] stack : stos wielomianów
 * @param[in] line_number : numer wczytanej linii
 * @param[in] len : liczba znaków w tablicy `line`
 */
static void ParseLine(char *line, PolyStack *stack, size_t line_number, size_t len) {
    if (LineIsIgnored(line, len)) {
        return;
    }

    if (LineIsCommand(line)) {
        if (CommandHasInvalidChars(line, len)) {
            WrongCommandError(line_number);
        } else {
            ParseCommand(line, stack, line_number);
        }
        return;
    }

    if (PolyHasInvalidChars(line, len)) {
        WrongPolyError(line_number);
    } else {
        char *endptr;
        Poly p;

        if (!ParsePoly(line, &endptr, &p)) {
            WrongPolyError(line_number);
        } else if (*endptr != '\0') {
            PolyDestroy(&p);
            WrongPolyError(line_number);
        } else {
            Push(stack, p);
        }
    }
}

/**
 * Funkcja `main` wykonuje program: czyta polecenia ze standardowego wejścia i
 * wypisuje wyniki operacji na wielomianach.
 */
int main(void) {
    PolyStack stack;
    InitStack(&stack);

    char *line = NULL;
    size_t line_len = 0;
    size_t characters;
    size_t line_number = 1;

    while ((characters = getline(&line, &line_len, stdin)) != (size_t)EOF) {
        if (line == NULL) return 1;
        if (line[characters - 1] == '\n') line[--characters] = '\0';
        ParseLine(line, &stack, line_number++, characters);
    }

    free(line);
    FreeStack(&stack);
    return 0;
}
