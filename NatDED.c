#include "Proof.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define BUFFER_SIZE 4096
#define RULE_BUFFER_SIZE 32

#define DEF_CONFIG(name, value, size) \
    static const char name[] = value; \
    static const int name##_size = size;
#include "Constants.dat"
#undef DEF_CONFIG

FILE * input;
FILE * output;
Proof * proof;
Proof * stack;
char * buffer;
char * ruleBuffer;

void help();
void commandHelp();
char * safe_fgets(char * __s, int __n, FILE * __stream);
char * str_replace(char * str, char oldch, char newch);

void fillProofStack();
void traverseProofStack();
unsigned char eliminateContradictions();
void eliminateImplications();
void clearProofStack();

unsigned char derive(Expr * goal);

int main(int argc, char * argv[])
{
    buffer = (char *)malloc(BUFFER_SIZE);
    assert(buffer != NULL);
    proof = Proof_create();
    stack = Proof_create();
    if (argc > 2)
    {
        output = fopen(argv[2], "w");
        input  = fopen(argv[1], "r");
    }
    else if (argc > 1)
    {
        output = stdout;
        input  = fopen(argv[1], "r");
    }
    else
    {
        output = stdout;
        input  = stdin;
    }
    
    if (input == NULL || output == NULL)
    {
        help();
    }
    
    char tmp;
    ProofLine * pl;
    int i;
    while(!feof(input))
    {
        if (input == stdin)
        {
            printf("> ");
        }
                
        buffer = safe_fgets(buffer, BUFFER_SIZE, input);
        buffer = str_replace(buffer, '\n', '\0');
                
        if (buffer[0] == '[')
        {
            ruleBuffer = (char *)calloc(RULE_BUFFER_SIZE, 1);
            assert(ruleBuffer != NULL);
            int i;
            for (i = 1; i < RULE_BUFFER_SIZE; i++)
            {
                if (buffer[i] == ']') break;
                ruleBuffer[i-1] = buffer[i];
            }
            
            // 1
            if (!strncmp(ruleBuffer, proof_PREMISE, proof_PREMISE_size))
            {
                Expr * premise = Expr_create(buffer + i + 1);
                Proof_applyPREMISE(proof, premise);
            }
            // 2
            else if (!strncmp(ruleBuffer, 
                              proof_ASSUMPTION, 
                              proof_ASSUMPTION_size))
            {
                Expr * assumption = Expr_create(buffer + i + 1);
                Proof_applyASSUMPTION(proof, assumption);
            }
            // 3
            else if (!strncmp(ruleBuffer, 
                              proof_INDEPENDENT_ASSUMPTION, 
                              proof_INDEPENDENT_ASSUMPTION_size))
            {
                Expr * assumption = Expr_create(buffer + i + 1);
                Proof_applyINDEPENDENT_ASSUMPTION(proof, assumption);
            }
            // 4
            else if (!strncmp(ruleBuffer, 
                              proof_LEM, 
                              proof_LEM_size))
            {
                Expr * expr = Expr_create(buffer + i + 1);
                Proof_applyLEM(proof, expr);
            }
            // 5
            else if (!strncmp(ruleBuffer, 
                              proof_CONJUNCTION_ELIMINATION1, 
                              proof_CONJUNCTION_ELIMINATION1_size))
            {
                unsigned int conjunction;
                sscanf(ruleBuffer + proof_CONJUNCTION_ELIMINATION1_size, 
                       "%u", &conjunction);
                Proof_applyCONJUNCTION_ELIMINATION1(proof, conjunction);
            }
            // 6
            else if (!strncmp(ruleBuffer, 
                              proof_CONJUNCTION_ELIMINATION2, 
                              proof_CONJUNCTION_ELIMINATION2_size))
            {
                unsigned int conjunction;
                sscanf(ruleBuffer + proof_CONJUNCTION_ELIMINATION2_size, 
                       "%u", &conjunction);
                Proof_applyCONJUNCTION_ELIMINATION2(proof, conjunction);
            }
            // 7
            else if (!strncmp(ruleBuffer, 
                              proof_CONJUNCTION_INTRODUCTION, 
                              proof_CONJUNCTION_INTRODUCTION_size))
            {
                unsigned int left, right;
                sscanf(ruleBuffer + proof_CONJUNCTION_INTRODUCTION_size,
                       "%u,%u", &left, &right);
                Proof_applyCONJUNCTION_INTRODUCTION(proof, left, right);
            }
            // 8
            else if (!strncmp(ruleBuffer, 
                              proof_DISJUNCTION_ELIMINATION, 
                              proof_DISJUNCTION_ELIMINATION_size))
            {
                unsigned int disj, a1, c1, a2, c2;
                sscanf(ruleBuffer + proof_DISJUNCTION_ELIMINATION_size,
                       "%u,%u-%u,%u-%u", &disj, &a1, &c1, &a2, &c2);
                Proof_applyDISJUNCTION_ELIMINATION(proof, disj, a1, c1, a2, c2);
            }
            // 9
            else if (!strncmp(ruleBuffer, 
                              proof_DISJUNCTION_INTRODUCTION1, 
                              proof_DISJUNCTION_INTRODUCTION1_size))
            {
                unsigned int left;
                sscanf(ruleBuffer + proof_DISJUNCTION_INTRODUCTION1_size,
                       "%u", &left);
                Expr * right = Expr_create(buffer + i + 1);
                Proof_applyDISJUNCTION_INTRODUCTION1(proof, left, right);
            }
            // 10
            else if (!strncmp(ruleBuffer, 
                              proof_DISJUNCTION_INTRODUCTION2, 
                              proof_DISJUNCTION_INTRODUCTION2_size))
            {
                Expr * left = Expr_create(buffer + i + 1);
                unsigned int right;
                sscanf(ruleBuffer + proof_DISJUNCTION_INTRODUCTION2_size,
                       "%u", &right);
                Proof_applyDISJUNCTION_INTRODUCTION2(proof, left, right);
            }
            // 11
            else if (!strncmp(ruleBuffer, 
                              proof_DOUBLE_NEGATION_ELIMINATION, 
                              proof_DOUBLE_NEGATION_ELIMINATION_size))
            {
                unsigned int expr;
                sscanf(ruleBuffer + proof_DOUBLE_NEGATION_ELIMINATION_size,
                       "%u", &expr);
                Proof_applyDOUBLE_NEGATION_ELIMINATION(proof, expr);
            }
            // 12
            else if (!strncmp(ruleBuffer, 
                              proof_DOUBLE_NEGATION_INTRODUCTION, 
                              proof_DOUBLE_NEGATION_INTRODUCTION_size))
            {
                unsigned int expr;
                sscanf(ruleBuffer + proof_DOUBLE_NEGATION_INTRODUCTION_size,
                       "%u", &expr);
                Proof_applyDOUBLE_NEGATION_INTRODUCTION(proof, expr);
            }    
            // 13                                
            else if (!strncmp(ruleBuffer, 
                              proof_NEGATION_ELIMINATION, 
                              proof_NEGATION_ELIMINATION_size))
            {
                unsigned int expr, negExpr;
                sscanf(ruleBuffer + proof_NEGATION_ELIMINATION_size,
                       "%u,%u", &expr, &negExpr);
                Proof_applyNEGATION_ELIMINATION(proof, expr, negExpr);
            }
            // 14                             
            else if (!strncmp(ruleBuffer, 
                              proof_NEGATION_INTRODUCTION, 
                              proof_NEGATION_INTRODUCTION_size))
            {
                unsigned int assumption, falsum;
                sscanf(ruleBuffer + proof_NEGATION_INTRODUCTION_size,
                       "%u-%u", &assumption, &falsum);
                Proof_applyNEGATION_INTRODUCTION(proof, assumption, falsum); 
            }
            // 15                                
            else if (!strncmp(ruleBuffer, 
                              proof_IMPLICATION_ELIMINATION, 
                              proof_IMPLICATION_ELIMINATION_size))
            {
                unsigned int implication, antecedant;
                sscanf(ruleBuffer + proof_IMPLICATION_ELIMINATION_size,
                       "%u,%u", &implication, &antecedant);
                Proof_applyIMPLICATION_ELIMINATION(proof, 
                                                   implication, 
                                                   antecedant);
            }
            // 16
            else if (!strncmp(ruleBuffer, 
                              proof_IMPLICATION_INTRODUCTION, 
                              proof_IMPLICATION_INTRODUCTION_size))
            {
                unsigned int assumption, conclusion;
                sscanf(ruleBuffer + proof_IMPLICATION_INTRODUCTION_size,
                       "%u-%u", &assumption, &conclusion);
                Proof_applyIMPLICATION_INTRODUCTION(proof, 
                                                    assumption, 
                                                    conclusion);
            }
            free(ruleBuffer);
            ruleBuffer = NULL;
        }
        else if (buffer[0] == '-')
        {         
            if (!strncmp(buffer+1, 
                         command_goal,
                         command_goal_size))
            {
                Expr * goal = Expr_create(buffer + command_goal_size + 1);
                derive(goal);
                clearProofStack();
            }
            else if (!strncmp(buffer+1, 
                         command_delete_last,
                         command_delete_last_size))
            {
                if (proof->nLines == 0)
                {
                    fputs("\n", output);
                    fputs("No lines to delete!\n", output);
                    fputs("\n", output);
                }
                else
                {
                    Proof_deleteLastLine(proof);
                }
            }
            else if (!strncmp(buffer+1, 
                              command_print_short,
                              command_print_short_size))
            {
                Proof_print_short(proof, output);
            }
            else if (!strncmp(buffer+1, 
                              command_print_long,
                              command_print_long_size))
            {
                Proof_print_long(proof, output);
            }       
            else if (!strncmp(buffer+1, 
                      command_clear,
                      command_clear_size))
            {
                Proof_delete(&proof);
                clearProofStack();
                proof = Proof_create();
            }
            else if (!strncmp(buffer+1, 
                      command_exit,
                      command_exit_size))
            {
                exit(0);
            }
            else
            {
                commandHelp();
            }
        }
        else
        {
            commandHelp();
        }
    }
    
    return 0;
}

