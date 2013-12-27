#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>

typedef enum {
    PARANTHESIS,
    AND,
    OR,
    IMPLICATION,
    NEGATION,
    FALSUM,
    LITERAL
} ExprOperation;

typedef struct ExprBody
{
    struct ExprBody * leftExpr;
    ExprOperation operation;
    char * literal;
    struct ExprBody * rightExpr;
} Expr;

Expr * Expr_createAtomic(ExprOperation operation);
Expr * Expr_create(const char * str);
Expr * Expr_copy(Expr * original);
void Expr_delete(Expr ** exprPtr);

void Expr_print(Expr * expr, FILE * stream);

unsigned char Expr_areEqual(Expr * expr1, Expr * expr2);

#endif
