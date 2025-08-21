/*! \file
    \brief Фильтры DblSquareBracketOpenComposingFilter и DblSquareBracketCloseComposingFilter - преобразуют двойные токены [[ и ]] в одинарные
 */
#pragma once

#include "filter_base.h"


//----------------------------------------------------------------------------
// umba::tokenizer::filters::
namespace umba {
namespace tokenizer {
namespace filters {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define UMBA_TOKENIZER_DECLARE_REPEATED_TOKEN_COMPOSING_FILTER_CLASS(FilterClassName, resultPayloadToken, nRepetitions, srcPayloadToken)              \
            template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >                                             \
            struct FilterClassName : umba::tokenizer::filters::RepeatedTokenComposingFilter<TokenizerType, VectorType>                                \
            {                                                                                                                                         \
                using TBase = umba::tokenizer::filters::RepeatedTokenComposingFilter<TokenizerType, VectorType>;                                      \
                UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);                                                                    \
                using payload_type             = umba::tokenizer::payload_type        ;                                                               \
                UMBA_RULE_OF_FIVE(FilterClassName, delete, default, default, default, default);                                                       \
                                                                                                                                                      \
                FilterClassName(token_handler_type curTokenHandler)                                                                                   \
                : TBase(curTokenHandler, resultPayloadToken, nRepetitions, srcPayloadToken)                                                           \
                {}                                                                                                                                    \
            }

//----------------------------------------------------------------------------

UMBA_TOKENIZER_DECLARE_REPEATED_TOKEN_COMPOSING_FILTER_CLASS(DblSquareBracketOpenComposingFilter , UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN , 2, UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN );
UMBA_TOKENIZER_DECLARE_REPEATED_TOKEN_COMPOSING_FILTER_CLASS(DblSquareBracketCloseComposingFilter, UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_CLOSE, 2, UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE);

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



