#include "Expr.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEF_CONFIG(name, value, size) \
    static const char name[] = value; \
    static const int name##_size = size;
#include "Constants.dat"
#undef DEF_CONFIG

static unsigned char isValidLiteral(const char * str, int begin, int end)
{
    int i;
    const char * itr;
    for (i = begin, itr = str + begin; i < end; i++, itr++)
    {
        if (!strncmp(itr, expr_and, expr_and_size)
         || !strncmp(itr, expr_or, expr_or_size)
         || !strncmp(itr, expr_implication, expr_implication_size)
         || !strncmp(itr, expr_negation, expr_negation_size)
         || !strncmp(itr, expr_open_p, expr_open_p_size)
         || !strncmp(itr, expr_close_p, expr_close_p_size))
        {
            return 0;
        }
    }
    
    return 1;
}

static char * trim(const char * str)
{
    char trimmed[strlen(str) + 1];
    
    int i, j;
    for (i = j = 0; i < strlen(str); i++)
    {
        if (str[i] != ' ' && str[i] != '\t')
        {
            trimmed[j++] = str[i];
        }
    }
    trimmed[j] = '\0';
        
    char * result = (char *)malloc(j + 1);
    assert(result != NULL);
    return strncpy(result, trimmed, j);
}

/*
 *  <expr> = F
 *  <expr> = <literal>
 *  <expr> = ~ <literal>
 *  <expr> = (<expr>)
 *  <expr> = ~ (<expr>)
 *  <expr> = <expr><op><expr>
 *  <op>   = & | V | ->
 *  <literal> = [a-z] | [a-z]<literal>
 */
static Expr * Expr_createHelper(const char * str, int begin, int end)
{
    // <expr> = F
    if (!strcmp(expr_falsum, str + begin))
    {
        return Expr_createAtomic(FALSUM);
    }
    // <expr> = <literal>
    else if (isValidLiteral(str, begin, end))
    {
        Expr * expr = Expr_createAtomic(LITERAL);
        expr->literal = (char *)malloc(end - begin + 1);
        assert(expr->literal != NULL);
        
        expr->literal = strncpy(expr->literal, str + begin, end - begin + 1);
        
        return expr;
    }
    //  <expr> = ~ <literal>
    else if (!strncmp(str + begin, expr_negation, expr_negation_size)
            && isValidLiteral(str, begin + expr_negation_size, end))
    {
        Expr * expr = Expr_createAtomic(NEGATION);
        expr->rightExpr 
            = Expr_createHelper(str, begin + expr_negation_size, end);
            
        return expr;
    }
    // <expr> = (<expr>)
    else if (!strncmp(str + begin, expr_open_p, expr_open_p_size)
          && !strncmp(str + end - expr_close_p_size + 1, expr_close_p, expr_close_p_size))
    {
        return Expr_createHelper(str, begin + expr_open_p_size, end - expr_close_p_size);
    }
    // <expr> = ~ (<expr>)
    else if (!strncmp(str + begin, expr_negation, expr_negation_size)
          && !strncmp(str + begin + expr_negation_size, expr_open_p, expr_open_p_size)
          && !strncmp(str + end - expr_close_p_size + 1, expr_close_p, expr_close_p_size))
    {
        Expr * expr = Expr_createAtomic(NEGATION);
        expr->rightExpr 
            = Expr_createHelper(str, begin + expr_negation_size, end);
            
        return expr;        
    }
    // <expr> = <expr><op><expr>
    else
    {
        Expr * expr = Expr_createAtomic(PARANTHESIS);
        int left_begin, left_end, right_begin, right_end;
        int paranthesisCount = 0;
        int i;
        const char * itr;
        
        left_begin = begin;
        right_end  = end;
        
        for (i = begin, itr = str + begin; i < end; i++, itr++)
        {
            // printf("%d, %s\n", paranthesisCount, itr);
            
            assert(paranthesisCount >= 0);
            if (!strncmp(itr, expr_open_p, expr_open_p_size))
            {
                paranthesisCount++;
            }
            else if (!strncmp(itr, expr_close_p, expr_close_p_size))
            {
                paranthesisCount--;
            }

            if (paranthesisCount == 0)
            {
                if (!strncmp(itr, expr_and, expr_and_size))
                {
                    expr->operation = AND;
                    left_end = i - 1;
                    right_begin = i + expr_and_size;
                    break;
                }
                else if (!strncmp(itr, expr_or, expr_or_size))
                {
                    expr->operation = OR;
                    left_end = i - 1;
                    right_begin = i + expr_or_size;
                    break;
                }
                else if (!strncmp(itr, expr_implication, expr_implication_size))
                {
                    expr->operation = IMPLICATION;
                    left_end = i - 1;
                    right_begin = i + expr_implication_size;
                    break;
                }
            }
        }
        expr->leftExpr = Expr_createHelper(str, left_begin, left_end);
        expr->rightExpr = Expr_createHelper(str, right_begin, right_end);
        
        return expr;
    }
}

