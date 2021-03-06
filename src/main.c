/** @file
   Punkt wejścia programu kalkulatora stosowego

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include "parser.h"
#include "stack_calc.h"

#include "utils.h"

/**
 * Punkt wejścia programu kalkulatora stosowego.
 * @return wynik działania (zawsze 0)
 */
int main(int argc, char *argv[])
{
    (void)argc;//nie używamy argumentów z linii poleceń, więc nie chemy warningów o nieużywanym argumencie
    (void)argv;

    PolyStack stack = PolyStackEmpty();
    Initialize();
    while (HasMoreLines())
    {
        ParseLine(&stack);
    }

    PolyStackDestroy(&stack);
    return 0;
}
