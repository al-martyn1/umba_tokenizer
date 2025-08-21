/*! \file
    \brief Фильтр для преобразования простой последовательности из N токенов в единственный токен
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
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct RepeatedTokenComposingFilter : SimpleSequenceComposingFilter<TokenizerType, VectorType>
{
    using TBase = SimpleSequenceComposingFilter<TokenizerType, VectorType>;
    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;
    UMBA_RULE_OF_FIVE(RepeatedTokenComposingFilter, delete, default, default, default, default);

    RepeatedTokenComposingFilter( token_handler_type curTokenHandler
                                , payload_type       resultPayloadToken
                                , std::size_t        nRepetitions
                                , payload_type       srcPayloadToken
                                )
    : TBase( curTokenHandler
           , resultPayloadToken
           , 0 // данные берем из первого токена, всё равно это обычно для всяких скобок будет использоваться
           , std::vector<payload_type>(nRepetitions, srcPayloadToken)
           )
    {}

}; // struct RepeatedTokenComposingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