Expr * Expr_createAtomic(ExprOperation operation)
{
    Expr * expr = (Expr *)malloc(sizeof(Expr));
    assert(expr != NULL);
    
    expr->leftExpr  = NULL;
    expr->literal   = NULL;
    expr->rightExpr = NULL;
    expr->operation = operation;
    
    return expr;
}

Expr * Expr_create(const char * str)
{
    assert(str != NULL);
    char * trimmed = trim(str);
    return Expr_createHelper(trimmed, 0, strlen(trimmed)-1);
}

Expr * Expr_copy(Expr * original)
{
    if (original == NULL) return NULL;
    
    Expr * copyExpr = Expr_createAtomic(original->operation);
    
    if (original->literal != NULL)
    {
        copyExpr->literal = (char *)calloc(strlen(original->literal + 1), 1);
        assert(copyExpr->literal != NULL);
        copyExpr->literal = strcpy(copyExpr->literal, original->literal);
    }
    if (original->leftExpr != NULL)
    {
        copyExpr->leftExpr = Expr_copy(original->leftExpr);
    }
    if (original->rightExpr != NULL)
    {
        copyExpr->rightExpr = Expr_copy(original->rightExpr);
    }
    copyExpr->operation = original->operation;
    
    return copyExpr;    
}

void Expr_delete(Expr ** exprPtr)
{
    Expr * expr = *exprPtr;
    
    if (expr->leftExpr  != NULL) Expr_delete(&expr->leftExpr);
    if (expr->literal   != NULL) free(expr->literal);
    if (expr->rightExpr != NULL) Expr_delete(&expr->rightExpr);
    
    free(expr);
    *exprPtr = NULL;
}

void Expr_print(Expr * expr, FILE * stream)
{
    if (expr->operation == FALSUM)
    {
        fputs(expr_falsum, stream);
        return;
    }
    else if (expr->operation == LITERAL)
    {
        fputs(expr->literal, stream);
        return;
    }
    else if (expr->operation == NEGATION)
    {
        fputs(expr_negation, stream);
        Expr_print(expr->rightExpr, stream);
    }
    else
    {
        fputs(expr_open_p, stream);
        Expr_print(expr->leftExpr, stream);
        fputc(' ', stream);
        switch (expr->operation)
        {
            case AND         :
                fputs(expr_and, stream);
                break;
            case OR          :
                fputs(expr_or, stream);
                break;
            case IMPLICATION :
                fputs(expr_implication, stream);
                break;
        }
        fputc(' ', stream);
        Expr_print(expr->rightExpr, stream);
        fputs(expr_close_p, stream);
    }
}

unsigned char Expr_areEqual(Expr * expr1, Expr * expr2)
{
    if (expr1 == NULL && expr2 == NULL) 
        return 1;
    else if (expr1 == NULL || expr2 == NULL) 
        return 0;
    else if (expr1->operation == LITERAL && expr2->operation == LITERAL
          && !strcmp(expr1->literal, expr2->literal)) 
        return 1;
    else
        return (expr1->operation != LITERAL || expr2->operation != LITERAL)
            && expr1->operation == expr2->operation
            && Expr_areEqual(expr1->leftExpr, expr2->leftExpr) 
            && Expr_areEqual(expr1->rightExpr, expr2->rightExpr);
}
