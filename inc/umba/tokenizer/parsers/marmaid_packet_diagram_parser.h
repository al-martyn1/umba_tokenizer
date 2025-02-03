/*! \file
    \brief Парсер marmaid packet diagram, расширенная версия, с использованием типов и массивов
 */

// TODO: Переделать логи. Сейчас нет вывода файла/строки, и используется макрос LOG_ERR, из-за чего данный файл надо подключать после его определения.


#pragma once


#include "umba/tokenizer.h"
//
#include "umba/tokenizer/tokenizer_log.h"
#include "umba/tokenizer/tokenizer_log_console.h"
#include "umba/tokenizer/token_collection.h"
#include "umba/tokenizer/parser_base.h"
#include "umba/tokenizer/lang/marmaid_packet_diagram.h"

//
#include <stdexcept>


// umba::tokenizer::
namespace umba {
namespace tokenizer {


enum class MarmaidPacketDiagramRangeType
{
    singleValue,
    range      ,
    explicitType
};


template<typename TokenCollectionItemType>
struct MarmaidPacketDiagramItem
{
    MarmaidPacketDiagramRangeType   rangeType;
    umba::tokenizer::payload_type   explicitTypeTokenId;
    std::string                     text;

    std::uint64_t                   start;
    std::uint64_t                   end  ; // входит в диапазон
    std::uint64_t                   arraySize = std::uint64_t(-1);

    const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки
};







template<typename TokenizerType>
class MarmaidPacketDiagramParser : public umba::tokenizer::ParserBase<TokenizerType>
{

public:

    using BaseClass = umba::tokenizer::ParserBase<TokenizerType>;
    using tokenizer_type           = TokenizerType;
    using TokenCollectionType      = typename BaseClass::TokenCollectionType     ;
    using TokenCollectionItemType  = typename BaseClass::TokenCollectionItemType ;
    using TokenInfoType            = typename BaseClass::TokenInfoType           ;
    using TokenPosType             = typename BaseClass::TokenPosType            ;
    using token_parsed_data_type   = typename tokenizer_type::token_parsed_data_type;
    using shared_log_type          = umba::tokenizer::log::SharedParserErrorLog;

    using MarmaidPacketDiagramItemType = MarmaidPacketDiagramItem<TokenCollectionItemType>;



    std::vector<MarmaidPacketDiagramItemType>  diagramData;
    shared_log_type                            log;



    UMBA_RULE_OF_FIVE_COPY_DELETE(MarmaidPacketDiagramParser);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(MarmaidPacketDiagramParser);

    MarmaidPacketDiagramParser() {}

    explicit
    MarmaidPacketDiagramParser(std::shared_ptr<TokenCollectionType> tc, shared_log_type a_log)
    : BaseClass(tc)
    , log(a_log)
    {
    }

    static bool isAnyType(umba::tokenizer::payload_type tk)
    {
        switch(tk)
        {
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return true;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return true;
            default: return false;
        }
    }

    static bool isAnyNumber(umba::tokenizer::payload_type tk)
    {
        switch(tk)
        {
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN: return true;
            //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN: return true;
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC: return true;
            //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC: return true;
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT: return true;
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX: return true;
            //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX: return true;
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT: return true;
            default: return false;
        }
    }


    static
    std::string getTokenIdStr(umba::tokenizer::payload_type tk)
    {
        switch(tk)
        {
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return "char"  ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return "int8"  ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return "int16" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return "int32" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return "int64" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return "uint8" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return "uint16";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return "uint32";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return "uint64";
            case UMBA_TOKENIZER_TOKEN_IDENTIFIER         : return "identifier";
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN: return "number";
            //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN: return "number";
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC: return "number";
            //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC: return "number";
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT: return "number";
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX: return "number";
            //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX: return "number";
            case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT: return "number";
            case UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST: return "comment";
            case UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION: return "range operator";
            case UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE: return "text delimiter";
            case UMBA_TOKENIZER_TOKEN_STRING_LITERAL     : return "string";
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN : return "array size start bracket";
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE: return "array size end bracket";
            case UMBA_TOKENIZER_TOKEN_LINEFEED : return "linefeed";
            case UMBA_TOKENIZER_TOKEN_SPACE    : return "space";
            case UMBA_TOKENIZER_TOKEN_TAB      : return "tab";
            case UMBA_TOKENIZER_TOKEN_FORM_FEED: return "form feed";
            
            case UMBA_TOKENIZER_TOKEN_CTRL_FIN: return "EOF";
            // case : return "";
            // case : return "";
            // case : return "";
            // case : return "";
            default: return "unknown_" + std::to_string(tk);
        }
    }

    bool checkNotNul(const TokenInfoType *pTokenInfo) const
    {
        if (!pTokenInfo)
        {
            // LOG_ERR << "Something goes wrong\n";
            // log->logErrorEvent( ParserErrorEventType::customError
            //                   , textPositionInfoInitialized(getFileId())
            //                   , 0
            //                   )
            // return false;

            throw std::runtime_error("Something goes wrong");
        }

        return true;
    }

