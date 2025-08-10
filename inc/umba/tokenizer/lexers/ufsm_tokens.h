/*! \file
    \brief Определение токенов, специфичных для Umba State Machine
 */

#pragma once


//----------------------------------------------------------------------------
// 0x4000u
#define UFSM_TOKEN_SET_KEYWORDS                  UMBA_TOKENIZER_TOKEN_KEYWORD_SET0_FIRST
#define UFSM_TOKEN_SET_KEYWORDS_FIRST            UMBA_TOKENIZER_TOKEN_KEYWORD_SET0_FIRST
#define UFSM_TOKEN_SET_KEYWORDS_LAST             UMBA_TOKENIZER_TOKEN_KEYWORD_SET0_LAST

#define UFSM_TOKEN_SET_OPERATORS                 UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST
#define UFSM_TOKEN_SET_OPERATORS_FIRST           UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST
#define UFSM_TOKEN_SET_OPERATORS_LAST            ((UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST)+0x1FF)



#define UFSM_TOKEN_KWD_NAMESPACE                 ((UFSM_TOKEN_SET_KEYWORDS)+0x0000)
#define UFSM_TOKEN_KWD_FSM                       ((UFSM_TOKEN_SET_KEYWORDS)+0x0001)
#define UFSM_TOKEN_KWD_EVENTS                    ((UFSM_TOKEN_SET_KEYWORDS)+0x0002)
#define UFSM_TOKEN_KWD_ACTIONS                   ((UFSM_TOKEN_SET_KEYWORDS)+0x0003)
#define UFSM_TOKEN_KWD_STATES                    ((UFSM_TOKEN_SET_KEYWORDS)+0x0004)
#define UFSM_TOKEN_KWD_PREDICATES                ((UFSM_TOKEN_SET_KEYWORDS)+0x0005)
#define UFSM_TOKEN_KWD_TRANSITIONS               ((UFSM_TOKEN_SET_KEYWORDS)+0x0006)
#define UFSM_TOKEN_KWD_EXTERNAL                  ((UFSM_TOKEN_SET_KEYWORDS)+0x0007)
#define UFSM_TOKEN_KWD_INITIAL                   ((UFSM_TOKEN_SET_KEYWORDS)+0x0008)
#define UFSM_TOKEN_KWD_FINAL                     ((UFSM_TOKEN_SET_KEYWORDS)+0x0009)
#define UFSM_TOKEN_KWD_ENTER                     ((UFSM_TOKEN_SET_KEYWORDS)+0x000A)
#define UFSM_TOKEN_KWD_LEAVE                     ((UFSM_TOKEN_SET_KEYWORDS)+0x000B)
#define UFSM_TOKEN_KWD_SELF_ENTER                ((UFSM_TOKEN_SET_KEYWORDS)+0x000C)
#define UFSM_TOKEN_KWD_SELF_LEAVE                ((UFSM_TOKEN_SET_KEYWORDS)+0x000D)
#define UFSM_TOKEN_KWD_INHERITS                  ((UFSM_TOKEN_SET_KEYWORDS)+0x000E)
#define UFSM_TOKEN_KWD_VALID_FOR                 ((UFSM_TOKEN_SET_KEYWORDS)+0x000F)
#define UFSM_TOKEN_KWD_CALL_BEFORE               ((UFSM_TOKEN_SET_KEYWORDS)+0x0010)
#define UFSM_TOKEN_KWD_CALL_AFTER                ((UFSM_TOKEN_SET_KEYWORDS)+0x0011)



#define UFSM_TOKEN_OP_DESCR_FOLOWS               UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION   /* Description follows (also separator for kebab names) */
#define UFSM_TOKEN_OP_ANY                        UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION
#define UFSM_TOKEN_OP_NOT                        UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT
#define UFSM_TOKEN_OP_AND                        UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND
#define UFSM_TOKEN_OP_OR                         UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR 
#define UFSM_TOKEN_OP_NOT_ALTER                  UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT
#define UFSM_TOKEN_OP_AND_ALTER                  UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND
#define UFSM_TOKEN_OP_OR_ALTER                   UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR 
#define UFSM_TOKEN_OP_ASSIGN                     UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT
#define UFSM_TOKEN_OP_COLON                      UMBA_TOKENIZER_TOKEN_OPERATOR_COLON
#define UFSM_TOKEN_OP_COMMA                      UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA










// fsm(state-machine), events, external, actions, predicates, valid for, states
// initial, enter, leave
// self-enter
// self-leave
// inherits call before after
// transitions
//  
// final

// найси токены selfe/selfle (self-e/self-l/enter-self/enter-s/leave-self/leave-s)?


//----------------------------------------------------------------------------