void help() 
{
    puts("");
    puts("Usage : NatDED [<InputFile>] [<OutputFile>]");
    puts("");
    exit(0);
}

void commandHelp()
{
    fputc('\n', output);
    fputs("Program Commands :\n", output);
    fputs("------------------\n", output);
    fprintf(output, "-%s : Try to reach a given goal using current rules\n",
            command_goal);
    fprintf(output, "-%s : Delete the last line of the proof\n", 
            command_delete_last);
    fprintf(output, "-%s : Print the proof using short notation\n", 
            command_print_short);
    fprintf(output, "-%s : Print the proof using long notation\n", 
            command_print_long);
    fprintf(output, "-%s : Clear proof\n", 
            command_clear);
    fprintf(output, "-%s : Finish program\n", 
            command_exit);
    fputc('\n', output);
    fputs("Proof Rules :\n", output);
    fputs("-------------\n", output);
    fprintf(output, "[%s]\t%s\n",
            proof_PREMISE, name_PREMISE);
    fprintf(output, "[%s]\t%s\n",
            proof_ASSUMPTION, name_ASSUMPTION);
    fprintf(output, "[%s]\tindependent %s\n",
            proof_INDEPENDENT_ASSUMPTION, name_INDEPENDENT_ASSUMPTION);
    fprintf(output, "[%s]\t%s\n",
            proof_CONJUNCTION_INTRODUCTION, name_CONJUNCTION_INTRODUCTION);
    fprintf(output, "[%s]\t%s\n",
            proof_CONJUNCTION_ELIMINATION1, name_CONJUNCTION_ELIMINATION1);
    fprintf(output, "[%s]\t%s\n",
            proof_CONJUNCTION_ELIMINATION2, name_CONJUNCTION_ELIMINATION2);
    fprintf(output, "[%s]\t%s\n",
            proof_DISJUNCTION_INTRODUCTION1, name_DISJUNCTION_INTRODUCTION1);
    fprintf(output, "[%s]\t%s\n",
            proof_DISJUNCTION_INTRODUCTION2, name_DISJUNCTION_INTRODUCTION2);
    fprintf(output, "[%s]\t%s\n",
            proof_DISJUNCTION_ELIMINATION, name_DISJUNCTION_ELIMINATION);
    fprintf(output, "[%s]\t%s\n",
            proof_IMPLICATION_INTRODUCTION, name_IMPLICATION_INTRODUCTION);
    fprintf(output, "[%s]\t%s\n",
            proof_IMPLICATION_ELIMINATION, name_IMPLICATION_ELIMINATION);
    fprintf(output, "[%s]\t%s\n",
            proof_NEGATION_INTRODUCTION, name_NEGATION_INTRODUCTION);
    fprintf(output, "[%s]\t%s\n",
            proof_NEGATION_ELIMINATION, name_NEGATION_ELIMINATION);
    fprintf(output, "[%s]\t%s\n",
            proof_DOUBLE_NEGATION_INTRODUCTION, 
            name_DOUBLE_NEGATION_INTRODUCTION);
    fprintf(output, "[%s]\t%s\n",
            proof_DOUBLE_NEGATION_ELIMINATION, 
            name_DOUBLE_NEGATION_ELIMINATION);
    fprintf(output, "[%s]\t%s\n",
            proof_LEM, name_LEM);
    fputc('\n', output);
}

