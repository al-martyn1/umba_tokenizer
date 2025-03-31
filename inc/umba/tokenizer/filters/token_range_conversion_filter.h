#pragma once

#include "filter_base.h"


//----------------------------------------------------------------------------
// umba::tokenizer::filters::
namespace umba {
namespace tokenizer {
namespace filters {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Конвертирует токены из диапазона в один токен, и устанавливает нагрузку в размер офсета от стартового, добавяя дополнительное смещение
// Предназначено для операторов переменной длины - сводим к одному токену, а длину загружаем в token_parsed_data_type parsedData
// Минимальная длина - 1 (это фиксированный офсет по умолчанию)
template<typename TokenizerType, typename VectorType=std::vector<TokenInfo<TokenizerType> > >
struct TokenRangeConversionFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;
    // using string_type              = typename TokenizerType::string_type  ;

    payload_type     rangeBegin  ;
    payload_type     rangeSize   ; 
    payload_type     targetToken ; 
    std::size_t      lenghtExtra ;


    UMBA_RULE_OF_FIVE(TokenRangeConversionFilter, default, default, default, default, default);

    TokenRangeConversionFilter( token_handler_type                                  curTokenHandler_
                              , payload_type                                        rangeBegin_
                              , payload_type                                        rangeSize_
                              , payload_type                                        targetToken_
                              , std::size_t                                         lenghtExtra_      = 0
                              )
    : TBase(curTokenHandler_  )
    , rangeBegin (rangeBegin_ )
    , rangeSize  (rangeSize_  )
    , targetToken(targetToken_)
    , lenghtExtra(lenghtExtra_)
    {
    }

    bool reset(bool res = true)
    {
        return TBase::reset(res);
    }

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
            return this->callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        if (payloadToken>=rangeBegin && payloadToken<(rangeBegin+rangeSize))
        {
            std::size_t lenght = payloadToken-rangeBegin+1;
            lenght += lenghtExtra;

            payloadToken = targetToken;

            typename TokenizerType::IntegerNumericLiteralData lenghtData;

            lenghtData.value       = lenght;
            lenghtData.fOverflow   = false ;
            lenghtData.hasSuffix   = false ;
            // lenghtData.suffixStartPos - ничего не задаём

            parsedData = lenghtData;
        }

        return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
    }


}; // struct TokenRangeConversionFilter

//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



