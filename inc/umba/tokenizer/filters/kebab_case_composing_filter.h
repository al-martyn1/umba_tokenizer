#pragma once

#include "filter_base.h"


//----------------------------------------------------------------------------
// umba::tokenizer::filters::
namespace umba {
namespace tokenizer {
namespace filters {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! https://tproger.ru/explain/naming-variables-and-functions-use-them-all
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct KebabCaseComposingFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type              = umba::tokenizer::payload_type;
    using token_buffer_vector_type  = typename TBase::token_buffer_vector_type;
    // using string_type               = typename TokenizerType::string_type;

    string_type stringLiteralValueCollector;

    // First argument is index in sequence
    UMBA_RULE_OF_FIVE(KebabCaseComposingFilter, default, default, default, default, default);


    KebabCaseComposingFilter(token_handler_type curTokenHandler)
    : TBase(curTokenHandler)
    {
    }

    bool reset(bool res=true)
    {
        TBase::reset();
        return res;
    }

protected:


public:

    // UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION
    // UMBA_TOKENIZER_TOKEN_IDENTIFIER

    bool passComposedIdentifier( TokenizerType           &tokenizer
                               , iterator_type           &b
                               , iterator_type           &e
                               , messages_string_type    &msg
                               )
    {
        UMBA_ASSERT(!this->tokenBuffer.empty()); // точно ли ы буфере что-то есть?
        UMBA_ASSERT(this->tokenBuffer.front().payloadToken==UMBA_TOKENIZER_TOKEN_IDENTIFIER); // Начало kebab идентификатора должны быть идентификаторами

        //TokenInfo<KebabCaseComposingFilter> minusTokenInfo;
        typename TBase::token_buffer_vector_type::value_type minusTokenInfo;
        bool lastMinus = false;
        if (this->tokenBuffer.back().payloadToken==UMBA_TOKENIZER_TOKEN_IDENTIFIER)
        {
            lastMinus = false;
            minusTokenInfo = this->tokenBuffer.back();
            this->tokenBuffer.pop_back();
        }


        stringLiteralValueCollector.clear();

        //typename TokenizerType::StringLiteralData collectedStringLiteralData;
        typename token_buffer_vector_type::const_iterator it = this->tokenBuffer.begin();
        for(; it!=this->tokenBuffer.end(); ++it)
        {
            UMBA_ASSERT(it->payloadToken==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION || it->payloadToken==UMBA_TOKENIZER_TOKEN_IDENTIFIER);
            if (it->payloadToken==UMBA_TOKENIZER_TOKEN_IDENTIFIER)
            {
                auto stringLiteralData = std::get<typename TokenizerType::IdentifierDataHolder>(it->parsedData);
                stringLiteralValueCollector.append(stringLiteralData.pData->asString());
            }
            else
            {
                stringLiteralValueCollector.append(1,'-');
            }
        }

    //makeStringView(stringLiteralValueCollector.str())


        //auto 
        b = this->tokenBuffer.front().b;
        //auto 
        e = this->tokenBuffer.back ().e;
        bool lineStartFlag = this->tokenBuffer.front().lineStartFlag;
        this->clearTokenBuffer();
        typename TokenizerType::StringLiteralData collectedStringLiteralData;
        this->tokenBufferPushBack(lineStartFlag, UMBA_TOKENIZER_TOKEN_IDENTIFIER, b, e, typename TokenizerType::StringLiteralDataHolder(typename TokenizerType::StringLiteralData{stringLiteralValueCollector}));

        if (!this->flushTokenBuffer(tokenizer, msg, b, e)) // сбрасываем буферизированное (с очисткой буфера)
            return false;

        if (lastMinus)
        {
            this->tokenBuffer.push_back(minusTokenInfo);
            return this->flushTokenBuffer(tokenizer, msg, b, e);
        }
        
        return true;
    }



// struct TokenInfo
// {
//     UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
//     using payload_type             = umba::tokenizer::payload_type                ;
//  
//  
//     bool                                 lineStartFlag;
//     payload_type                         payloadToken;
//     iterator_type                        b;
//     iterator_type                        e;
//     // std::basic_string_view<value_type>   strValue;
//     token_parsed_data_type               parsedData;


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
            if (!this->tokenBuffer.empty() /* && this->tokenBuffer.back().payloadToken==UMBA_TOKENIZER_TOKEN_IDENTIFIER */ )
            {
                auto b_ = b; auto e_ = e;
                // буфер не пуст, можно склеивать (минус в конце разрешен, он будет отклеен)
                if (!passComposedIdentifier(tokenizer, b, e, msg))
                    return false;

                b = b_; e = e_;
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
            }
            else
            {
                return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
            }
        }

        if (payloadToken!=UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION && payloadToken!=UMBA_TOKENIZER_TOKEN_IDENTIFIER)
        {
            // У нас не идентификатор и не минус

            if (this->tokenBuffer.empty())
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);

            auto b_ = b; auto e_ = e;
            // буфер не пуст, можно склеивать (минус в конце разрешен, он будет отклеен)
            if (!passComposedIdentifier(tokenizer, b, e, msg))
                return false;

            b = b_; e = e_;
            return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION)
        {
            // Если буфер пустой - пробрасываем текущий минус дальше - идентификатор не может с него начинаться
            if (this->tokenBuffer.empty())
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);

            // Если в буфере не идентификатор, то минус, а он в конце разрешен, он будет отклеен. А двойной минус в кебабе нельзя
            if (this->tokenBuffer.back().payloadToken!=UMBA_TOKENIZER_TOKEN_IDENTIFIER)
            {
                auto b_ = b; auto e_ = e;
                if (!passComposedIdentifier(tokenizer, b, e, msg))
                    return false;

                b = b_; e = e_;
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
            }

            // Кладём в буфер
            this->tokenBufferPushBack(lineStartFlag, payloadToken, b, e, parsedData);
            return true;
        }

        // У нас - идентификатор, других вариантов нет

        // Буфер пуст или в конце - минус
        if (this->tokenBuffer.empty() || this->tokenBuffer.back().payloadToken==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION)
        {
            this->tokenBufferPushBack(lineStartFlag, payloadToken, b, e, parsedData);
            return true;
        }

        // Если в буфере не минус или он пуст, это косяк, флушим и пробрасываем текущий минус дальше

        return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );

    }

}; // struct KebabCaseComposingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



