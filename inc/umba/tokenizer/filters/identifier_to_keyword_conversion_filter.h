/*! \file
    \brief Фильтр для преобразования идентификаторов в различные токены по заданной таблице
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
struct IdentifierToKeywordConversionFilter : FilterBase<TokenizerType, VectorType>
{
    using TBase = FilterBase<TokenizerType, VectorType>;

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TBase);
    using payload_type             = umba::tokenizer::payload_type        ;
    // using string_type              = typename TokenizerType::string_type  ;

    UMBA_RULE_OF_FIVE(IdentifierToKeywordConversionFilter, default, default, default, default, default);

    // Config
    payload_type                                  matchTo            = UMBA_TOKENIZER_TOKEN_NUL;
    std::unordered_map<string_type, payload_type> keywordsMap;
    bool                                          caseSensitive      = true;

    payload_type                                  contextToken       = UMBA_TOKENIZER_TOKEN_NUL;

    // State
    bool                                          contextTokenFound  = false;

    IdentifierToKeywordConversionFilter( token_handler_type                                  curTokenHandler
                                       , payload_type                                        matchTo_
                                       , const std::unordered_map<string_type, payload_type> keywordsMap_
                                       , bool                                                caseSensitive_=true
                                 )
    : TBase(curTokenHandler)
    , matchTo      (matchTo_      )
    , keywordsMap  (keywordsMap_  )
    , caseSensitive(caseSensitive_)
    {
    }

    IdentifierToKeywordConversionFilter( token_handler_type                                  curTokenHandler
                                       , payload_type                                        matchTo_
                                       , payload_type                                        contextToken_
                                       , const std::unordered_map<string_type, payload_type> keywordsMap_
                                       , bool                                                caseSensitive_=true
                                 )
    : TBase(curTokenHandler)
    , matchTo      (matchTo_      )
    , keywordsMap  (keywordsMap_  )
    , caseSensitive(caseSensitive_)
    , contextToken (contextToken_ )
    {
    }

    bool reset(bool res = true)
    {
        contextTokenFound  = false;
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

        if (contextToken!=UMBA_TOKENIZER_TOKEN_NUL)
        {
            // Используется контекст для преобразования
            if (!contextTokenFound)
            {
                // Контекст пока не найден
                if (payloadToken==contextToken)
                   contextTokenFound = true;

                // Просто прокидываем дальше
                return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
            }
            else // Контекст найден ранее
            {
                if ( payloadToken==UMBA_TOKENIZER_TOKEN_LINEFEED
                  || payloadToken==UMBA_TOKENIZER_TOKEN_SPACE
                  || payloadToken==UMBA_TOKENIZER_TOKEN_TAB
                  || payloadToken==UMBA_TOKENIZER_TOKEN_FORM_FEED
                  || payloadToken==UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION
                  || payloadToken==UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT
                  || (payloadToken>=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST && payloadToken<=UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST)
                  //|| (payloadToken>= && payloadToken<=)
                  //|| payloadToken==
                   )
                {
                    // Эти токены просто прокидываем дальше даже пр наличии контекста
                    return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
                }

                // Дальше используем обычную обработку
            }
        }

        // В любом случае сбрасываем признак, что ранее был найден контекстный токен
        contextTokenFound = false;

        if (payloadToken!=matchTo)
        {
            if (payloadToken==contextToken) // Но токен является контекстным, устанавливаем признак
               contextTokenFound = true;
            return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }

        auto identifierData = std::get<typename TokenizerType::IdentifierDataHolder>(parsedData);
        string_type identifierStr = string_type(identifierData.pData->value);
        //string_type identifier = umba::string_plus::make_string<string_type>(umba::iterator::makeString(tringLiteralData.suffixStartPos, itEnd));

        if (!caseSensitive)
            umba::string_plus::tolower(identifierStr);

        typename std::unordered_map<string_type, payload_type>::const_iterator kit = keywordsMap.find(identifierStr);

        if (kit==keywordsMap.end())
        {
            // Преобразование не найдено
            // Поэтому проверяем текущий токен, вдруг он контекстный
            if (contextToken!=UMBA_TOKENIZER_TOKEN_NUL && payloadToken==contextToken)
            {
                contextTokenFound = true;
            }
            return this->callNextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
        }
        else
        {
            return this->callNextTokenHandler(tokenizer, lineStartFlag, kit->second, b, e, parsedData, msg);
        }

    }


}; // struct IdentifierToKeywordConversionFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



