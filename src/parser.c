/** @file
  Implementacja funkcji parsujących wielomiany.

  @authors Paweł Olejnik <po417770@students.mimuw.edu.pl>
  @date 2021
*/

#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "parser.h"

/**
 * Sprawdza poprawną alokację pamięci.
 * Jeśli pamięć nie została poprawnie zaalokowana, kończy program z kodem 1.
 * @param p : wskaźnik na zaalokowaną pamięć
 */
static void CheckPtr(const void *p) {
    if (p == NULL) exit(1);
}

bool ParseCoeff(char *s, char **endptr, Poly *p) {
    if (s[0] == '+') return false;

    errno = 0;
    poly_coeff_t coeff = strtol(s, endptr, 10);
    if (*endptr == s || errno == ERANGE) return false;

    *p = PolyFromCoeff(coeff);
    return true;
}

bool ParsePolyExp(char *s, char **endptr, poly_exp_t *val) {
    if (isdigit(*s) == 0 && *s != '0') return false;

    errno = 0;
    *val = strtoul(s, endptr, 10);
    if (*endptr == s || errno == ERANGE || *val < 0 || *val > INT_MAX) {
        return false;
    }

    return true;
}

bool ParseMonoSum(char *s, char **endptr, Poly *p) {
    size_t monos_size = 0;
    size_t monos_count = -1;
    Mono *monos = (Mono*) calloc(monos_size, sizeof(Mono));
    CheckPtr(monos);

    do {
        Mono new_mono;
        if (!ParseMono(s, endptr, &new_mono)) {
            DestroyMonoArray(monos, monos_count+1);
            return false;
        }
        s = *endptr + 1;

        if (++monos_count == monos_size) {
            monos_size = 1 + 2 * monos_size;
            Mono *tmp_monos = (Mono*) realloc(monos, monos_size * sizeof(Mono));
            CheckPtr(tmp_monos);
            monos = tmp_monos;
        }

        monos[monos_count] = new_mono;

    } while (**endptr == '+');

    *p = PolyAddMonos(++monos_count, monos);
    free(monos);
    return true;
}

bool ParsePoly(char *s, char **endptr, Poly *p) {
    if (s[0] != '(') {
        return ParseCoeff(s, endptr, p);
    }

    return ParseMonoSum(s, endptr, p);
}

bool ParseMono(char *s, char **endptr, Mono *m) {
    if (s[0] != '(') {
        return false;
    }

    Poly coeff_poly;
    if (!ParsePoly(s+1, endptr, &coeff_poly)) {
        return false;
    }

    if (**endptr != ',') {
        PolyDestroy(&coeff_poly);
        return false;
    }

    s = *endptr + 1;
    poly_exp_t exp;
    if (!ParsePolyExp(s, endptr, &exp) || **endptr != ')') {
        PolyDestroy(&coeff_poly);
        return false;
    }

    if (PolyIsZero(&coeff_poly) && exp != 0) {
        exp = 0;
    }
    
    *m = MonoFromPoly(&coeff_poly, exp);
    (*endptr)++;
    return true;
}