    // void logErrorEvent( ParserErrorEventType          eventType
    //                   , const TextPositionInfo        &textPos
    //                   , umba::tokenizer::payload_type payload
    //                   , std::string                   erroneousValue     // Can be empty, can contain non-printable chars 
    //                   , std::string                   erroneousLineText  // Can be empty, can contain non-printable chars 
    //                   , const std::string             &customMsgId       // Can be empty, used only for customError and customWarning, can be overriden by 'msg-id' value
    //                   , const std::string             &customMessage     // Can be empty, used only for tokenError, customError and customWarning
    //                   , const char* srcFile,          int srcLine
    //                   ) const
    //                   = 0;


            // case ParserErrorEventType::customError:
            // {
            //     std::string msgId = customMsgId; //  = "error";
            //     auto msgIdIt = formatArgs.find("msg-id");
            //     if (msgIdIt!=formatArgs.end())
            //         msgId = msgIdIt->second;
            //     UMBA_TOKENIZER_LOG_CONLOG_ERR_INPUT_EX(msgId)
            //         << FormatMessage(customMessage).values(formatArgs).arg("Value", erroneousValue)
            //            .toString() <<"\n";
            //     break;
            // }



    bool expectedReachedMsg(umba::tokenizer::payload_type tkExpected, umba::tokenizer::payload_type tkReached, const std::string &msg) const
    {
        LOG_ERR << msg << ". Expected " << getTokenIdStr(tkExpected) << ", but got " << getTokenIdStr(tkReached) << "\n";
        return false;
    }

    bool checkExactTokenType(umba::tokenizer::payload_type tkExpected, umba::tokenizer::payload_type tkReached, const std::string &msg) const
    {
        if (tkExpected==tkReached)
            return true;
        return expectedReachedMsg(tkExpected, tkReached, msg);
    }


    bool parse()
    {
        using umba::tokenizer::ParserWaitForTokenFlags;
        TokenPosType tokenPos;

        while(true)
        {
            const TokenInfoType *pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            if (!checkNotNul(pTokenInfo))
                return false;

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                return true; // normal stop

            MarmaidPacketDiagramItemType item;
            item.pTokenInfo = pTokenInfo;

            if (isAnyNumber(pTokenInfo->tokenType))
            {
                const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
                auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
                if (numericLiteralData.pData->fOverflow)
                {
                    LOG_ERR << "integer oveflow\n";
                    return false;
                }

                item.rangeType = MarmaidPacketDiagramRangeType::singleValue;
                item.start     = std::uint64_t(numericLiteralData.pData->value);

                pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                if (!checkNotNul(pTokenInfo))
                    return false;

                if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION)
                {
                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                    if (!checkNotNul(pTokenInfo))
                        return false;
                    if (!isAnyNumber(pTokenInfo->tokenType))
                        return expectedReachedMsg(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, pTokenInfo->tokenType, "invalid range");

                    pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
                    auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
                    if (numericLiteralData.pData->fOverflow)
                    {
                        LOG_ERR << "integer oveflow\n";
                        return false;
                    }
        
                    item.rangeType = MarmaidPacketDiagramRangeType::range;
                    item.end       = std::uint64_t(numericLiteralData.pData->value);

                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                    
                }
            }

            else if (isAnyType(pTokenInfo->tokenType))
            {
                item.rangeType           = MarmaidPacketDiagramRangeType::explicitType;
                item.explicitTypeTokenId = pTokenInfo->tokenType;

                pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                if (!checkNotNul(pTokenInfo))
                    return false;
                if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN)
                {
                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                    if (!checkNotNul(pTokenInfo))
                        return false;
                    if (!isAnyNumber(pTokenInfo->tokenType))
                        return expectedReachedMsg(UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, pTokenInfo->tokenType, "invalid array size");

                    const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
                    auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
                    if (numericLiteralData.pData->fOverflow)
                    {
                        LOG_ERR << "integer oveflow\n";
                        return false;
                    }
        
                    item.arraySize = std::uint64_t(numericLiteralData.pData->value);

                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                    if (!checkNotNul(pTokenInfo))
                        return false;

                    if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE)
                        return expectedReachedMsg(UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE, pTokenInfo->tokenType, "invalid array size");
                
                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);

                }
            }

            else if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
            {
                continue;
            }

            else
            {
                LOG_ERR << "unexpected " << getTokenIdStr(pTokenInfo->tokenType) << "\n";
                return false;
            }

            // Тут у нас в pTokenInfo должен быть следующий токен
            if (!checkNotNul(pTokenInfo))
                return false;

            if (!checkExactTokenType(UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE, pTokenInfo->tokenType, "invalid record definition"))
                return false;

            pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            if (!checkNotNul(pTokenInfo))
                return false;

            if (!checkExactTokenType(UMBA_TOKENIZER_TOKEN_STRING_LITERAL, pTokenInfo->tokenType, "invalid record definition"))
                return false;

            const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
            auto stringLiteralData = std::get<typename tokenizer_type::StringLiteralDataHolder>(*pParsedData);
            item.text = stringLiteralData.pData->asString();

            pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            if (!checkNotNul(pTokenInfo))
                return false;

            if (!checkExactTokenType(UMBA_TOKENIZER_TOKEN_LINEFEED, pTokenInfo->tokenType, "invalid record definition"))
                return false;

            diagramData.emplace_back(item);
        }

        return false;
    }


}; // class MarmaidPacketDiagramParser



} // namespace tokenizer
} // namespace umba

