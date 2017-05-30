/** @file
 * Implementacja testów jednostkowych
 * Copyright 2008 Google Inc.
 * Copyright 2015 Tomasz Kociumaka
 * Copyright 2016, 2017 IPP team
 * Copyright 2017 Radosław Waśko
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>

#include "poly.h"

#include "cmocka.h"

/**
 * Oblicza ilość elementów w statycznej tablicy
 * @param[in] x : tablica
 * @return ilość elementów w x
 */
#define array_length(x) (sizeof(x) / sizeof((x)[0]))

/**
 * Punkt wejścia programu kalkulatora stosowego ze zmienioną nazwą (przystosowana do testowania -> podmieniamy właściwy main na ten z testami)
 * @param[in] argc : liczba argumentów z lini poleceń (nie używane)
 * @param[in] argv : tablica argumentów z lini poleceń (nie używane)
 */
extern int calculator_main(int argc, char *argv[]);

int mock_fprintf(FILE *const file, const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int mock_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * Pomocnicze bufory, do których piszą atrapy funkcji printf i fprintf oraz
 * pozycje zapisu w tych buforach. Pozycja zapisu wskazuje bajt o wartości 0.
 */

static char fprintf_buffer[256]; ///< bufor do testowania stderr
static char printf_buffer[256];  ///< bufor do testowania stdout
static int fprintf_position = 0; ///< wskaźnik, do którego miejsca już są zapisane dane w buforze stderr
static int printf_position = 0;  ///< wskaźnik, do którego miejsca już są zapisane dane w buforze stdout

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_fprintf(FILE *const file, const char *format, ...)
{
    int return_value;
    va_list args;

    assert_true(file == stderr);
    /* Poniższa asercja sprawdza też, czy fprintf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));
    return return_value;
}

/**
 * Atrapa funkcji printf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_printf(const char *format, ...)
{
    int return_value;
    va_list args;

    /* Poniższa asercja sprawdza też, czy printf_position jest nieujemne.
    W buforze musi zmieścić się kończący bajt o wartości 0. */
    assert_true((size_t)printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t)printf_position < sizeof(printf_buffer));
    return return_value;
}

/**
 *  Pomocniczy bufor, z którego korzystają atrapy funkcji operujących na stdin.
 */
static char input_stream_buffer[256];
static int input_stream_position = 0; ///< miejsce dokąd aplikacja już odczytała z stdin
static int input_stream_end = 0;      ///< pozycja, gdzie kończą się przygotowane w buforze dane

/**
 * Atrapa funkcji getchar używana do przechwycenia czytania z stdin.
 */
int mock_getchar()
{
    if (input_stream_position < input_stream_end)
        return input_stream_buffer[input_stream_position++];
    else
        return EOF;
}

/**
 * Funkcja wołana przed każdym testem korzystającym z stdout lub stderr.
 */
static int test_setup(void **state)
{
    (void)state;

    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));
    memset(printf_buffer, 0, sizeof(printf_buffer));
    printf_position = 0;
    fprintf_position = 0;

    /* Zwrócenie zera oznacza sukces. */
    return 0;
}

/**
 * Funkcja inicjująca dane wejściowe dla programu korzystającego ze stdin.
 */
static void init_input_stream(const char *str)
{
    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    input_stream_position = 0;
    input_stream_end = strlen(str);
    assert_true((size_t)input_stream_end < sizeof(input_stream_buffer));
    strcpy(input_stream_buffer, str);
}

/**
 * Test PolyCompose: p - wielomian zerowy, count = 0
 * Oczekiwany wynik: wielomian zerowy
 */
static void test_poly_zero_count_zero(void **state)
{
    (void)state;

    Poly p = PolyZero();
    Poly r = PolyCompose(&p, 0, NULL);

    //expect: 0
    assert_true(PolyIsZero(&r));

    PolyDestroy(&p);
    PolyDestroy(&r);
}

/**
 * Test PolyCompose: p - wielomian zerowy, count = 1, x[0] = wielomian stały
 * Oczekiwany wynik: wielomian zerowy
 */
