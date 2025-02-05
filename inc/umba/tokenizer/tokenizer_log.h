/*! \file
    \brief Подсистема протоколирования для токенизеров-парсеров
 */

#pragma once

#include "umba/umba.h"
//
#include "umba/simple_formatter.h"
//
#include "umba/rule_of_five.h"
#include "umba/utility.h"
#include "umba/text_position_info.h"
#include "umba/format_message.h"
#include "umba/escape_string.h"
#include "umba/iterator.h"
//
#include "umba/log.h"
//
// #include "protogen-types.h"

//
#include <string>
#include <memory>
#include <algorithm>
#include <utility>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// umba::tokenizer::log::
namespace umba {
namespace tokenizer {
namespace log
{

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*! @enum umba::tokenizer::log::ParserErrorEventType

    Типы ошибок, которые генерирует токенизер (лексический анализатор).
    Пользовательские ошибки, возникающие при разборе потока токенов 
    (при синтаксическом анализе), надо выводить с использованием
    значения `customError`. При этом необходимые для форматирования величины
    следует помещать в поле `formatArgs` объекта лога.

    Тип выводимого сообщения и его идентификатор зависят от значений данного перечисления.
    При выводе `customError` идентификатор сообщения должен быть помещён в поле `formatArgs`
    объекта лога по ключу `msg-id`.

    Значение по ключу `Value` используется для вывода "ошибочного" значения, его либо заполняет 
    токенизатор, либо, для пользовательских сообщений, заполнаяет пользователь.

    `msg-id` изначально указывается через параметр `customMsgId`, а поле `formatArgs` позволяет его переопрделить.


    @var ParserErrorEventType::customError
    Пользовательские сообщения об ошибках - FormatMessage(customMessage).values(formatArgs).arg("Value", erroneousValue)

    @var ParserErrorEventType::customWarning
    Пользовательские предупреждения - FormatMessage(customMessage).values(formatArgs).arg("Value", erroneousValue)

    @var ParserErrorEventType::unexpected
    Что-то неожиданное - FormatMessage("unexpected symbol: \'$(Value)\'").arg("Value", erroneousValue)

    @var ParserErrorEventType::unknownOperator
    Неизвестный оператор - FormatMessage("unknown operator: '$(Value)'").arg("Value", erroneousValue)

    @var ParserErrorEventType::stringLiteralWarning
    Предупреждение в строковом литерале - например, неизвестная escape-последовательность
    FormatMessage("string literal, suspicious character value: '$(Value)': $(Message)").arg("Value", erroneousValue).arg("Message", customMessage)

    @var ParserErrorEventType::stringLiteralError
    Ошибка в строковом литерале
    FormatMessage("string literal, invalid character value: '$(Value)': $(Message)")
    .arg("Value", erroneousValue)
    .arg("Message", customMessage) // customMsgId - for translation

    @var ParserErrorEventType::tokenError
    Сообщение, если обработчик токенов вернул false
    Пользователь может тут задавать свою ошибку - FormatMessage(customMessage).values(formatArgs).arg("Value", erroneousValue)


*/
enum class ParserErrorEventType
{
    customError          ,
    customWarning        ,
    unexpected           ,
    unknownOperator      , // warning/info report
    stringLiteralWarning , // warning/info report
    stringLiteralError   ,
    tokenError
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//! Интерфейс логгера ошибок разбора. 
struct ParserErrorLog
{

    virtual ~ParserErrorLog() {}

    using FormatArgsMapType = typename umba::FormatMessage<std::string>::macros_map_type;

    FormatArgsMapType formatArgs; // Заполняется в парсере, используется при eventType == ParserErrorEventType::tokenError


    virtual
    void logErrorEvent( ParserErrorEventType          eventType
                      , const TextPositionInfo        &textPos
                      , umba::tokenizer::payload_type payload
                      , std::string                   erroneousValue     // Can be empty, can contain non-printable chars 
                      , std::string                   erroneousLineText  // Can be empty, can contain non-printable chars 
                      , const std::string             &customMsgId       // Can be empty, used only for customError and customWarning, can be overriden by 'msg-id' value
                      , const std::string             &customMessage     // Can be empty, used only for tokenError, customError and customWarning
                      , const char* srcFile,          int srcLine
                      ) const
                      = 0;