char * safe_fgets(char * __s, int __n, FILE * __stream)
{
    char tmp = fgetc(input);
    if (tmp == EOF) exit(0);
    ungetc(tmp, input);
    
    return fgets(__s, BUFFER_SIZE, input);
}

char * str_replace(char * str, char oldch, char newch)
{
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == oldch)
        {
            str[i] = newch;
        }
    }
    
    return str;
}

void fillProofStack()
{
    ProofLine * pl;
    ProofLine * newpl;
    int i;
    for (i = 1, pl = proof->begin; pl != NULL; i++, pl = pl->nextLine)
    {
        if (pl->isReachable)
        {
            stack->end = ProofLine_create(stack->end, 
                                          0, 
                                          PREMISE, 
                                          Expr_copy(pl->result));
            stack->end->mark = i;
            if (stack->begin == NULL)
            {
                stack->begin = stack->end;
            }
        }
    }
}

void traverseProofStack()
{
    ProofLine * pl;
    ProofLine * tmp;
    for (pl = stack->begin; pl != NULL; pl = pl->nextLine)
    {
        // Double Negation Elimination
        if (pl->result->operation == NEGATION 
         && pl->result->rightExpr->operation == NEGATION)
        {
            int i = pl->mark;
            Proof_applyDOUBLE_NEGATION_ELIMINATION(proof, i);
            stack->end = ProofLine_create(stack->end, 
                                          proof->end->level,
                                          proof->end->rule,
                                          Expr_copy(proof->end->result));
            stack->end->mark = proof->nLines;
            
            if (pl->prevLine != NULL) pl->prevLine->nextLine = pl->nextLine;
            if (pl->nextLine != NULL) pl->nextLine->prevLine = pl->prevLine;
            
            pl->prevLine = NULL;
            tmp = pl->nextLine;
            if (pl == stack->begin) stack->begin = pl->nextLine;
            pl->nextLine = NULL;
            ProofLine_delete(&pl);
            pl = tmp;
        }
        
        // Conjunction seperation
        if (pl->result->operation == AND)
        {
            int i = pl->mark;
            
            Proof_applyCONJUNCTION_ELIMINATION1(proof, i);
            stack->end = ProofLine_create(stack->end, 
                                          proof->end->level,
                                          proof->end->rule,
                                          Expr_copy(proof->end->result));
            stack->end->mark = proof->nLines;

            Proof_applyCONJUNCTION_ELIMINATION2(proof, i);
            stack->end = ProofLine_create(stack->end, 
                                     proof->end->level,
                                     proof->end->rule,
                                     Expr_copy(proof->end->result));
            stack->end->mark = proof->nLines;

            if (pl->prevLine != NULL) pl->prevLine->nextLine = pl->nextLine;
            if (pl->nextLine != NULL) pl->nextLine->prevLine = pl->prevLine;
            
            pl->prevLine = NULL;
            tmp = pl->nextLine;
            if (pl == stack->begin) stack->begin = pl->nextLine;
            pl->nextLine = NULL;
            ProofLine_delete(&pl);
            pl = tmp;
        }
        
        // Disjunction conversion
        if (pl->result->operation == OR
         && !(pl->result->rightExpr->operation == NEGATION
           && Expr_areEqual(pl->result->leftExpr, 
                            pl->result->rightExpr->rightExpr)))
        {
            if (pl->result->leftExpr->operation != NEGATION)
            {
                Expr * ass1 = Expr_createAtomic(NEGATION);
                ass1->rightExpr = Expr_copy(pl->result->leftExpr);
                
                Proof_applyASSUMPTION(proof, ass1);
                int a1 = proof->nLines;
                
                Expr * ass2 = Expr_createAtomic(NEGATION);
                ass2->rightExpr = Expr_copy(pl->result->rightExpr);
                Proof_applyASSUMPTION(proof, Expr_copy(ass2));
                int a2 = proof->nLines;
                
                Proof_applyASSUMPTION(proof, Expr_copy(pl->result->leftExpr));
                int a3 = proof->nLines;
                
                Proof_applyNEGATION_ELIMINATION(proof, a3, a1);
                
                Proof_applyINDEPENDENT_ASSUMPTION(proof, 
                                                  Expr_copy(pl->result->rightExpr));
                int a4 = proof->nLines;
                
                Proof_applyNEGATION_ELIMINATION(proof, a4, a2);
                
                Proof_applyDISJUNCTION_ELIMINATION(proof, pl->mark, a3, a3 + 1,
                                                                    a4, a4 + 1);
                
                Proof_applyNEGATION_INTRODUCTION(proof, a2, proof->nLines);
                Proof_applyDOUBLE_NEGATION_ELIMINATION(proof, proof->nLines);
                Proof_applyIMPLICATION_INTRODUCTION(proof, a1, proof->nLines);
                
                stack->end = ProofLine_create(stack->end, 
                                         proof->end->level,
                                         proof->end->rule,
                                         Expr_copy(proof->end->result));
                stack->end->mark = proof->nLines;
                
                if (pl->prevLine != NULL) pl->prevLine->nextLine = pl->nextLine;
                if (pl->nextLine != NULL) pl->nextLine->prevLine = pl->prevLine;
                
                pl->prevLine = NULL;
                tmp = pl->nextLine;
                if (pl == stack->begin) stack->begin = pl->nextLine;
                pl->nextLine = NULL;
                ProofLine_delete(&pl);
                pl = tmp;        
            }
            else
            {
                Expr * ass1 = Expr_copy(pl->result->leftExpr->rightExpr);
                Proof_applyASSUMPTION(proof, ass1);
                int a1 = proof->nLines;
                
                Expr * ass2 = Expr_createAtomic(NEGATION);
                ass2->rightExpr = Expr_copy(pl->result->rightExpr);
                Proof_applyASSUMPTION(proof, Expr_copy(ass2));
                int a2 = proof->nLines;
                
                Proof_applyASSUMPTION(proof, Expr_copy(pl->result->leftExpr));
                int a3 = proof->nLines;
                
                Proof_applyNEGATION_ELIMINATION(proof, a1, a3);
                
                Proof_applyINDEPENDENT_ASSUMPTION(proof, 
                                                  Expr_copy(pl->result->rightExpr));
                int a4 = proof->nLines;
                
                Proof_applyNEGATION_ELIMINATION(proof, a4, a2);
                
                Proof_applyDISJUNCTION_ELIMINATION(proof, pl->mark, a3, a3 + 1,
                                                                    a4, a4 + 1);
                
                Proof_applyNEGATION_INTRODUCTION(proof, a2, proof->nLines);
                Proof_applyDOUBLE_NEGATION_ELIMINATION(proof, proof->nLines);
                Proof_applyIMPLICATION_INTRODUCTION(proof, a1, proof->nLines);
                
                stack->end = ProofLine_create(stack->end, 
                                         proof->end->level,
                                         proof->end->rule,
                                         Expr_copy(proof->end->result));
                stack->end->mark = proof->nLines;
                
                if (pl->prevLine != NULL) pl->prevLine->nextLine = pl->nextLine;
                if (pl->nextLine != NULL) pl->nextLine->prevLine = pl->prevLine;
                
                pl->prevLine = NULL;
                tmp = pl->nextLine;
                if (pl == stack->begin) stack->begin = pl->nextLine;
                pl->nextLine = NULL;
                ProofLine_delete(&pl);
                pl = tmp;                
            }
        }
    }
}

