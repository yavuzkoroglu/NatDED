#include "Proof.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEF_CONFIG(name, value, size) \
    static const char name[] = value; \
    static const int name##_size = size;
#include "Constants.dat"
#undef DEF_CONFIG

ProofLine * ProofLine_create
    (ProofLine * prev, unsigned int level, Rule rule, Expr * result)
{
    ProofLine * pl = (ProofLine *)malloc(sizeof(ProofLine));
    assert(pl != NULL);
    
    pl->level = level;
    pl->rule = rule;
    pl->result = result;
    pl->isReachable = 1;
    pl->mark = 0;
    pl->prevLine = prev;

    if (prev != NULL)
    {
        prev->nextLine = pl;
        ProofLine * tmp = pl;
        while (tmp != NULL)
        {
            if (tmp->level <= level)
            {
                tmp->isReachable = 1;
            }
            else
            {
                tmp->isReachable = 0;
            }

            if (tmp->rule == INDEPENDENT_ASSUMPTION && tmp->level <= level)
            {
                tmp->isReachable = 1;
                tmp = tmp->prevLine;
                while (tmp->rule != ASSUMPTION)
                {
                    tmp->isReachable = 0;
                    tmp = tmp->prevLine;
                }
                tmp->isReachable = 0;
            }
            
            if (tmp->level < level) level = tmp->level;
            tmp = tmp->prevLine;
        }
    }
    
    return pl;
}

unsigned char Proof_isReachable(Proof * proof, unsigned int lineNo)
{
    if (lineNo < 1 || lineNo > proof->nLines) return 0;
    
    int i;
    ProofLine * pl;
    for (i = 1, pl = proof->begin; i < lineNo; i++, pl = pl->nextLine);
    
    return pl->isReachable;
}

