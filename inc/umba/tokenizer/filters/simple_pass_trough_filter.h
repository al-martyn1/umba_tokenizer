/*! \file
    \brief Фильтр, просто пропускающий токены дальше. Пример простого фильтра
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
//! Тестовый фильтр, который ничего не делает
template<typename TokenizerType>
struct SimplePassTroughFilter
{

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type             = umba::tokenizer::payload_type                ;

protected:

    token_handler_type nextTokenHandler;


public:

    SimplePassTroughFilter(token_handler_type curTokenHandler)
    : nextTokenHandler(curTokenHandler)
    {}

    UMBA_RULE_OF_FIVE(SimplePassTroughFilter, default, default, default, default, default);

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (nextTokenHandler)
            return nextTokenHandler(lineStartFlag, payloadToken, b, e, parsedData, msg);
        return true;
    }

}; // struct SimplePassTroughFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



