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

#define array_length(x) (sizeof(x) / sizeof((x)[0]))

extern int calculator_main(int argc, char *argv[]);

int mock_fprintf(FILE* const file, const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int mock_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * Pomocnicze bufory, do których piszą atrapy funkcji printf i fprintf oraz
 * pozycje zapisu w tych buforach. Pozycja zapisu wskazuje bajt o wartości 0.
 */
static char fprintf_buffer[256];
static char printf_buffer[256];
static int fprintf_position = 0;
static int printf_position = 0;

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_fprintf(FILE* const file, const char *format, ...) {
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
int mock_printf(const char *format, ...) {
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
static int input_stream_position = 0;
static int input_stream_end = 0;
int read_char_count;

/**
 * Atrapa funkcji getchar używana do przechwycenia czytania z stdin.
 */
int mock_getchar() {
    if (input_stream_position < input_stream_end)
        return input_stream_buffer[input_stream_position++];
    else
        return EOF;
}

/**
 * Funkcja wołana przed każdym testem korzystającym z stdout lub stderr.
 */
static int test_setup(void **state) {
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
static void init_input_stream(const char *str) {
    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    input_stream_position = 0;
    input_stream_end = strlen(str);
    assert_true((size_t)input_stream_end < sizeof(input_stream_buffer));
    strcpy(input_stream_buffer, str);
}


static void test_poly_zero_count_zero(void **state) {
    (void)state;

    Poly p = PolyZero();
    Poly r = PolyCompose(&p, 0, NULL);

    //expect: 0
    assert_true(PolyIsZero(&r));
    
    PolyDestroy(&p);
    PolyDestroy(&r);
}

static void test_poly_zero_coeff(void **state) {
    (void)state;

    Poly p = PolyZero();
    Poly x[1] = { PolyFromCoeff(5) };
    Poly r = PolyCompose(&p, 1, x);
    
    //expect: 0
    assert_true(PolyIsZero(&r));
    
    for(unsigned int i = 0; i < array_length(x); ++i){
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

static void test_coeff_count_zero(void **state) {
    (void)state;

    Poly p = PolyFromCoeff(7);
    Poly r = PolyCompose(&p, 0, NULL);
    
    //expect: 7
    assert_true(PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &p));
    
    PolyDestroy(&p);
    PolyDestroy(&r);
}

static void test_coeff_coeff(void **state) {
    (void)state;

    Poly p = PolyFromCoeff(7);
    Poly x[1] = { PolyFromCoeff(5) };
    Poly r = PolyCompose(&p, 1, x);
    
    //expect: 7
    assert_true(PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &p));
    
    for(unsigned int i = 0; i < array_length(x); ++i){
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

static Poly makex0(){
    Poly one = PolyFromCoeff(1);
    Mono m = MonoFromPoly(&one, 1);
    return PolyAddMonos(1, &m);
}

static void test_x0_count_zero(void **state) {
    (void)state;

    Poly p = makex0();
    Poly r = PolyCompose(&p, 0, NULL);
    
    //expect: 0
    assert_true(PolyIsZero(&r));
    
    PolyDestroy(&p);
    PolyDestroy(&r);
}

static void test_x0_coeff(void **state) {
    (void)state;

    Poly p = makex0();
    Poly x[1] = { PolyFromCoeff(5) };
    Poly r = PolyCompose(&p, 1, x);
    
    //expect: 5
    assert_true(PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &x[0]));
    
    for(unsigned int i = 0; i < array_length(x); ++i){
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

static void test_x0_x0(void **state) {
    (void)state;

    Poly p = makex0();
    Poly x[1] = { makex0() };
    Poly r = PolyCompose(&p, 1, x);
    
    //expect: x
    assert_true(!PolyIsCoeff(&r));
    assert_true(PolyIsEq(&r, &p));
    
    for(unsigned int i = 0; i < array_length(x); ++i){
        PolyDestroy(&x[i]);
    }
    PolyDestroy(&p);
    PolyDestroy(&r);
}

int main(void) {
    const struct CMUnitTest compose_function_tests[] = {
        cmocka_unit_test(test_poly_zero_count_zero),
        cmocka_unit_test(test_poly_zero_coeff),
        cmocka_unit_test(test_coeff_count_zero),
        cmocka_unit_test(test_coeff_coeff),
        cmocka_unit_test(test_x0_count_zero),
        cmocka_unit_test(test_x0_coeff),
        cmocka_unit_test(test_x0_x0),
        //cmocka_unit_test_setup(test_perform_operation_first_arg_not_integer, test_setup),
    };
    const struct CMUnitTest compose_parse_tests[] = {
        //cmocka_unit_test(test_add),
        //cmocka_unit_test_setup(test_perform_operation_first_arg_not_integer, test_setup),
    };

    return cmocka_run_group_tests(compose_function_tests, NULL, NULL)
        +  cmocka_run_group_tests(compose_parse_tests, NULL, NULL);
}