void ProofLine_print_short(unsigned int lineNo, ProofLine * pl, FILE * stream)
{
    int i;
    
    fprintf(stream, "%d) ", lineNo);
    for (i = 0; i < pl->level; i++)
    {
        if (i == 0) fputc('\t', stream);
        fputc('\t', stream);
    }
    
    Expr_print(pl->result, stream);
    
    fputs("\t[", stream);
    switch (pl->rule)
    {
        case PREMISE:
            fputs(proof_PREMISE, stream);
            break;
        case ASSUMPTION:
        case INDEPENDENT_ASSUMPTION:
            fputs(proof_ASSUMPTION, stream);
            break;
        case LEM:
            fputs(proof_LEM, stream);
            break;

        case CONJUNCTION_ELIMINATION1:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(proof_CONJUNCTION_ELIMINATION1, stream);
            break;
        case CONJUNCTION_ELIMINATION2:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(proof_CONJUNCTION_ELIMINATION2, stream);
            break;
        case CONJUNCTION_INTRODUCTION:
            fprintf(stream, "%u,%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(proof_CONJUNCTION_INTRODUCTION, stream);
            break;

        case DISJUNCTION_ELIMINATION:
            fprintf(stream, "%u,%u-%u,%u-%u ", pl->dependencies[0], 
                                               pl->dependencies[1],
                                               pl->dependencies[2],
                                               pl->dependencies[3],
                                               pl->dependencies[4]);
            fputs(proof_DISJUNCTION_ELIMINATION, stream);
            break;
        case DISJUNCTION_INTRODUCTION1:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(proof_DISJUNCTION_INTRODUCTION1, stream);
            break;
        case DISJUNCTION_INTRODUCTION2:
            fputs(proof_DISJUNCTION_INTRODUCTION2, stream);
            fprintf(stream, "%u ", pl->dependencies[0]);
            break;
            
        case IMPLICATION_ELIMINATION:
            fprintf(stream, "%u,%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(proof_IMPLICATION_ELIMINATION, stream);
            break;
        case IMPLICATION_INTRODUCTION:
            fprintf(stream, "%u-%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(proof_IMPLICATION_INTRODUCTION, stream);
            break;

        case NEGATION_ELIMINATION:
            fprintf(stream, "%u,%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(proof_NEGATION_ELIMINATION, stream);
            break;
        case NEGATION_INTRODUCTION:
            fprintf(stream, "%u-%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(proof_NEGATION_INTRODUCTION, stream);
            break;

        case DOUBLE_NEGATION_ELIMINATION:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(proof_DOUBLE_NEGATION_ELIMINATION, stream);
            break;
        case DOUBLE_NEGATION_INTRODUCTION:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(proof_DOUBLE_NEGATION_INTRODUCTION, stream);
            break;
    }
    fputs("]\n", stream);
    fflush(stream);
}

void ProofLine_print_long(unsigned int lineNo, ProofLine * pl, FILE * stream)
{
    int i;
    
    fprintf(stream, "%d,%d) ", pl->isReachable, lineNo);
    for (i = 0; i < pl->level; i++)
    {
        if (i == 0) fputc('\t', stream);
        fputc('\t', stream);
    }
    
    Expr_print(pl->result, stream);
    
    fputs("\t[", stream);
    switch (pl->rule)
    {
        case PREMISE:
            fputs(name_PREMISE, stream);
            break;
        case ASSUMPTION:
        case INDEPENDENT_ASSUMPTION:
            fputs(name_ASSUMPTION, stream);
            break;
        case LEM:
            fputs(name_LEM, stream);
            break;

        case CONJUNCTION_ELIMINATION1:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(name_CONJUNCTION_ELIMINATION1, stream);
            break;
        case CONJUNCTION_ELIMINATION2:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(name_CONJUNCTION_ELIMINATION2, stream);
            break;
        case CONJUNCTION_INTRODUCTION:
            fprintf(stream, "%u,%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(name_CONJUNCTION_INTRODUCTION, stream);
            break;

        case DISJUNCTION_ELIMINATION:
            fprintf(stream, "%u,%u-%u,%u-%u ", pl->dependencies[0], 
                                               pl->dependencies[1],
                                               pl->dependencies[2],
                                               pl->dependencies[3],
                                               pl->dependencies[4]);
            fputs(name_DISJUNCTION_ELIMINATION, stream);
            break;
        case DISJUNCTION_INTRODUCTION1:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(name_DISJUNCTION_INTRODUCTION1, stream);
            break;
        case DISJUNCTION_INTRODUCTION2:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(name_DISJUNCTION_INTRODUCTION2, stream);
            break;
            
        case IMPLICATION_ELIMINATION:
            fprintf(stream, "%u,%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(name_IMPLICATION_ELIMINATION, stream);
            break;
        case IMPLICATION_INTRODUCTION:
            fprintf(stream, "%u-%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(name_IMPLICATION_INTRODUCTION, stream);
            break;

        case NEGATION_ELIMINATION:
            fprintf(stream, "%u,%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(name_NEGATION_ELIMINATION, stream);
            break;
        case NEGATION_INTRODUCTION:
            fprintf(stream, "%u-%u ", pl->dependencies[0], pl->dependencies[1]);
            fputs(name_NEGATION_INTRODUCTION, stream);
            break;

        case DOUBLE_NEGATION_ELIMINATION:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(name_DOUBLE_NEGATION_ELIMINATION, stream);
            break;
        case DOUBLE_NEGATION_INTRODUCTION:
            fprintf(stream, "%u ", pl->dependencies[0]);
            fputs(name_DOUBLE_NEGATION_INTRODUCTION, stream);
            break;
    }
    fputs("]\n", stream);
    fflush(stream);
}

void ProofLine_delete(ProofLine ** plPtr)
{
    ProofLine * pl = *plPtr;
    ProofLine * p;
    if (pl->prevLine != NULL && pl->prevLine->level > pl->level)
    {
        int openLevel = pl->prevLine->level;
        for (p = pl->prevLine; 
             p != NULL && p->level >= openLevel; 
             p = p->prevLine)
        {
            if (p->level == openLevel)
            {
                p->isReachable = 1;
            }
        }
    }
    
    if (pl->prevLine != NULL)
        pl->prevLine->nextLine = NULL;
    Expr_delete(&pl->result);
    free(pl);
    *plPtr = NULL;
}

Proof * Proof_create()
{
    Proof * proof = (Proof *)malloc(sizeof(Proof));
    assert(proof != NULL);
    
    proof->nLines = 0;
    proof->begin  = NULL;
    proof->end    = NULL;
    
    return proof;
}

void Proof_applyPREMISE(Proof * proof, Expr * premise)
{
    if (proof->begin == NULL)
    {
        proof->begin = proof->end 
            = ProofLine_create(NULL, 0, PREMISE, premise);
        proof->nLines++;
    }
    else if (proof->end->rule == PREMISE)
    {
        proof->end = ProofLine_create(proof->end, 0, PREMISE, premise);
        proof->nLines++;
    }
    else
    {
        // This should be unreachable
        assert(0);
    }
}
void Proof_applyASSUMPTION(Proof * proof, Expr * assumption)
{
    if (proof->begin == NULL)
    {
        proof->begin = proof->end 
            = ProofLine_create(NULL, 1, ASSUMPTION, assumption);
        proof->nLines++;
    }
    else
    {
        proof->end 
            = ProofLine_create(proof->end, proof->end->level+1, 
                               ASSUMPTION, assumption);
        proof->nLines++;
    }
}
void Proof_applyINDEPENDENT_ASSUMPTION(Proof * proof, Expr * assumption)
{
    if (proof->begin == NULL)
    {
        proof->begin = proof->end 
            = ProofLine_create(NULL, 1, ASSUMPTION, assumption);
        proof->nLines++;
    }
    else if (proof->end->level == 0)
    {
        proof->end 
            = ProofLine_create(proof->end, proof->end->level+1, 
                               ASSUMPTION, assumption);
        proof->nLines++;
    }
    else
    {
        proof->end 
            = ProofLine_create(proof->end, proof->end->level, 
                               INDEPENDENT_ASSUMPTION, assumption);
        proof->nLines++;
    }
}
void Proof_applyCONJUNCTION_INTRODUCTION
    (Proof * proof, unsigned int left, unsigned int right)
{
    assert(0 < left && left <= proof->nLines
        && 0 < right && right <= proof->nLines);
    
    Expr * result = Expr_createAtomic(AND);
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == left)
        {
            assert(pl->isReachable);
            result->leftExpr = Expr_copy(pl->result);
        }
        if (i == right)
        {
            assert(pl->isReachable);
            result->rightExpr = Expr_copy(pl->result);
        }
    }
    
    proof->end 
        = ProofLine_create(proof->end, proof->end->level, 
                           CONJUNCTION_INTRODUCTION, result);
    proof->end->dependencies[0] = left;
    proof->end->dependencies[1] = right;
    proof->nLines++;
}
void Proof_applyCONJUNCTION_ELIMINATION1
    (Proof * proof, unsigned int conjunction)
{
    assert(0 < conjunction && conjunction <= proof->nLines);
    
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == conjunction)
        {
            assert(pl->isReachable && pl->result->operation == AND);
            proof->end
                = ProofLine_create
                    (proof->end, proof->end->level, 
                     CONJUNCTION_ELIMINATION1,Expr_copy(pl->result->leftExpr));
            proof->end->dependencies[0] = conjunction;
            proof->nLines++;
            return;
        }
    }        
}
void Proof_applyCONJUNCTION_ELIMINATION2(Proof * proof, unsigned int conjunction)
{
    assert(0 < conjunction && conjunction < proof->nLines);
    
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == conjunction)
        {
            assert(pl->isReachable && pl->result->operation == AND);
            proof->end
                = ProofLine_create
                    (proof->end, proof->end->level, 
                     CONJUNCTION_ELIMINATION2,Expr_copy(pl->result->rightExpr));
            proof->end->dependencies[0] = conjunction;
            proof->nLines++;
            return;
        }
    }    
}

void Proof_applyDISJUNCTION_INTRODUCTION1
    (Proof * proof, unsigned int left, Expr * right)
{
    assert(0 < left && left <= proof->nLines);

    Expr * result = Expr_createAtomic(OR);
    result->rightExpr = right;
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == left)
        {
            assert(pl->isReachable);
            result->leftExpr = Expr_copy(pl->result);
            proof->end
                = ProofLine_create
                    (proof->end, proof->end->level,
                     DISJUNCTION_INTRODUCTION1, result);
            proof->end->dependencies[0] = left;
            proof->nLines++;
            return;
        }
    }
}
void Proof_applyDISJUNCTION_INTRODUCTION2
    (Proof * proof, Expr * left, unsigned int right)
{
    assert(0 < right && right <= proof->nLines);

    Expr * result = Expr_createAtomic(OR);
    result->leftExpr = left;
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == right)
        {
            assert(pl->isReachable);
            result->rightExpr = Expr_copy(pl->result);
            proof->end
                = ProofLine_create
                    (proof->end, proof->end->level,
                     DISJUNCTION_INTRODUCTION2, result);
            proof->end->dependencies[0] = right;
            proof->nLines++;
            return;
        }
    }    
}
void Proof_applyDISJUNCTION_ELIMINATION
    (Proof * proof, unsigned int disjunction,
                    unsigned int assumption1, 
                    unsigned int conclusion1,
                    unsigned int assumption2,
                    unsigned int conclusion2)
{
    assert(0 < disjunction && disjunction <= proof->nLines
        && 0 < assumption1 && assumption1 <= proof->nLines
        && 0 < conclusion1 && conclusion1 <= proof->nLines
        && 0 < assumption2 && assumption2 <= proof->nLines
        && 0 < conclusion2 && conclusion2 <= proof->nLines
        && proof->end->level > 0);
    
    int i = 1;
    Expr * disj_expr = NULL;
    Expr * ass1_expr = NULL;
    Expr * ass2_expr = NULL;
    Expr * con1_expr = NULL;
    Expr * con2_expr = NULL;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == disjunction)
        {
            assert(pl->isReachable && pl->result->operation == OR);
            disj_expr = pl->result;
        }
        if (i == assumption1)
        {
            ass1_expr = pl->result;
        }
        if (i == assumption2)
        {
            ass2_expr = pl->result;
        }
        if (i == conclusion1)
        {
            con1_expr = pl->result;
        }
        if (i == conclusion2)
        {
            con2_expr = pl->result;
        }
    }
        
    assert(Expr_areEqual(con1_expr, con2_expr)
        && Expr_areEqual(disj_expr->leftExpr, ass1_expr)
        && Expr_areEqual(disj_expr->rightExpr, ass2_expr));
            
    proof->end = 
        ProofLine_create(proof->end, proof->end->level-1, 
                         DISJUNCTION_ELIMINATION, Expr_copy(con1_expr));
    proof->end->dependencies[0] = disjunction;
    proof->end->dependencies[1] = assumption1;
    proof->end->dependencies[2] = conclusion1;
    proof->end->dependencies[3] = assumption2;
    proof->end->dependencies[4] = conclusion2;    
    proof->nLines++;
}
void Proof_applyIMPLICATION_INTRODUCTION
    (Proof * proof, unsigned int assumption, unsigned int conclusion)
{
    assert(0 < assumption && assumption <= proof->nLines
        && 0 < conclusion && conclusion <= proof->nLines
        && proof->end->level > 0);
    
    Expr * result = Expr_createAtomic(IMPLICATION);
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == assumption)
        {
            assert(pl->isReachable);
            result->leftExpr = Expr_copy(pl->result);
        }
        if (i == conclusion)
        {
            assert(pl->isReachable);
            result->rightExpr = Expr_copy(pl->result);
        }
    }
    
    proof->end = 
        ProofLine_create(proof->end, proof->end->level-1, 
                         IMPLICATION_INTRODUCTION, result);
    proof->end->dependencies[0] = assumption;
    proof->end->dependencies[1] = conclusion;
    proof->nLines++;
}
void Proof_applyIMPLICATION_ELIMINATION
    (Proof * proof, unsigned int implication, unsigned int antecedant)
{
    assert(0 < implication && implication <= proof->nLines
        && 0 < antecedant && antecedant <= proof->nLines);
    
    Expr * impl_expr;
    Expr * ant_expr;
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == implication)
        {
            assert(pl->isReachable && pl->result->operation == IMPLICATION);
            impl_expr = pl->result;
        }
        if (i == antecedant)
        {
            assert(pl->isReachable);
            ant_expr = pl->result;
        }
    }
    
    assert(Expr_areEqual(impl_expr->leftExpr, ant_expr));
    
    proof->end =
        ProofLine_create(proof->end, proof->end->level,
                         IMPLICATION_ELIMINATION, 
                         Expr_copy(impl_expr->rightExpr));
    proof->end->dependencies[0] = implication;
    proof->end->dependencies[1] = antecedant;
    proof->nLines++;
}

