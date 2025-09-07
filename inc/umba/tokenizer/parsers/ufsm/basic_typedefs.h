/*! \file
    \brief Базовые typedef'ы для парсера ufsm
 */

#pragma once

#include "enums.h"
// 
#include "marty_expressions/simple_bool_expressions.h"
#include "marty_expressions/tokenizer_helpers.h"

//
#include "marty_containers/insertion_ordered_map.h"

//
#include <algorithm>
#include <string>
#include <vector>

//
#include "../../../undef_min_max.h"


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
using PositionInfo     =  /* umba::tokenizer:: */ FullTokenPosition;

//----------------------------------------------------------------------------
using LogicExpressionInputItem = marty::expressions::ExpressionInputItem< PositionInfo
                                                                        , umba::tokenizer::payload_type // OperatorTokenType
                                                                        , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                                        , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                                        , std::string // StringType
                                                                        >;
// Use marty::expressions::tokenizer_helpers::convertTokenizerEvent to convert tokenizer events to LogicExpressionInputItem

//----------------------------------------------------------------------------
using LogicExpressionItem = marty::expressions::ExpressionItem< PositionInfo
                                                              , umba::tokenizer::payload_type // OperatorTokenType
                                                              , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                              , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                              , std::string // StringType
                                                              >;

//----------------------------------------------------------------------------
using LogicExpression  = marty::expressions::ExpressionNode< PositionInfo
                                                           , umba::tokenizer::payload_type // OperatorTokenType
                                                           , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                           , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                           , std::string // StringType
                                                           >;

//----------------------------------------------------------------------------
using LogicExpressionOperatorTraits = marty::expressions::SimpleBoolExpressionOperatorTraits<umba::tokenizer::payload_type /* OperatorTokenType*/ >;

//----------------------------------------------------------------------------
using LogicExpressionEvaluator = marty::expressions::SimpleBoolExpressionEvaluator< LogicExpressionOperatorTraits
                                                                                  , PositionInfo
                                                                                  , umba::tokenizer::payload_type // OperatorTokenType
                                                                                  , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                                                  , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                                                  , std::string // StringType
                                                                                  >;
/*
  LogicExpressionEvaluator methods
    ExpressionNodeType performConstantAbsorption(const ExpressionNodeType &node) const
    template<typename ValueGetter>
    bool evaluate(const ExpressionNodeType &node, ValueGetter valueGetter) const
    template<typename ValueMap>
    bool evaluateMap(const ExpressionNodeType &node, const ValueMap &mVals) const
    std::set<StringType> getExpressionVars(ExpressionNodeType node) const
    TruthTable<StringType> makeTruthTable(const ExpressionNodeType &node) const
    TruthTable<StringType> makeTruthTable(const ExpressionNodeType &node, typename TruthTable<StringType>::PayloadType plt) const
    StringType toString( const ExpressionNodeType &node
                       , bool useNamedOps=false // Также вместо 0/1 выводит false/true
                       ) const
*/

//----------------------------------------------------------------------------
using LogicExpressionParser = marty::expressions::SimpleBoolExpressionParser< LogicExpressionOperatorTraits
                                                                            , PositionInfo
                                                                            , umba::tokenizer::payload_type // OperatorTokenType
                                                                            , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                                            , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                                            , std::string // StringType
                                                                            >;
/*
  LogicExpressionParser methods
    SimpleBoolExpressionParser(const OperatorTraitsType &opTraits) : m_opTraits(opTraits)
    ExpressionNodeType getExpression() const
    std::string getErrorMessage(Error err, bool addErrCode=true) const
    std::string getErrorMessage(bool addErrCode=true) const
    Error initialize()
    Error finalize()
    Error parse(const ExpressionInputItemType &tk_)

*/

using LogicExpressionOperatorTraits = marty::expressions::SimpleBoolExpressionOperatorTraits<umba::tokenizer::payload_type  /* OperatorTokenType */ >;

inline
LogicExpressionOperatorTraits makeLogicExpressionOperatorTraits()
{
    LogicExpressionOperatorTraits traits;
    traits.openBracketOp  = UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN  ;
    traits.closeBracketOp = UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE ;
    traits.notOp          = UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT;
    traits.andOp          = UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND;
    traits.orOp           = UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR ;

    return traits;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


