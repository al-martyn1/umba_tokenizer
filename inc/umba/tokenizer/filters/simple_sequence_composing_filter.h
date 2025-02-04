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
struct SimpleSequenceComposingFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;

    // First argument is index in sequence
    using extra_check_function_type = std::function<bool(std::size_t,TokenizerType&,bool,payload_type,iterator_type,iterator_type,token_parsed_data_type&)>;

    UMBA_RULE_OF_FIVE(SimpleSequenceComposingFilter, default, default, default, default, default);


    payload_type                resultPayloadToken = 0;
    std::size_t                 resultPayloadDataIndex   = 0;
    std::vector<payload_type>   payloadsMatchList;
    extra_check_function_type   extraCheckFunction;

    // https://www.scs.stanford.edu/~dm/blog/param-pack.html
    // https://selfboot.cn/2024/05/07/variadic_arguments_in_c++/
    // https://dev.to/pauljlucas/variadic-functions-in-c-2alg

    SimpleSequenceComposingFilter( token_handler_type                   curTokenHandler
                                 , payload_type                         resultPayloadToken_
                                 , std::size_t                          resultPayloadDataIndex_ /* обычно 0 */
                                 , const std::vector<payload_type>      &payloadsList_
                                 )
    : TBase(curTokenHandler)
    , resultPayloadToken(resultPayloadToken_)
    , resultPayloadDataIndex(resultPayloadDataIndex_)
    , payloadsMatchList(payloadsList_)
    {
        UMBA_ASSERT(!payloadsMatchList.empty());
        UMBA_ASSERT(resultPayloadDataIndex<payloadsMatchList.size());
    }

    SimpleSequenceComposingFilter( token_handler_type                   curTokenHandler
                                 , payload_type                         resultPayloadToken_
                                 , std::size_t                          resultPayloadDataIndex_ /* обычно 0 */
                                 , const std::vector<payload_type>      &payloadsList_
                                 , extra_check_function_type            extraCheckFunction_
                                 )
    : TBase(curTokenHandler)
    , resultPayloadToken(resultPayloadToken_)
    , resultPayloadDataIndex(resultPayloadDataIndex_)
    , payloadsMatchList(payloadsList_)
    , extraCheckFunction(extraCheckFunction_)
    {
        UMBA_ASSERT(!payloadsMatchList.empty());
        UMBA_ASSERT(resultPayloadDataIndex<payloadsMatchList.size());
    }

    bool reset(bool res=true)
    {
        TBase::reset();
        return res;
    }

protected:


public:

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
            return reset(this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
        }

        // this->tokenBuffer.size() хранит всегода меньше элементов, чем в образце payloadsList.size()
        UMBA_ASSERT(this->tokenBuffer.size()<payloadsMatchList.size()); // если payloadsList пустой - тоже выстрелит

        // Тут надо матчер вызывать
        if (payloadsMatchList[this->tokenBuffer.size()]!=payloadToken)
        {
            // пришедшая нагрузка не соответствует паттерну
            // флушим, прокидываем текущие аргументы и очищаем буфер
            return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
        }

        if (extraCheckFunction)
        {
            // using extra_check_function_type = std::function<bool(std::size_t,TokenizerType&,bool,payload_type,iterator_type,iterator_type,token_parsed_data_type)>;
            if (!extraCheckFunction(this->tokenBuffer.size(), tokenizer, lineStartFlag, payloadToken, b, e, parsedData))
            {
                // пришедшая нагрузка не прошла дополнительную проверку
                // флушим, прокидываем текущие аргументы и очищаем буфер
                return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
            }
        }

        // Иначе - кладём в буфер
        this->tokenBufferPushBack(lineStartFlag, payloadToken, b, e, parsedData);

        if (this->tokenBuffer.size()<payloadsMatchList.size())
            return true; // Ещё не набрали полностью паттерн


        auto tki = this->tokenBuffer.front();
        //auto tmpB = this->tokenBuffer.front().b;
        if (!this->callNextTokenHandler(tokenizer, tki.lineStartFlag, resultPayloadToken, tki.b, e, this->tokenBuffer[resultPayloadDataIndex].parsedData /*strValue*/, msg))
        {
             b = tki.b;
             this->reset();
             return false;
        }

        this->reset();
        return true;

    }

}; // struct SimpleSequenceComposingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