void Proof_applyNEGATION_INTRODUCTION
    (Proof * proof, unsigned int assumption, unsigned int falsum)
{
    assert(0 < assumption && assumption <= proof->nLines
        && 0 < falsum && falsum <= proof->nLines);
        
    Expr * result = Expr_createAtomic(NEGATION);
    Expr * ass_expr;
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == assumption)
        {
            assert(pl->isReachable);
            ass_expr = pl->result;
        }
        if (i == falsum)
        {
            assert(pl->isReachable && pl->result->operation == FALSUM);
        }
    }
    result->rightExpr = Expr_copy(ass_expr);
    
    proof->end =
        ProofLine_create(proof->end, proof->end->level-1,
                         NEGATION_INTRODUCTION, result);
    proof->end->dependencies[0] = assumption;
    proof->end->dependencies[1] = falsum;
    proof->nLines++;
}
void Proof_applyNEGATION_ELIMINATION
    (Proof * proof, unsigned int expr, unsigned int negExpr)
{
    assert(0 < expr && expr <= proof->nLines
        && 0 < negExpr && negExpr <= proof->nLines);
    Expr * _expr;
    Expr * neg_expr;
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == expr)
        {
            assert(pl->isReachable);
            _expr = pl->result;
        }
        if (i == negExpr)
        {
            assert(pl->isReachable && pl->result->operation == NEGATION);
            neg_expr = pl->result;
        }
    }
    
    assert(Expr_areEqual(neg_expr->rightExpr, _expr));
    
    proof->end =
        ProofLine_create(proof->end, proof->end->level,
                         NEGATION_ELIMINATION, Expr_createAtomic(FALSUM));
    proof->end->dependencies[0] = expr;
    proof->end->dependencies[1] = negExpr;
    proof->nLines++;
}