unsigned char eliminateContradictions()
{
    ProofLine * pl_1;
    ProofLine * pl_2;
    
    for(pl_1 = stack->begin; pl_1 != NULL; pl_1 = pl_1->nextLine)
    {
        if (Proof_isReachable(proof, pl_1->mark))
        {
            for(pl_2 = pl_1->nextLine; pl_2 != NULL; pl_2 = pl_2->nextLine)
            {
                if (Proof_isReachable(proof, pl_2->mark))
                {
                    if (pl_1->result->operation == NEGATION)
                    {
                        if (Expr_areEqual(pl_1->result->rightExpr, 
                                          pl_2->result))
                        {
                            Proof_applyNEGATION_ELIMINATION(proof, pl_2->mark, 
                                                                   pl_1->mark);
                            return 1;
                        }
                    }
                    else if (pl_2->result->operation == NEGATION)
                    {
                        if (Expr_areEqual(pl_2->result->rightExpr, 
                                          pl_1->result))
                        {
                            Proof_applyNEGATION_ELIMINATION(proof, pl_1->mark, 
                                                                   pl_2->mark);
                            return 1;
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

void eliminateImplications()
{
    ProofLine * pl_1;
    ProofLine * pl_2;
    ProofLine * tmp;
    
    for(pl_1 = stack->begin; pl_1 != NULL; pl_1 = pl_1->nextLine)
    {
        if (Proof_isReachable(proof, pl_1->mark) 
         && pl_1->result->operation == IMPLICATION)
        {
            for(pl_2 = stack->begin; pl_2 != NULL; pl_2 = pl_2->nextLine)
            {
                if (Proof_isReachable(proof, pl_2->mark))
                {
                    // Modus Ponens
                    if (Expr_areEqual(pl_1->result->leftExpr, pl_2->result))
                    {
                        Proof_applyIMPLICATION_ELIMINATION(proof, pl_1->mark,
                                                                  pl_2->mark);
                        stack->end = ProofLine_create(stack->end, 
                                                 proof->end->level,
                                                 proof->end->rule,
                                                 Expr_copy(proof->end->result));
                        stack->end->mark = proof->nLines;

                        if (pl_1->prevLine != NULL) 
                            pl_1->prevLine->nextLine = pl_1->nextLine;
                        if (pl_1->nextLine != NULL) 
                            pl_1->nextLine->prevLine = pl_1->prevLine;
                        
                        pl_1->prevLine = NULL;
                        tmp = pl_1->nextLine;
                        if (pl_1 == stack->begin) stack->begin = pl_1->nextLine;
                        pl_1->nextLine = NULL;
                        ProofLine_delete(&pl_1);
                        pl_1 = tmp;                        
                    }
                }
            }
        }
    }
}

void clearProofStack()
{
    Proof_delete(&stack);
    stack = Proof_create();
}

unsigned char derive(Expr * goal)
{
    if (goal == NULL) 
    {
        return 1;
    }
    else if (goal->operation == IMPLICATION)
    {
        Proof_applyASSUMPTION(proof, Expr_copy(goal->leftExpr));
        int assumption = proof->nLines;
        
        if (derive(goal->rightExpr))
        {
            int conclusion = proof->nLines;
            Proof_applyIMPLICATION_INTRODUCTION(proof, assumption, conclusion);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (goal->operation == AND)
    {
        if (derive(goal->leftExpr))
        {
            int left = proof->nLines;
            if (derive(goal->rightExpr))
            {
                int right = proof->nLines;
                Proof_applyCONJUNCTION_INTRODUCTION(proof, left, right);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else if (goal->operation == OR)
    {
        Proof_applyLEM(proof, Expr_copy(goal->leftExpr));
        int d = proof->nLines;
        
        Proof_applyASSUMPTION(proof, Expr_copy(goal->leftExpr));
        int a1 = proof->nLines;
        
        Proof_applyDISJUNCTION_INTRODUCTION1(proof, a1, 
                                                    Expr_copy(goal->rightExpr));
        int c1 = proof->nLines;
        
        Proof_applyINDEPENDENT_ASSUMPTION(proof, Expr_createAtomic(NEGATION));
        proof->end->result->rightExpr = Expr_copy(goal->leftExpr);
        int a2 = proof->nLines;
        
        if (derive(goal->rightExpr))
        {
            Proof_applyDISJUNCTION_INTRODUCTION2(proof, 
                                                 Expr_copy(goal->leftExpr), 
                                                 proof->nLines);
            int c2 = proof->nLines;
            
            Proof_applyDISJUNCTION_ELIMINATION(proof, d, a1, c1, a2, c2);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (goal->operation == NEGATION)
    {
        Proof_applyASSUMPTION(proof, Expr_copy(goal->rightExpr));
        int assumption = proof->nLines;
        
        if (derive(Expr_createAtomic(FALSUM)))
        {
            int falsum = proof->nLines;            
            Proof_applyNEGATION_INTRODUCTION(proof, assumption, falsum);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (goal->operation == LITERAL)
    {
        Expr * _assumption = Expr_createAtomic(NEGATION);
        _assumption->rightExpr = Expr_copy(goal);
        Proof_applyASSUMPTION(proof, _assumption);
        int assumption = proof->nLines;
        
        if (derive(Expr_createAtomic(FALSUM)))
        {
            int falsum = proof->nLines;
            Proof_applyNEGATION_INTRODUCTION(proof, assumption, falsum);
            
            int dbl_neg = proof->nLines;
            Proof_applyDOUBLE_NEGATION_ELIMINATION(proof, dbl_neg);
            
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (goal->operation == FALSUM)
    {
        fillProofStack();

        int prev_nLines;
        while (prev_nLines = proof->nLines)
        {
            if (eliminateContradictions()) {
                clearProofStack();
                return 1;
            }
            
            traverseProofStack();
            eliminateImplications();
            
            if (prev_nLines == proof->nLines) {
                clearProofStack();
                return 0;
            }
        }
    }
}
