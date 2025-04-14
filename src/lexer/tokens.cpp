#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

#include "tokens.hpp"

extern char *yytext;
extern size_t yycolumn;
extern size_t yylineno;

static char *desugar_num(char *, char *);

static int hex_to_num(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else
    {
        printf("error: invalid hex digit '%c'\n", c);
        abort();
    }
}

static char *desugar_num(char *tok, char *default_suffix)
{
    int len = strlen(tok);
    int start = 0;
    int end;
    long long int val = 0;
    char *res = (char *)malloc(64);

    if (tok[0] == '0')
    {
        start = 2;
    }

    for (int i = 0, k = 0; i < len; i++)
    {
        if (tok[i] == '_')
        {
            k = i + 1;
            memmove(tok + i, tok + k, len - i);
            len -= k - i;
        }
    }

    end = len - 1;

    for (int i = len; i > 0; i--)
    {
        if (tok[i] == 'i' || tok[i] == 'u')
        {
            end = i;
            break;
        }
    }

    if (tok[1] == 'b')
    {
        for (int i = end, accum = 0; i >= start; i--, accum++)
        {
            if (tok[i] == '_')
            {
                accum--;
                continue;
            }
            if (tok[i] == '1')
            {
                val += (long long int)pow(2, accum);
            }
        }
    }
    else if (tok[1] == 'x')
    {
        for (int i = end, accum = 0; i >= start; i--, accum++)
        {
            if (tok[i] == '_' || tok[i] == 'u' || tok[i] == 'i')
            {
                accum--;
                continue;
            }
            val += hex_to_num(tok[i]) * (long long int)pow(16, accum);
        }
    }
    else if (tok[1] == 'o')
    {
        for (int i = end, accum = 0; i >= start; i--, accum++)
        {
            if (tok[i] == '_')
            {
                accum--;
                continue;
            }
            val += (tok[i] - '0') * (long long int)pow(8, accum);
        }
    }
    else
    {
        for (int i = end, accum = 0; i >= start; i--, accum++)
        {
            if (tok[i] == '_')
            {
                accum--;
                continue;
            }
            val += (tok[i] - '0') * (long long int)pow(10, accum);
        }
    }

    if (default_suffix[0] == 'u')
    {
        snprintf(res, 64, "%llu%s", val, (end == len - 1) ? default_suffix : tok + end);
    }
    else
    {
        snprintf(res, 64, "%lld%s", val, (end == len - 1) ? default_suffix : tok + end);
    }
    return res;
}

Token *token(TokenType type, char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value.string = strdup(value);
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *doc(char *value, size_t startline, size_t endline, size_t startcolumn, size_t endcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_DOC;
    token->value.string = strdup(value);
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *identifier(char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_IDENTIFIER;
    token->value.string = strdup(value);
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *integer(char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_INTEGER;
    token->value.integer = std::stoll(std::string(value, strlen(value)));
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *number(char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_FLOAT;
    token->value.number = std::stod(std::string(value, strlen(value)));
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *charater(char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_CHAR;
    token->value.character = value[0];
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *boolean(char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_BOOL;
    token->value.boolean = std::string(value) == "true" ? 1 : 0;
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *strings(char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_STRING;
    token->value.string = strdup(value);
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

Token *bytes(char *value, size_t startline, size_t startcolumn)
{
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = T_BYTE;
    token->value.string = strdup(value);
    token->loc = location(startline, startline, startcolumn, startcolumn + strlen(value));

    return token;
};

const char *token_type_to_string(TokenType type)
{
    const char *string;
    switch (type)
    {
    case T_AT:
    case T_AMPER:
        string = "<operator>";
        break;

    default:
        string = "<unknown>";
        break;
    }

    return string;
};

void token_type_printf(TokenType type)
{
    printf("%s\n", token_type_to_string(type));
};

const char *token_to_string(Token *token)
{
    const char *string;
    switch (token->type)
    {
    case T_AT:
        string = "@";
        break;

    default:
        string = "<unknown>";
        break;
    }

    return string;
};

void token_printf(Token *token)
{
    printf("%s", token_to_string(token));
};