    //----------------------------------------------------------------------------
    // Хелперы для текстового вывода

    //----------------------------------------------------------------------------
    // Корректирует позицию, если из исходного символа 
    static
    std::string escapeStringAndCorrectPos( const std::string &str, std::size_t &pos )
    {
        std::string res; res.reserve(str.size());

        auto appendEscapeHelper = [&](const std::string &s)
        {
            if (s.size()>1 && res.size()<pos)
            {
                pos += s.size()-1; // Корректируем позицию если вставляется больше одного символа, но только если ещё не дошли до позиции
            }

            res.append(s);
        };

        auto makeOctalEscStr = [](unsigned uch)
        {
            char buf[5] = { 0 };
            buf[0] = '\\';
            for(auto i=0u; i!=3; ++i)
            {
                buf[i+1] = '0'+(char)(uch%8);
                uch /= 8;
            }

            std::reverse(&buf[1], &buf[4]);
            return std::string(&buf[0]);
        };

        // TODO: Тут надо бы сделать итерацию по символам текста, а не по char'ам
        for(auto ch: str)
        {
            switch(ch)
            {
                case '\'': appendEscapeHelper("\\'" ); break;
                case '\"': appendEscapeHelper("\\\""); break;
                case '\\': appendEscapeHelper("\\\\"); break;
                case '\a': appendEscapeHelper("\\a" ); break;
                case '\b': appendEscapeHelper("\\b" ); break;
                case '\f': appendEscapeHelper("\\f" ); break;
                case '\n': appendEscapeHelper("\\n" ); break;
                case '\r': appendEscapeHelper("\\r" ); break;
                case '\t': appendEscapeHelper("\\t" ); break;
                case '\v': appendEscapeHelper("\\v" ); break;
                default:
                    if (ch>=' ' || ch<0)
                        res.append(1, ch);
                    else
                        appendEscapeHelper(makeOctalEscStr((unsigned char)ch));
            }
        }

        return res;
    }

