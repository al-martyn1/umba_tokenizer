#pragma once

#include "filter_base.h"


//----------------------------------------------------------------------------
// umba::tokenizer::filters::
namespace umba {
namespace tokenizer {
namespace filters {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Простой фильтр, который буферизирует одинаковые токены. Он хранит только итератор первого токена в последовательности
template<typename TokenizerType, umba::tokenizer::payload_type CollectingPayloadValue>
struct TokenCollectingFilter
{

    UMBA_TOKENIZER_TOKEN_FILTERS_DECLARE_USING_DEPENDENT_TYPES(TokenizerType);
    using payload_type      = umba::tokenizer::payload_type;
    using token_info_type   = TokenInfo<TokenizerType>;

protected:

    token_handler_type nextTokenHandler;

    token_info_type  sequenceStartInfo;

    bool reset(bool res=true)
    {
        sequenceStartInfo = token_info_type{};
        sequenceStartInfo.payloadToken = UMBA_TOKENIZER_TOKEN_NUL; // Для надёжности )
        return res;
    }

    void saveSequenceStartInfo(bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type parsedData)
    {
        UMBA_ASSERT(payloadToken!=UMBA_TOKENIZER_TOKEN_NUL); // если не равно, то всё нормас
        sequenceStartInfo = token_info_type{lineStartFlag, payloadToken, b, e, parsedData};
    }

    static
    bool isCollectingPayloadToken(payload_type payloadToken)
    {
        return payloadToken==CollectingPayloadValue; // UMBA_TOKENIZER_TOKEN_RAW_CHAR;
    }

    bool isCollectingMode() const
    {
        return isCollectingPayloadToken(sequenceStartInfo.payloadToken);
    }


    // По ссылке мы передаём token_parsed_data_type только для того, чтобы не плодить копии
    // По неконстантной ссылке - чтобы хэндлер могли поменять там значение и передать изменённое дальше
    // Возврат значения по ссылке тут у нас не используется

    // bool callNextTokenHandler( TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type &parsedData, messages_string_type &msg) const
    bool callNextTokenHandler( TokenizerType &tokenizer, bool lineStartFlag, payload_type payloadToken, iterator_type &b, iterator_type &e, token_parsed_data_type parsedData, messages_string_type &msg) const
    {
        if (!nextTokenHandler)
            return true;

        return nextTokenHandler(tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
    }

public:

    TokenCollectingFilter(token_handler_type curTokenHandler)
    : nextTokenHandler(curTokenHandler)
    {
        reset();
    }

    UMBA_RULE_OF_FIVE(TokenCollectingFilter, default, default, default, default, default);

    bool operator()( TokenizerType            &tokenizer
                   , bool                     lineStartFlag
                   , payload_type             payloadToken
                   , iterator_type            &b
                   , iterator_type            &e
                   , token_parsed_data_type   &parsedData // std::variant<...>
                   , messages_string_type     &msg
                   )
    {
        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_RST)
        {
            return reset(callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        if (payloadToken==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
        {
            if (isCollectingMode()) // режим коллекционирования токенов
            {
                // режим коллекционирования был включен - надо сбросить коллекцию
                auto tmpB = sequenceStartInfo.b; // начало коллекции - было сохранено
                auto tmpE = b;                   // конец коллекции - оно же начало FIN
    
                if (!callNextTokenHandler( tokenizer, sequenceStartInfo.lineStartFlag, sequenceStartInfo.payloadToken, tmpB, tmpE, sequenceStartInfo.parsedData, msg))
                {
                    // tmpB, tmpE могли быть скорректированы, надо их поместить в b и e для возврата
                    b = tmpB;
                    e = tmpE;
                    return reset(false);
                }
            }

            // Теперь, вне зависимости от предыдущего режима просто прокидываем FIN дальше
            return reset(callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
        }

        // FIN обработали, теперь нормальная работа

        if (isCollectingMode()==isCollectingPayloadToken(payloadToken))
        {
            // режим не меняется
            if (!isCollectingMode()) // не режим коллекционирования, просто прокидываем дальше
                return callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg);
            // в режиме коллекционирования ничего не делаем
            return true;
        }
        else
        {
            // режим меняется
            if (!isCollectingMode())
            {
                // был режим неколлекционирования
                // сохраняем данные начала последовательности
                saveSequenceStartInfo(lineStartFlag, payloadToken, b, e, parsedData);

                // Дальше прокидывать ничего не надо
                return true;
            }
            else
            {
                // был режим коллекционирования

                auto tmpB = sequenceStartInfo.b; // начало колелкции - было сохранено
                auto tmpE = b;                   // конец коллекции - оно же начало текущего

                // Пробрасываем коллекцию
                if (!callNextTokenHandler( tokenizer, sequenceStartInfo.lineStartFlag, sequenceStartInfo.payloadToken, tmpB, tmpE, sequenceStartInfo.parsedData, msg))
                {
                    // tmpB, tmpE могли быть скорректированы, надо их поместить в b и e для возврата
                    b = tmpB;
                    e = tmpE;
                    return reset(false);
                }

                // Теперь, вне зависимости от предыдущего режима просто прокидываем то, что пришло - дальше
                return reset(callNextTokenHandler( tokenizer, lineStartFlag, payloadToken, b, e, parsedData, msg));
            }

        }

    }

}; // struct TokenCollectingFilter

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace filters
} // namespace tokenizer
} // namespace umba