static void test_poly_zero_coeff(void **state)
{
    (void)state;

    Poly p = PolyZero();
    Poly x[1] = {PolyFromCoeff(5)};
    Poly r = PolyCompose(&p, 1, x);

    //expect: 0
    assert_true(PolyIsZero(&r));

    for (unsigned int i = 0; i < array_length(x); ++i)
    {
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

/**
 * Test PolyCompose: p - wielomian stały, count = 0
 * Oczekiwany wynik: wielomian stały
 */
static void test_coeff_count_zero(void **state)
{
    (void)state;

    Poly p = PolyFromCoeff(7);
    Poly r = PolyCompose(&p, 0, NULL);

    //expect: 7
    assert_true(PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &p));

    PolyDestroy(&p);
    PolyDestroy(&r);
}

/**
 * Test PolyCompose: p - wielomian stały, count = 1, x[0] = wielomian stały
 * Oczekiwany wynik: oryginalny wielomian stały (nie ma zmiennych, pod które mielibyśmy podstawiać)
 */
static void test_coeff_coeff(void **state)
{
    (void)state;

    Poly p = PolyFromCoeff(7);
    Poly x[1] = {PolyFromCoeff(5)};
    Poly r = PolyCompose(&p, 1, x);

    //expect: 7
    assert_true(PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &p));

    for (unsigned int i = 0; i < array_length(x); ++i)
    {
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

/**
 * Tworzy nowy wielomian równy `x` (funkcja identycznościowa po zerowej zmiennej)
 * @return wielomian stopnia pierwszego = `x`
 */
static Poly make_x0()
{
    Poly one = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&one, 1);
    return PolyAddMonos(1, &m);
}

/**
 * Test PolyCompose: p - wielomian identycznościowy, count = 0
 * Oczekiwany wynik: wielomian zerowy (podstawiamy 0 pod wielomian identycznościowy)
 */
static void test_x0_count_zero(void **state)
{
    (void)state;

    Poly p = make_x0();
    Poly r = PolyCompose(&p, 0, NULL);

    //expect: 0
    assert_true(PolyIsZero(&r));

    PolyDestroy(&p);
    PolyDestroy(&r);
}

/**
 * Test PolyCompose: p - wielomian identycznościowy, count = 1, x[0] = wielomian stały
 * Oczekiwany wynik: wielomian stały równy podstawionemu
 */
static void test_x0_coeff(void **state)
{
    (void)state;

    Poly p = make_x0();
    Poly x[1] = {PolyFromCoeff(5)};
    Poly r = PolyCompose(&p, 1, x);

    //expect: 5
    assert_true(PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &x[0]));

    for (unsigned int i = 0; i < array_length(x); ++i)
    {
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

/**
 * Test PolyCompose: p - wielomian identycznościowy, count = 1, x[0] = wielomian identycznościowy
 * Oczekiwany wynik: wielomian identycznościowy (składamy identyczność ze sobą)
 */
static void test_x0_x0(void **state)
{
    (void)state;

    Poly p = make_x0();
    Poly x[1] = {make_x0()};
    Poly r = PolyCompose(&p, 1, x);

    //expect: x
    assert_true(!PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &p));

    for (unsigned int i = 0; i < array_length(x); ++i)
    {
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

/**
 * Test parsowania polecenia COMPOSE: brak parametru
 * Oczekiwany wynik: Zły parametr (WRONG COUNT)
 */
static void test_no_param(void **state)
{
    (void)state;

    init_input_stream("COMPOSE ");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia COMPOSE: parametr minimalny = 0
 * Oczekiwany wynik: Wszystko ok (brak błędów, nic się nie wypisuje)
 */
static void test_min_count(void **state)
{
    (void)state;

    init_input_stream("0\nCOMPOSE 0\n");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "");
}

/**
 * Test parsowania polecenia COMPOSE: maksymalna wartość typu unsigned
 * Oczekiwany wynik: Brakuje elementów na stosie (STACK UNDERFLOW)
 */
static void test_max_count(void **state)
{
    (void)state;

    init_input_stream("COMPOSE 4294967295\n");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 STACK UNDERFLOW\n");
}

/**
 * Test parsowania polecenia COMPOSE: parametr ujemny
 * Oczekiwany wynik: Zły parametr (WRONG COUNT)
 */
static void test_negative_count(void **state)
{
    (void)state;

    init_input_stream("COMPOSE -1\n");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia COMPOSE: parametr przekraczający maksymalną wartość o 1
 * Oczekiwany wynik: Zły parametr (WRONG COUNT)
 */
static void test_slightly_too_big_count(void **state)
{
    (void)state;

    init_input_stream("COMPOSE 4294967296\n");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia COMPOSE: parametr znacznie przekraczający dopuszczalną wartość
 * Oczekiwany wynik: Zły parametr (WRONG COUNT)
 */
static void test_far_too_big_count(void **state)
{
    (void)state;

    init_input_stream("COMPOSE 99999999999999999999999\n");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia COMPOSE: litery w miejscu gdzie powinna być liczba
 * Oczekiwany wynik: Zły parametr (WRONG COUNT)
 */
static void test_letters_as_count(void **state)
{
    (void)state;

    init_input_stream("COMPOSE fooBAR\n");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Test parsowania polecenia COMPOSE: litery, ale zaczynające się od cyfry
 * Oczekiwany wynik: Zły parametr (WRONG COUNT)
 */
static void test_mixed_letters_and_digits_as_count(void **state)
{
    (void)state;

    init_input_stream("COMPOSE 1Foo2Bar\n");
    assert_int_equal(calculator_main(0, NULL), 0);
    assert_string_equal(printf_buffer, "");
    assert_string_equal(fprintf_buffer, "ERROR 1 WRONG COUNT\n");
}

/**
 * Punkt wejścia naszych testów: przygotowujemy 2 grupy testów i uruchamiamy je, korzystając z funkcji dostarczonych przez bibliotekę cmocka
 */
int main(void)
{
    const struct CMUnitTest compose_function_tests[] = {
        cmocka_unit_test(test_poly_zero_count_zero),
        cmocka_unit_test(test_poly_zero_coeff),
        cmocka_unit_test(test_coeff_count_zero),
        cmocka_unit_test(test_coeff_coeff),
        cmocka_unit_test(test_x0_count_zero),
        cmocka_unit_test(test_x0_coeff),
        cmocka_unit_test(test_x0_x0),
    };

    const struct CMUnitTest compose_parse_tests[] = {
        cmocka_unit_test_setup(test_no_param, test_setup),
        cmocka_unit_test_setup(test_min_count, test_setup),
        cmocka_unit_test_setup(test_max_count, test_setup),
        cmocka_unit_test_setup(test_negative_count, test_setup),
        cmocka_unit_test_setup(test_slightly_too_big_count, test_setup),
        cmocka_unit_test_setup(test_far_too_big_count, test_setup),
        cmocka_unit_test_setup(test_letters_as_count, test_setup),
        cmocka_unit_test_setup(test_mixed_letters_and_digits_as_count, test_setup),
    };

    return cmocka_run_group_tests(compose_function_tests, NULL, NULL) + cmocka_run_group_tests(compose_parse_tests, NULL, NULL);
}
