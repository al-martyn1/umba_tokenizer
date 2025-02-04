/*! \file
    \brief Подсистема протоколирования для токенизеров-парсеров
 */

#pragma once

#include "tokenizer_log.h"
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// umba::tokenizer::log::
namespace umba {
namespace tokenizer {
namespace log
{

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE   srcFile ? srcFile : __FILE__
#define UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE   srcFile ? srcLine : __LINE__

#define UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT                 umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::err ,  /* gopts, */  std::string()/*"err"*/, curFile.c_str(), std::size_t(lineNo), UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, int(UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE) )
#define UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX(errType)     umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::err ,  /* gopts, */  std::string(errType)  , curFile.c_str(), std::size_t(lineNo), UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, int(UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE) )
#define UMBA_TOKENIZER_LOG_CONLOG_WARN_INPUT(warnType)      umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::warn,  /* gopts, */  std::string(warnType) , curFile.c_str(), std::size_t(lineNo), UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, int(UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE) )
#define UMBA_TOKENIZER_LOG_CONLOG_INFO_INPUT(infoType)      umba::log::startLogError( umbaLogStreamErr, umba::log::LogEntryType::msg ,  /* gopts, */  std::string(infoType) , curFile.c_str(), std::size_t(lineNo), UMBA_TOKENIZER_LOG_CONLOG_SRC_FILE, int(UMBA_TOKENIZER_LOG_CONLOG_SRC_LINE) )

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename FilenameSetType_>
struct ParserConsoleErrorLog : public ParserErrorLog
{

    // TextPositionInfo
    // umba_text_position_info_line_offset_type    lineOffset  ; //!< From data origin to line start
    // umba_text_position_info_symbol_offset_type  symbolOffset; //!< From line start
    // umba_text_position_info_symbol_offset_type  lineLen     ; //!< From start to first line end symbol or to end of text
    // umba_text_position_info_line_number_type    lineNumber  ; //!< Zero based line number
    // umba_text_position_info_file_id_type        fileId      ; //!< FileID

    using FilenameSetType       = FilenameSetType_;
    using SharedFilenameSetType = std::shared_ptr<FilenameSetType>;
    using FileInfoType          = typename FilenameSetType::FileInfoType;

    SharedFilenameSetType pFilenameSet;

    ParserConsoleErrorLog(SharedFilenameSetType pFnSet) : ParserErrorLog(), pFilenameSet(pFnSet) {}


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
        UMBA_USED(payload);

        erroneousValue = umba::escapeStringC(erroneousValue); // Тут не обязательно подсчитывать, сколько лишних символов добавилось

        int lineNo = (int)textPos.lineNumber + 1;

        std::string curFile = "<->";

        auto pFileInfo = pFilenameSet->getFileInfo(textPos.fileId);
        UMBA_ASSERT(pFileInfo!=0);
        curFile = pFileInfo->orgFilename;

        using FormatMessage = umba::FormatMessage<std::string>;

        bool bError = true;


        switch(eventType)
        {
            case ParserErrorEventType::unexpected:
            {
                if (erroneousValue.empty())
                {
                    UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX("unexpected-symbol") << FormatMessage("unexpected symbol").toString() <<"\n";
                }
                else
                {
                    UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX("unexpected-symbol") << FormatMessage("unexpected symbol: \'$(Value)\'").arg("Value", erroneousValue).toString() <<"\n";
                }


                break;
            }

            case ParserErrorEventType::unknownOperator:
            {
                UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX("unknown-operator") << FormatMessage("unknown operator: '$(Value)'").arg("Value", erroneousValue).toString() <<"\n";
                //std::cout << "Possible unknown operator: '" << umba::iterator::makeString(b, e) << "'\n";
                break;
            }

            case ParserErrorEventType::stringLiteralWarning:
            {
                bError = false;
                UMBA_TOKENIZER_LOG_CONLOG_WARN_INPUT("string-literal") 
                    << FormatMessage("string literal, suspicious character value: '$(Value)': $(Message)")
                       .arg("Value", erroneousValue)
                       .arg("Message", customMessage) // customMsgId - for translation
                       .toString() <<"\n";
                break;
            }

            case ParserErrorEventType::stringLiteralError:
            {
                UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX("string-literal") 
                    << FormatMessage("string literal, invalid character value: '$(Value)': $(Message)")
                       .arg("Value", erroneousValue)
                       .arg("Message", customMessage) // customMsgId - for translation
                       .toString() <<"\n";
                break;
            }

            case ParserErrorEventType::tokenError:
            {
                // Это сообщение обычно выводится, когда парсер возвращает false.
                // Парсер должен установить formatArgs, и заполнить текст сообщения
                std::string msgId = "token-error";
                auto msgIdIt = formatArgs.find("msg-id");
                if (msgIdIt!=formatArgs.end())
                    msgId = msgIdIt->second;
                UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX(msgId)
                    << FormatMessage(customMessage).values(formatArgs).arg("Value", erroneousValue)
                       .toString() <<"\n";
                break;
            }

            case ParserErrorEventType::customError:
            {
                std::string msgId = customMsgId; //  = "error";
                auto msgIdIt = formatArgs.find("msg-id");
                if (msgIdIt!=formatArgs.end())
                    msgId = msgIdIt->second;
                UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX(msgId)
                    << FormatMessage(customMessage).values(formatArgs).arg("Value", erroneousValue)
                       .toString() <<"\n";
                break;
            }

            case ParserErrorEventType::customWarning:
            {
                bError = false;
                std::string msgId = customMsgId; // = "error";
                auto msgIdIt = formatArgs.find("msg-id");
                if (msgIdIt!=formatArgs.end())
                    msgId = msgIdIt->second;
                UMBA_TOKENIZER_LOG_CONLOG_WARN_INPUT(msgId)
                    << FormatMessage(customMessage).values(formatArgs).arg("Value", erroneousValue)
                       .toString() <<"\n";
                break;
            }

            default: erroneousLineText.clear(); // Доп инфу не надо выводить, так как нет основного сообщения
                     
        }

        if (!erroneousLineText.empty())
        {
            std::size_t errPos = textPos.symbolOffset;
            std::string escapedStr = umba::tokenizer::log::ParserErrorLog::escapeStringAndCorrectPos( erroneousLineText, errPos );
            std::string markerStr  = umba::tokenizer::log::ParserErrorLog::makeMarkerString(escapedStr, errPos );

            if (bError)
            {
                UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT << "Line :" << escapedStr << "\n";
                UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT << "     |" << markerStr  << "|\n";
            }
            else
            {
                UMBA_TOKENIZER_LOG_CONLOG_WARN_INPUT(std::string()) << "Line :" << escapedStr << "\n";
                UMBA_TOKENIZER_LOG_CONLOG_WARN_INPUT(std::string()) << "     |" << markerStr  << "|\n";
            }
        }

    }


}; // struct ParserConsoleErrorLog

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace log
} // namespace tokenizer
} // namespace umba



