#ifndef PROOF_H
#define PROOF_H

#include "Expr.h"

#define DEPENDENCY_SIZE 5

#define PROOF_APPLY(Proof, Rule, ...) \
    Proof_apply##Rule (Proof, __VA_ARGS__)

typedef enum {
    PREMISE,
    ASSUMPTION,
    INDEPENDENT_ASSUMPTION,
    CONJUNCTION_INTRODUCTION,
    CONJUNCTION_ELIMINATION1,
    CONJUNCTION_ELIMINATION2,
    DISJUNCTION_INTRODUCTION1,
    DISJUNCTION_INTRODUCTION2,
    DISJUNCTION_ELIMINATION,
    IMPLICATION_INTRODUCTION,
    IMPLICATION_ELIMINATION,
    NEGATION_INTRODUCTION,
    NEGATION_ELIMINATION,
    DOUBLE_NEGATION_INTRODUCTION,
    DOUBLE_NEGATION_ELIMINATION,
    LEM
} Rule;

typedef struct ProofLineBody {
    struct ProofLineBody * prevLine;
    unsigned int level;
    Rule rule;
    unsigned int dependencies[DEPENDENCY_SIZE];
    Expr * result;
    unsigned char isReachable;
    int mark;
    struct ProofLineBody * nextLine;
} ProofLine;

ProofLine * ProofLine_create
    (ProofLine * prev, unsigned int level, Rule rule, Expr * result);

void ProofLine_print_short(unsigned int lineNo, ProofLine * pl, FILE * stream);
void ProofLine_print_long(unsigned int lineNo, ProofLine * pl, FILE * stream);

void ProofLine_delete(ProofLine ** plPtr);

typedef struct {
    unsigned int nLines;
    ProofLine * begin;
    ProofLine * end;
} Proof;

Proof * Proof_create();

unsigned char Proof_isReachable(Proof * proof, unsigned int lineNo);

void Proof_applyPREMISE(Proof * proof, Expr * premise);

void Proof_applyASSUMPTION(Proof * proof, Expr * assumption);

void Proof_applyCONJUNCTION_INTRODUCTION
    (Proof * proof, unsigned int left, unsigned int right);
void Proof_applyCONJUNCTION_ELIMINATION1
    (Proof * proof, unsigned int conjunction);
void Proof_applyCONJUNCTION_ELIMINATION2
    (Proof * proof, unsigned int conjunction);
    
void Proof_applyDISJUNCTION_INTRODUCTION1
    (Proof * proof, unsigned int left, Expr * right);
void Proof_applyDISJUNCTION_INTRODUCTION2
    (Proof * proof, Expr * left, unsigned int right);
void Proof_applyDISJUNCTION_ELIMINATION
    (Proof * proof, unsigned int disjunction,
                    unsigned int assumption1, 
                    unsigned int conclusion1,
                    unsigned int assumption2,
                    unsigned int conclusion2);

void Proof_applyIMPLICATION_INTRODUCTION
    (Proof * proof, unsigned int assumption, unsigned int conclusion);
void Proof_applyIMPLICATION_ELIMINATION
    (Proof * proof, unsigned int implication, unsigned int antecedant);
                                        
void Proof_applyNEGATION_ELIMINATION
    (Proof * proof, unsigned int expr, unsigned int negExpr);
void Proof_applyNEGATION_INTRODUCTION
    (Proof * proof, unsigned int assumption, unsigned int falsum);

void Proof_applyDOUBLE_NEGATION_INTRODUCTION
    (Proof * proof, unsigned int expr);
void Proof_applyDOUBLE_NEGATION_ELIMINATION
    (Proof * proof, unsigned int expr);

void Proof_applyLEM(Proof * proof, Expr * expr);

void Proof_print_short(Proof * proof, FILE * stream);
void Proof_print_long(Proof * proof, FILE * stream);

void Proof_deleteLastLine(Proof * proof);
void Proof_delete(Proof ** proofPtr);

#endif
