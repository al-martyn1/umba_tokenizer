/*! \file
    \brief Подсистема протоколирования для токенизеров-парсеров
 */

#pragma once

#include "umba/umba.h"
//
#include "umba/rule_of_five.h"
#include "umba/utility.h"
#include "umba/text_position_info.h"
#include "umba/format_message.h"
#include "umba/escape_string.h"
//
#include "protogen-types.h"

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
enum class ParserErrorEventType
{
    customEvent          ,
    unexpected           ,
    unknownOperator      , // warning/info report
    stringLiteralWarning , // warning/info report
    stringLiteralError   ,
    tokenError
};

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
struct ParserErrorLog
{

    ~ParserErrorLog() {}

    virtual
    void logErrorEvent( ParserErrorEventType          eventType
                      , const TextPositionInfo        &textPos
                      , umba::tokenizer::payload_type payload
                      , std::string                   erroneousValue     // Can be empty, can contain non-printable chars 
                      , std::string                   erroneousLineText  // Can be empty, can contain non-printable chars 
                      , const std::string             &customMsgId       // Can be empty
                      , const std::string             &customMessage     // Can be empty
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
    TokenizerUnexpectedErrorHandler(SharedParserErrorLog errLog) : TokenizerErrorHandlerBase(errLog) {}

    // Событие unexpected
    // it может быть равен end, поэтому тут дополнительно передаётся itEnd
    // Но надо брать не [it,itEnd), а только it
    // Либо итератор места события, либо ничего: it==itEnd
    // const char* srcFile, int srcLine - источник в исходниках, srcFile может быть нулевым
    template<typename TokenizerType, typename IteratorType>
    bool operator()(TokenizerType &tokenizer, IteratorType it, IteratorType itEnd, const char* srcFile, int srcLine) const
    {
        std::string erroneousValue;
        std::string erroneousLineText;
        if (it!=itEnd)
        {
            erroneousValue    = umba::iterator::makeString(it);
            erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
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
#define UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE   srcFile ? srcFile : __FILE__
#define UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE   srcFile ? srcLine : __LINE__

#define UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT                 umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::err ,  /* gopts, */  std::string()/*"err"*/, curFile.c_str(), lineNo, UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE )
#define UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX(errType)     umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::err ,  /* gopts, */  std::string(errType)  , curFile.c_str(), lineNo, UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE )
#define UMBA_TOKENIZER_LOG_CONLOG_WARN_INPUT(warnType)      umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::warn,  /* gopts, */  std::string(warnType) , curFile.c_str(), lineNo, UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE )
#define UMBA_TOKENIZER_LOG_CONLOG_INFO_INPUT(infoType)      umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::msg ,  /* gopts, */  std::string(infoType) , curFile.c_str(), lineNo, UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE )

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename FilenameSetType>
struct ParserConsoleErrorLog : public ParserErrorLog
{

    // TextPositionInfo
    // umba_text_position_info_line_offset_type    lineOffset  ; //!< From data origin to line start
    // umba_text_position_info_symbol_offset_type  symbolOffset; //!< From line start
    // umba_text_position_info_symbol_offset_type  lineLen     ; //!< From start to first line end symbol or to end of text
    // umba_text_position_info_line_number_type    lineNumber  ; //!< Zero based line number
    // umba_text_position_info_file_id_type        fileId      ; //!< FileID

    using SharedFilenameSetType = std::shared_ptr<FilenameSetType>;
    using FileInfoType          = typename FilenameSetType::FileInfoType;

    SharedFilenameSetType pFilenameSet;

    ParserConsoleErrorLog(SharedFilenameSetType fnSet) : ParserErrorLog(), filenameSet(fnSet) {}


    void logErrorEvent( ParserErrorEventType          eventType
                      , const TextPositionInfo        &textPos
                      , umba::tokenizer::payload_type payload
                      , std::string                   erroneousValue     // Can be empty, can contain non-printable chars 
                      , std::string                   erroneousLineText  // Can be empty, can contain non-printable chars 
                      , const std::string             &customMsgId       // Can be empty
                      , const std::string             &customMessage     // Can be empty
                      , const char* srcFile,          int srcLine
                      ) const
                      override
    {
        erroneousValue = umba::escapeStringC(erroneousValue); // Тут не обязательно подсчитывать, сколько лишних символов добавилось

        int lineNo = (int)textPos.lineNumber;

        std::string curFile = "<->";

        auto pFileInfo = pFilenameSet->getFileInfo(textPos.fileId);
        UMBA_ASSERT(pFileInfo!=0);
        curFile = pFileInfo->orgFilename;

        auto errMarkerStr = std::string(erroneousLineText.size(), ' ');
        if (textPos.symbolOffset>=errMarkerStr.size())
            errMarkerStr.append(1,'^');
        else
            errMarkerStr[textPos.symbolOffset] = '^';
        

        if (!srcFile)
        {
            srcFile
        }
        
                  int srcLine

        switch(eventType)
        {
            case ParserErrorEventType::unexpected:
            {
                if (erroneousValue.empty())
                {
                    UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX("unxp-symbol") << umba::FormatMessage("Unexpected symbol").toString() <<"\n";
                }
                else
                {
                    UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX("unxp-symbol") << umba::FormatMessage("Unexpected symbol: \'$(Symbol)\'").arg("Symbol", erroneousValue).toString() <<"\n";
                }


                break;
            }

            case ParserErrorEventType::unknownOperator:
            {
                //std::cout << "Possible unknown operator: '" << umba::iterator::makeString(b, e) << "'\n";
                break;
            }

            case ParserErrorEventType::stringLiteralWarning:
            {
                break;
            }

            case ParserErrorEventType::stringLiteralError:
            {
                break;
            }

            case ParserErrorEventType::tokenError:
            {
                break;
            }

            case ParserErrorEventType::customEvent:
            {
                break;
            }

        }

        if (!erroneousLineText.empty() && !errMarkerStr.empty())
        {
            UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT << "Line: " << erroneousLineText << "\n";
            UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT << "     |" << errMarkerStr      << "|\n";
        }

    }

// template<typename StreamType, typename MsgType>
// void printError(StreamType &ss, const std::string &inputFilename, umba::tokenizer::payload_type tokenType, umba::iterator::TextPositionCountingIterator<char> it, umba::iterator::TextPositionCountingIterator<char> itEnd, MsgType msg)
// {
//      UMBA_USED(tokenType);
//  
//      ss << "!!! ";
//  
//      if (it==itEnd)
//      {
//          ss << inputFilename << ": Unexpected end of file\n";
//          ss << "TokenType: " << tokenType << "\n";
//          return;
//      }
//  
//      if (msg.empty())
//      {
//          msg = "Unexpected symbol";
//      }
//  
//      auto errPos = it.getPosition(true); // с поиском конца строки (а вообще не надо пока, но пусть)
//      std::string erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
//      ss << inputFilename << ":" << errPos.toString<std::string>() << ": " << msg << "\n";
//      ss << "Line: " << erroneousLineText << "\n";
//      auto errMarkerStr = std::string(erroneousLineText.size(), ' ');
//      if (errPos.symbolOffset>=errMarkerStr.size())
//          errMarkerStr.append(1,'^');
//      else
//          errMarkerStr[errPos.symbolOffset] = '^';
//      ss << "     |" << errMarkerStr << "|\n";
//  
//      if (it!=itEnd)
//      {
//          // char ch = *it;
//          // ss << "ch: " << umba::escapeStringC(std::string(1,ch)) << "\n";
//      }


}; // struct ParserConsoleErrorLog


// #define UMBA_LOG_ERR_INPUT                 umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::err ,  /* gopts, */  std::string()/*"err"*/, curFile.c_str(), lineNo, __FILE__, __LINE__ )
// #define UMBA_LOG_ERR_INPUT_EX(errType)     umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::err ,  /* gopts, */  std::string(errType)  , curFile.c_str(), lineNo, __FILE__, __LINE__ )
// #define UMBA_LOG_WARN_INPUT(warnType)      umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::warn,  /* gopts, */  std::string(warnType) , curFile.c_str(), lineNo, __FILE__, __LINE__ )
// #define UMBA_LOG_INFO_INPUT(infoType)      umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::msg ,  /* gopts, */  std::string(infoType) , curFile.c_str(), lineNo, __FILE__, __LINE__ )
//  
// // options and other errors
// #define UMBA_LOG_ERR                       umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::err ,  /* gopts,  */ std::string()/*"err"*/, (const char*)0 , 0     , __FILE__, __LINE__ )
// #define UMBA_LOG_WARN(warnType)            umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::warn,  /* gopts,  */ std::string(warnType) , (const char*)0 , 0     , __FILE__, __LINE__ )
// #define UMBA_LOG_INFO(infoType)            umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::msg ,  /* gopts,  */ std::string(infoType) , (const char*)0 , 0     , __FILE__, __LINE__ )
//                                                                             /*         */
// #define UMBA_LOG_MSG                       umba::log::startLogError( umbaLogStreamMsg, umba::log::LogEntryType::msg ,  /* gopts,  */ std::string()/*"msg"*/, (const char*)0 , 0     , (const char*)0, 0 )

// enum class ParserErrorEventType
// {
//     customEvent
//     unexpected
//     unknownOperator // warning
//     stringLiteralWarning
//     stringLiteralError
//     tokenError
// };

// #define LOG_ERR_INPUT                UMBA_LOG_ERR_INPUT
// #define LOG_WARN_INPUT(warnType)     UMBA_LOG_WARN_INPUT(warnType)
// #define LOG_INFO_INPUT(infoType)     UMBA_LOG_INFO_INPUT(infoType)
//  
// // options and other errors
// #define LOG_ERR                      UMBA_LOG_ERR
// #define LOG_WARN(warnType)           UMBA_LOG_WARN(warnType)
// #define LOG_INFO(infoType)           UMBA_LOG_INFO(infoType)
//  
// #define LOG_MSG                      UMBA_LOG_MSG


//----------------------------------------------------------------------------

} // namespace log
} // namespace tokenizer
} // namespace umba



