/*! \file
    \brief Фильтр простой замены одного токена на другой. Нагрузка не меняется
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
struct SimpleReplaceFilter
{

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type             = umba::tokenizer::payload_type                ;

protected:

    token_handler_type nextTokenHandler;
    payload_type  compareToPayloadToken   = 0;
    payload_type  replaceWithPayloadToken = 0;

    bool callNextTokenHandler( TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type &parsedData, messages_string_type &msg) const
    {
        if (!nextTokenHandler)
            return true;

        return nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
    }

public:

    SimpleReplaceFilter( token_handler_type curTokenHandler
                       , payload_type cmpTo
                       , payload_type replaceWith
                       )
    : nextTokenHandler(curTokenHandler)
    , compareToPayloadToken  (cmpTo      )
    , replaceWithPayloadToken(replaceWith)
    {}


    UMBA_RULE_OF_FIVE(SimpleReplaceFilter, default, default, default, default, default);

    bool operator()( TokenizerType           &tokenizer
                   , bool                    lineStartFlag
                   , payload_type            payloadToken
                   , iterator_type           &b
                   , iterator_type           &e
                   , token_parsed_data_type  &parsedData // std::variant<...>
                   , messages_string_type    &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            //return reset(this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
            return this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            //return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
            return this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        if (payloadToken==compareToPayloadToken)
        {
            return this->callNextTokenHandler( tokenizer, lineStartFlag, replaceWithPayloadToken, b, e, parsedData, msg);
        }

        return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg);
    }

}; // struct SimpleReplaceFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



