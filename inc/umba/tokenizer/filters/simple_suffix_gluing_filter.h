/*! \file
    \brief Фильтр, "приклеивающий" идентификатор-суффикс к строковому или числовому литералу
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
struct SimpleSuffixGluingFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;

    UMBA_RULE_OF_FIVE(SimpleSuffixGluingFilter, default, default, default, default, default);


    enum class GluingOption
    {
        glueAll,
        glueNumbers,
        glueStrings
    };

    GluingOption gluingOption = GluingOption::glueAll;


    SimpleSuffixGluingFilter(token_handler_type curTokenHandler, GluingOption gOpt=GluingOption::glueAll)
    : TBase(curTokenHandler), gluingOption(gOpt)
    {}


protected:

    static
    bool isNumberLiteral(payload_type payloadToken)
    {
        return payloadToken>=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST && payloadToken<=UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_LAST;
    }

    static
    bool isStringLiteral(payload_type payloadToken)
    {
        return payloadToken>=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_FIRST && payloadToken<=UMBA_TOKENIZER_TOKEN_STRING_LITERAL_LAST;
    }

    bool glueNumbers() const
    {
        return gluingOption==GluingOption::glueAll || gluingOption==GluingOption::glueNumbers;
    }

    bool glueStrings() const
    {
        return gluingOption==GluingOption::glueAll || gluingOption==GluingOption::glueStrings;
    }


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
            return this->reset(this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            return this->flushTokenBufferAndCallNextHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg, true /* bClear */ );
        }

        if (this->tokenBuffer.empty())
        {
            if (isNumberLiteral(payloadToken) && glueNumbers()) // Если числовой литерал и разрешено приклеивать к ним суффиксы
            {
                this->tokenBuffer.emplace_back(lineStartFlag, payloadToken, b, e, parsedData /* strValue */ );
                return true;
            }
            else if (isStringLiteral(payloadToken) && glueStrings()) // Если строковый литерал и разрешено приклеивать к ним суффиксы
            {
                this->tokenBuffer.emplace_back(lineStartFlag, payloadToken, b, e, parsedData /* strValue */ );
                return true;
            }
            else
            {
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg);
            }
        }

        // В буфере что-то есть

        if (payloadToken!=UMBA_TOKENIZER_TOKEN_IDENTIFIER) // Что у нас пришло?
        {
            // У нас пришел не идентификатор, значит, склейка не состоится
            if (!this->flushTokenBuffer(tokenizer, msg, b, e)) // сбрасываем буферизированное (с очисткой буфера)
                 return false;

            // Пробрасываем пришедшее на текущем шаге
            return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData /* strValue */ , msg);
        }


        // в буфере лежит токен числового литерала

        UMBA_ASSERT(!this->tokenBuffer.empty()); // точно ли ы буфере что-то есть?
        TokenInfo prevTokenInfo = this->tokenBuffer[0]; // инфа по числовому или строковому литералу
        UMBA_ASSERT(prevTokenInfo.e==b); // Начало текущего токена должно совпадать с концом предыдущего

        auto literalStartIter = prevTokenInfo.b;
        auto suffixStartIter  = prevTokenInfo.e; // началом суффикса является конец числового литерала
        auto literalEndIter   = e;

        auto updatePayloadDataAndCallNextHandler = [&](auto payloadData) -> bool
        {
            payloadData.pData->hasSuffix      = true;
            payloadData.pData->suffixStartPos = suffixStartIter; // .getOffsetFromStart();
            payloadData.pData->suffixEndPos   = literalEndIter ; // .getOffsetFromStart();
            // !!! Зачем я тут копию делаю?
            token_parsed_data_type payloadDataCopy = payloadData;
            return this->callNextTokenHandler( tokenizer, prevTokenInfo.lineStartFlag, prevTokenInfo.payloadToken
                                         , literalStartIter, literalEndIter
                                         , payloadDataCopy
                                         , msg
                                         );
        };


        bool res = true;

        if (isNumberLiteral(prevTokenInfo.payloadToken))
        {
            if (prevTokenInfo.payloadToken&UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
            {
                auto tmp = std::get<typename TokenizerType::FloatNumericLiteralDataHolder>(prevTokenInfo.parsedData);
                res = updatePayloadDataAndCallNextHandler(tmp);
            }
            else
            {
                auto tmp = std::get<typename TokenizerType::IntegerNumericLiteralDataHolder>(prevTokenInfo.parsedData);
                res = updatePayloadDataAndCallNextHandler(tmp);
            }
        }
        else if (isStringLiteral(prevTokenInfo.payloadToken))
        {
            auto tmp = std::get<typename TokenizerType::StringLiteralDataHolder>(prevTokenInfo.parsedData);
            res = updatePayloadDataAndCallNextHandler(tmp);
        }
        else
        {
            UMBA_ASSERT(0);
        }

        this->clearTokenBuffer();
        return res;

    }

}; // struct SimpleSuffixGluingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