void Proof_applyDOUBLE_NEGATION_INTRODUCTION
    (Proof * proof, unsigned int expr)
{
    assert(0 < expr && expr <= proof->nLines);
    
    Expr * result = Expr_createAtomic(NEGATION);
    result->rightExpr = Expr_createAtomic(NEGATION);
        
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == expr)
        {
            assert(pl->isReachable);
            result->rightExpr->rightExpr = Expr_copy(pl->result);
            proof->end 
                = ProofLine_create(proof->end, proof->end->level, 
                                   DOUBLE_NEGATION_INTRODUCTION, result);
            proof->end->dependencies[0] = expr;
            proof->nLines++;
            return;
        }
    }   
}
void Proof_applyDOUBLE_NEGATION_ELIMINATION
    (Proof * proof, unsigned int expr)
{
    assert(0 < expr && expr <= proof->nLines);
    
    int i = 1;
    ProofLine * pl;
    for (pl = proof->begin; pl != NULL; pl = pl->nextLine, i++)
    {
        if (i == expr)
        {
            assert(pl->isReachable 
                && pl->result->operation == NEGATION
                && pl->result->rightExpr->operation == NEGATION);
            proof->end 
                = ProofLine_create(proof->end, proof->end->level, 
                                   DOUBLE_NEGATION_ELIMINATION, 
                                   Expr_copy(pl->result->rightExpr->rightExpr));
            proof->end->dependencies[0] = expr;
            proof->nLines++;
            return;
        }
    }   
}
    