    static
    std::string makeMarkerString( const std::string &escapedStr, std::size_t &escapedPos )
    {
        auto markerStr = std::string(escapedStr.size(), ' ');
        if (escapedPos>=markerStr.size())
            markerStr.append(1,'^');
        else
            markerStr[escapedPos] = '^';

        return markerStr;
    }

}; // struct ParserErrorLog

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
using SharedParserErrorLog = std::shared_ptr<ParserErrorLog>;

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct TokenizerErrorHandlerBase
{
    SharedParserErrorLog  errorLog;
    UMBA_RULE_OF_FIVE(TokenizerErrorHandlerBase, delete, default, default, default, default);
    TokenizerErrorHandlerBase(SharedParserErrorLog errLog) : errorLog(errLog) {}
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// tokenizer.unexpectedHandler
struct TokenizerUnexpectedErrorHandler : public TokenizerErrorHandlerBase
{
    UMBA_RULE_OF_FIVE(TokenizerUnexpectedErrorHandler, delete, default, default, default, default);
    explicit TokenizerUnexpectedErrorHandler(SharedParserErrorLog errLog) : TokenizerErrorHandlerBase(errLog) {}

    // Событие unexpected
    // it может быть равен end, поэтому тут дополнительно передаётся itEnd
    // Но надо брать не [it,itEnd), а только it
    // Либо итератор места события, либо ничего: it==itEnd
    // const char* srcFile, int srcLine - источник в исходниках, srcFile может быть нулевым
    template<typename TokenizerType, typename IteratorType>
    bool operator()(TokenizerType &tokenizer, IteratorType it, IteratorType itEnd, const char* srcFile, int srcLine) const
    {
        UMBA_USED(tokenizer);

        std::string erroneousValue;
        std::string erroneousLineText;
        if (it!=itEnd)
        {
            erroneousValue    = umba::iterator::makeString(it,itEnd);
            erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
        }
        else
        {
            if (!it.isEndReached() && !it.isEndIterator())
            {
                erroneousValue = umba::iterator::makeString(it);
                erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
            }
        }

        errorLog->logErrorEvent( ParserErrorEventType::unexpected
                               , it.getPosition(true) // с поиском конца строки
                               , 0
                               , erroneousValue
                               , erroneousLineText
                               , std::string() // customMsgId
                               , std::string() // customMessage
                               , srcFile, srcLine
                               );
        return false;
    }

}; // struct TokenizerUnexpectedErrorHandler

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// tokenizer.reportUnknownOperatorHandler
struct TokenizerUnknownOperatorWarningHandler : public TokenizerErrorHandlerBase
{
    UMBA_RULE_OF_FIVE(TokenizerUnknownOperatorWarningHandler, delete, default, default, default, default);
    TokenizerUnknownOperatorWarningHandler(SharedParserErrorLog errLog) : TokenizerErrorHandlerBase(errLog) {}

    // Событие reportUnknownOperator
    // [b,e) - подстрока с оператором
    template<typename TokenizerType, typename IteratorType>
    void operator()(TokenizerType &tokenizer, IteratorType b, IteratorType e) const
    {
        UMBA_USED(tokenizer);

        std::string erroneousValue    = umba::iterator::makeString(b,e);
        std::string erroneousLineText = umba::iterator::makeString(b.getLineStartIterator(), b.getLineEndIterator());

        errorLog->logErrorEvent( ParserErrorEventType::unknownOperator
                               , b.getPosition(true) // с поиском конца строки
                               , 0
                               , erroneousValue
                               , erroneousLineText
                               , std::string() // customMsgId
                               , std::string() // customMessage
                               , 0, 0 // srcFile, srcLine
                               );
    }

}; // struct TokenizerUnknownOperatorWarningHandler

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// tokenizer.reportStringLiteralMessageHandler
struct TokenizerStringLiteralMessageHandler : public TokenizerErrorHandlerBase
{
    UMBA_RULE_OF_FIVE(TokenizerStringLiteralMessageHandler, delete, default, default, default, default);
    TokenizerStringLiteralMessageHandler(SharedParserErrorLog errLog) : TokenizerErrorHandlerBase(errLog) {}

    template<typename TokenizerType, typename IteratorType>
    void operator()(TokenizerType &tokenizer, bool bErr, IteratorType it, const std::string &msg) const
    {
        UMBA_USED(tokenizer);

        std::string erroneousValue    = umba::iterator::makeString(it);
        std::string erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());

        // bErr - ошибка или варнинг
        // it - положение
        // msg - сообщение
        errorLog->logErrorEvent( bErr ? ParserErrorEventType::stringLiteralError : ParserErrorEventType::stringLiteralWarning
                               , it.getPosition(true) // с поиском конца строки
                               , 0
                               , erroneousValue
                               , erroneousLineText
                               , std::string() // customMsgId
                               , msg // customMessage
                               , 0, 0 // srcFile, srcLine
                               );
    }

}; // struct TokenizerStringLiteralMessageHandler

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// tokenizer.reportHandleTokenErrorHandler
struct TokenizerTokenErrorHandler : public TokenizerErrorHandlerBase
{
    UMBA_RULE_OF_FIVE(TokenizerTokenErrorHandler, delete, default, default, default, default);
    TokenizerTokenErrorHandler(SharedParserErrorLog errLog) : TokenizerErrorHandlerBase(errLog) {}

    // Событие reportUnknownOperator
    // [b,e) - подстрока с оператором
    template<typename TokenizerType, typename IteratorType>
    void  operator()(TokenizerType &tokenizer, umba::tokenizer::payload_type payload, IteratorType b, IteratorType e, const std::string &msg) const
    {
        std::string erroneousValue    = umba::iterator::makeString(b,e);
        std::string erroneousLineText = umba::iterator::makeString(b.getLineStartIterator(), b.getLineEndIterator());

	        errorLog->logErrorEvent( ParserErrorEventType::tokenError
	                               , b.getPosition(true) // с поиском конца строки
	                               , payload
	                               , erroneousValue
	                               , erroneousLineText
	                               , std::string() // customMsgId
	                               , msg  // customMessage
	                               , 0, 0 // srcFile, srcLine
	                               );
    }

}; // struct TokenizerTokenErrorHandler

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace log
} // namespace tokenizer
} // namespace umba



