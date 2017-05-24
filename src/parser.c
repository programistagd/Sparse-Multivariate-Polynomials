/** @file
   Implementacja parsera poleceń

   @author Radosław Waśko <rw386491@students.mimuw.edu.pl>
   @copyright Uniwersytet Warszawski
   @date 2017-05-20
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "parser.h"
#include "stack_calc.h"
#include "poly.h"
#include "dynamics.h"
#include "errors.h"

static bool initialized = false; ///< czy została wykonana funkcja Initialize przygotowująca parser
static char next = ' ';          ///< najbliższy znak przychodzący z stdin
static int lineno;               ///< numer aktualnie przetwarzanego wiersza
static int columnno;             ///< numer aktualnie przetwarzanej kolumny

unsigned int GetCurrentLine()
{
    return lineno + 1;
}

unsigned int GetCurrentColumn()
{
    return columnno + 1;
}

bool EndOfStream()
{
    assert(initialized);
    return PeekChar() == EOF;
}

void PopChar()
{
    if (!EndOfStream())
    {
        next = getchar();
        columnno += 1;
    }
}

char PeekChar()
{
    assert(initialized);
    return next;
}

char GetChar()
{
    char c = PeekChar();
    PopChar();
    return c;
}

void Initialize()
{
    assert(!initialized);
    initialized = true;
    lineno = 0;
    columnno = 0;
    next = getchar();
}

bool HasMoreLines()
{
    return !EndOfStream();
}

bool IsLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool IsCommandLetter(char c)
{
    return IsLetter(c) || c == '_';
}

bool IsDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool IsEnding(char c)
{
    return c == '\n' || c == EOF;
}

void ConsumeLine()
{
    while (!IsEnding(PeekChar()))
    {
        PopChar();
    }
    PopChar(); //usuwamy \n
}

ParsingResult ReadCoeff()
{
    poly_coeff_t x = 0;
    bool adding = true;
    int digits = 0;

    if (PeekChar() == '-')
    {
        PopChar();
        adding = false;
    }

    while (IsDigit(PeekChar()))
    {
        poly_coeff_t d = PeekChar() - '0';

        if (x > LONG_MAX / 10 || x < LONG_MIN / 10)
        {
            return ParsingError();
        }
        x *= 10;
        if (adding)
        {
            if (d > LONG_MAX - x)
            {
                return ParsingError();
            }
            else
            {
                x += d;
            }
        }
        else
        {

            if (-d < LONG_MIN - x)
            {
                return ParsingError();
            }
            else
            {
                x -= d;
            }
        }

        PopChar();
        digits++;
    }

    if (digits == 0)
    {
        return ParsingError();
    }

    return PackCoeff(x);
}

ParsingResult ReadExp()
{
    poly_exp_t x = 0;
    int digits = 0;

    while (IsDigit(PeekChar()))
    {
        poly_exp_t d = PeekChar() - '0';
        if (x > INT_MAX / 10)
        {
            return ParsingError();
        }
        x *= 10;

        if (d > INT_MAX - x)
        {
            return ParsingError();
        }
        else
        {
            x += d;
        }

        PopChar();
        digits++;
    }

    if (digits == 0)
    {
        return ParsingError();
    }

    return PackExp(x);
}

ParsingResult ReadDeg()
{
    unsigned int x = 0;
    int digits = 0;

    while (IsDigit(PeekChar()))
    {
        unsigned int d = PeekChar() - '0';
        if (x > UINT_MAX / 10)
        {
            return ParsingError();
        }
        x *= 10;
        if (x + d < x)
        { //jeśli suma jest mniejsza od samego wyrazu a dodajemy dodatnie to znaczy że był overflow i się zawinęło, czyli za duża liczba
            return ParsingError();
        }
        x += d;

        PopChar();
        digits++;
    }

    if (digits == 0)
    {
        return ParsingError();
    }

    return PackDeg(x);
}

ParsingResult ReadPoly()
{
    if (PeekChar() == '(')
    { //jednomian lub suma jednomianów
        Monos monos = MonosEmpty();

        do
        {
            if (PeekChar() == '+')
            {
                PopChar();
            }

            if (PeekChar() != '(')
            {
                MonosDestroy(&monos);
                return ParsingError();
            }
            PopChar();

            Mono inner;
            ParsingResult res_poly = ReadPoly();
            if (IsError(res_poly))
            {
                MonosDestroy(&monos);
                return res_poly;
            }

            inner.p = UnpackPoly(res_poly);

            if (PeekChar() != ',')
            {
                PolyDestroy(&inner.p);
                MonosDestroy(&monos);
                return ParsingError();
            }
            PopChar();

            ParsingResult res_exp = ReadExp();
            if (IsError(res_exp))
            {
                PolyDestroy(&inner.p);
                MonosDestroy(&monos);
                return res_exp;
            }

            inner.exp = UnpackExp(res_exp);

            MonosAppend(&monos, inner); //dodajemy wielomian do "listy"

            if (PeekChar() != ')')
            {
                MonosDestroy(&monos);
                return ParsingError();
            }
            PopChar();

        } while (PeekChar() == '+');

        return PackPoly(MonosMergeIntoPoly(&monos));
    }
    else
    {
        ParsingResult res_coeff = ReadCoeff();

        if (IsError(res_coeff))
        {
            return res_coeff;
        }

        Poly p = PolyFromCoeff(UnpackCoeff(res_coeff));

        return PackPoly(p);
    }
}

void PrintError(const char *text)
{
    fprintf(stderr, "ERROR %d %s\n", lineno + 1, text);
}

void ParseCommand(PolyStack *stack)
{
    const unsigned int longest_command_name = 8;

    String s = StringEmpty();
    while (IsCommandLetter(PeekChar()) && StringLength(&s) < longest_command_name)
    {
        StringAppend(&s, GetChar());
    }

    if (StringCmp(&s, "DEG_BY") || StringCmp(&s, "AT"))
    {
        if (PeekChar() != ' ')
        {
            PrintError("WRONG COMMAND");
            ConsumeLine();
            StringFree(&s);
            return;
        }
        PopChar(); //zjadamy spację

        if (StringCmp(&s, "DEG_BY"))
        {
            ParsingResult res_idx = ReadDeg();

            if (IsError(res_idx) || !IsEnding(PeekChar()))
            {
                PrintError("WRONG VARIABLE");
                ConsumeLine();
                StringFree(&s);
                return;
            }
            PopChar(); //zjadamy koniec linii

            unsigned int idx = UnpackDeg(res_idx);
            CalcDegBy(stack, idx);
        }
        else if (StringCmp(&s, "AT"))
        {
            ParsingResult res_x = ReadCoeff();

            if (IsError(res_x) || !IsEnding(PeekChar()))
            {
                PrintError("WRONG VALUE");
                ConsumeLine();
                StringFree(&s);
                return;
            }
            PopChar(); //zjadamy koniec linii

            poly_coeff_t x = UnpackCoeff(res_x);
            CalcAt(stack, x);
        }
    }
    else
    {
        if (!IsEnding(PeekChar()))
        {
            PrintError("WRONG COMMAND");
            ConsumeLine();
            StringFree(&s);
            return;
        }
        PopChar(); //zjadamy koniec linii

        if (StringCmp(&s, "ZERO"))
        {
            CalcZero(stack);
        }
        else if (StringCmp(&s, "IS_COEFF"))
        {
            CalcIsCoeff(stack);
        }
        else if (StringCmp(&s, "IS_ZERO"))
        {
            CalcIsZero(stack);
        }
        else if (StringCmp(&s, "CLONE"))
        {
            CalcClone(stack);
        }
        else if (StringCmp(&s, "ADD"))
        {
            CalcAdd(stack);
        }
        else if (StringCmp(&s, "MUL"))
        {
            CalcMul(stack);
        }
        else if (StringCmp(&s, "NEG"))
        {
            CalcNeg(stack);
        }
        else if (StringCmp(&s, "SUB"))
        {
            CalcSub(stack);
        }
        else if (StringCmp(&s, "IS_EQ"))
        {
            CalcIsEq(stack);
        }
        else if (StringCmp(&s, "DEG"))
        {
            CalcDeg(stack);
        }
        else if (StringCmp(&s, "PRINT"))
        {
            CalcPrint(stack);
        }
        else if (StringCmp(&s, "POP"))
        {
            CalcPop(stack);
        }
        else
        {
            PrintError("WRONG COMMAND");
        }
    }
    StringFree(&s);
}

ParsingResult ParsePoly()
{
    ParsingResult res_poly = ReadPoly();

    if (IsError(res_poly))
    {
        return res_poly;
    }
    else
    {
        if (!IsEnding(PeekChar()))
        { //na sam koniec upewnijmy się że jest znak nowej linii / eof
            Poly p = UnpackPoly(res_poly);
            PolyDestroy(&p);
            return ParsingError();
        }
        PopChar();
        return res_poly;
    }
}

void ParseLine(PolyStack *stack)
{
    char first = PeekChar();
    //printf("{%c}\n",first);
    if (IsLetter(first))
    {
        ParseCommand(stack);
    }
    else
    {
        ParsingResult res_poly = ParsePoly();
        if (IsError(res_poly))
        {
            PrintParsingError(res_poly);
            ConsumeLine();
        }
        else
        {
            Poly p = UnpackPoly(res_poly);
            CalcPush(stack, p);
        }
    }

    lineno += 1;
    columnno = 0;
}