void Proof_applyLEM(Proof * proof, Expr * expr)
{
    Expr * result = Expr_createAtomic(OR);
    result->leftExpr = expr;
    result->rightExpr = Expr_createAtomic(NEGATION);
    result->rightExpr->rightExpr = Expr_copy(expr);
    
    if (proof->begin == NULL)
    {
        proof->begin = proof->end = ProofLine_create(NULL, 0, LEM, result);
        proof->nLines++;        
    }
    else
    {
        proof->end 
            = ProofLine_create(proof->end, proof->end->level, LEM, result);
        proof->nLines++;
    }
}

void Proof_print_short(Proof * proof, FILE * stream)
{
    unsigned int i;
    ProofLine * pl;
    fputs("\n", stream);
    for (i = 1, pl = proof->begin; pl != NULL; i++, pl = pl->nextLine)
    {
        ProofLine_print_short(i, pl, stream);
    }
    fputs("\n", stream);
}
void Proof_print_long(Proof * proof, FILE * stream)
{
    unsigned int i;
    ProofLine * pl;
    fputs("\n", stream);
    for (i = 1, pl = proof->begin; pl != NULL; i++, pl = pl->nextLine)
    {
        ProofLine_print_long(i, pl, stream);
    }
    fputs("\n", stream);
}

void Proof_deleteLastLine(Proof * proof)
{
    assert(proof->end != NULL);
    if (proof->end->prevLine != NULL)
    {
        proof->end = proof->end->prevLine;
        ProofLine_delete(&proof->end->nextLine);
    }
    else
    {
        ProofLine_delete(&proof->end);
        proof->begin = NULL;
    }
    proof->nLines--;
}
void Proof_delete(Proof ** proofPtr)
{
    Proof * proof = *proofPtr;
    while (proof->end != NULL)
        Proof_deleteLastLine(proof);
    *proofPtr = NULL;
}
