/*! \file
    \brief Парсер mermaid packet diagram, расширенная версия, с использованием типов и массивов
 */


#pragma once

#include "umba/string_plus.h"
//

#include "umba/tokenizer.h"
//
#include "umba/tokenizer/tokenizer_log.h"
#include "umba/tokenizer/tokenizer_log_console.h"
#include "umba/tokenizer/token_collection.h"
//
#include "utils.h"
#include "parser_base.h"
#include "usketch_parser_types.h"

//
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
#include "umba/the.h"


//
#include <stdexcept>
#include <initializer_list>


//----------------------------------------------------------------------------
// umba::tokenizer::usketch::
namespace umba {
namespace tokenizer {
namespace usketch {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType>
class USketchParser : public umba::tokenizer::ParserBase<TokenizerType>
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
    using string_type              = typename BaseClass::string_type;

    // using PacketDiagramItemType    = PacketDiagramItem<TokenCollectionItemType>;
    // using PacketDiagramType        = PacketDiagram<TokenCollectionItemType>;


    shared_log_type                log    ;


protected:

    // PacketDiagramType              diagram;
    // std::size_t                    orgCounter = 0;


public:

    UMBA_RULE_OF_FIVE_COPY_DELETE(USketchParser);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(USketchParser);

    USketchParser() {}

    explicit
    USketchParser(std::shared_ptr<TokenCollectionType> tc, shared_log_type a_log)
    : BaseClass(tc)
    , log(a_log)
    {
    }

    void setTitle(const std::string &t)
    {
         UMBA_USED(t);
         //diagram.title = t;
    }

    // PacketDiagramType getDiagram() const
    // {
    //     auto diagramCopy = diagram;
    //     diagramCopy.removeEmptyOrgs();
    //     return diagramCopy;
    // }


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
            /*
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return "type"; // return "char"  ; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return "type"; // return "int8"  ; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return "type"; // return "int16" ; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return "type"; // return "int32" ; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return "type"; // return "int64" ; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return "type"; // return "uint8" ; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return "type"; // return "uint16"; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return "type"; // return "uint32"; 
            case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return "type"; // return "uint64"; 
            */

            
            case UMBA_TOKENIZER_TOKEN_RAW_DATA           : return "raw-data";
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
            case UMBA_TOKENIZER_TOKEN_STRING_LITERAL     : return "string";
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN : return "array-size-start-bracket";
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE: return "array-size-end-bracket";
            case UMBA_TOKENIZER_TOKEN_LINEFEED : return "linefeed";
            case UMBA_TOKENIZER_TOKEN_SPACE    : return "space";
            case UMBA_TOKENIZER_TOKEN_TAB      : return "tab";
            case UMBA_TOKENIZER_TOKEN_FORM_FEED: return "form-feed";
            //case MERMAID_TOKEN_OPERATOR_EXTRA  : return "extra-options-operator";
            
            case UMBA_TOKENIZER_TOKEN_CTRL_FIN: return "EOF";

            // case MERMAID_TOKEN_OPERATOR_RANGE: return "range-operator";
            // case MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER: return "follow-delimiter";

            // case : return "";
            // case : return "";
            // case : return "";
            // case : return "";

            default: 
                 #if 0
                 if (tk>=MERMAID_TOKEN_SET_DIRECTIVES_FIRST && tk<=MERMAID_TOKEN_SET_DIRECTIVES_LAST)
                     return "directive";

                 if (tk>=MERMAID_TOKEN_SET_TYPES_FIRST && tk<=MERMAID_TOKEN_SET_TYPES_LAST)
                     return "type";

                 if (tk>=MERMAID_TOKEN_SET_OPERATORS_FIRST && tk<=MERMAID_TOKEN_SET_OPERATORS_LAST )
                     return "operator";

                 if (tk>=MERMAID_TOKEN_SET_ATTRS_FIRST && tk<=MERMAID_TOKEN_SET_ATTRS_LAST )
                     return "attribute/option";
                 #endif

                 return "unknown_" + std::to_string(tk);
        }
    }

    bool checkNotNul(const TokenInfoType *pTokenInfo) const
    {
        if (!pTokenInfo)
        {
            // log->logErrorEvent( ParserErrorEventType::customError
            //                   , textPositionInfoInitialized(getFileId())
            //                   , 0
            //                   )
            // return false;

            //throw std::runtime_error("Something goes wrong in parser");
            // Не надо ничего кидать, просто молча выходим
            return false;
        }

        return true;
    }

    bool expectedReachedMsg(const TokenInfoType *pTokenInfo, std::initializer_list<umba::tokenizer::payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        BaseClass::logUnexpected( pTokenInfo, payloadExpectedList, msg, [&](umba::tokenizer::payload_type tk) { return getTokenIdStr(tk); } );
        return false;
    }

    bool checkExactTokenType(const TokenInfoType *pTokenInfo, std::initializer_list<umba::tokenizer::payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        for(auto e : payloadExpectedList)
        {
            if (e==pTokenInfo->tokenType) // tkReached
                return true;
        }
        return expectedReachedMsg(pTokenInfo, payloadExpectedList, msg);
    }

    //! Разбираем токен числового литерала. Возвращается следующий токен для анализа и продолжения разбора строки или ноль при ошибке
    const TokenInfoType* parseNumber(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, std::uint64_t &val, const std::string &msg=std::string())
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        // if (!checkNotNul(pTokenInfo))
        //     return 0;

        using umba::tokenizer::ParserWaitForTokenFlags;

        if (!isAnyNumber(pTokenInfo->tokenType))
            return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, msg), (const TokenInfoType*)0;

        const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
        auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
        if (numericLiteralData.pData->fOverflow)
            return BaseClass::logSimpleMessage(pTokenInfo, "integer-overflow", msg.empty() ? "integer overflow" : msg + ": integer overflow"), (const TokenInfoType*)0;

        val = std::uint64_t(numericLiteralData.pData->value);

        return BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    }


    //! Разбор директивы title. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    const TokenInfoType* parseTitleDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        //auto pTextStart = BaseClass::getTextPointer(pTokenInfo);

        BaseClass::setRawModeAutoStop(umba::tokenizer::TokenizerRawAutoStopMode::stopOnCharExcluding, string_type("\r\n"));
        BaseClass::setRawMode(true);

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку
        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_RAW_DATA}, "invalid 'title' directive"))
            return 0;


        //allowOverrideType

        // if ((diagram.parsingOptions&PacketDiagramParsingOptions::allowOverrideTitle)!=0u)
        // {
        //     const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
        //     auto rawData = std::get<typename tokenizer_type::RawDataHolder>(*pParsedData);
        //     diagram.title = umba::string::trim_copy(rawData.pData->asString());
        // }

        return BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    }



    bool parse()
    {
        TokenPosType tokenPos;

        while(true)
        {
            const TokenInfoType *pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            if (!checkNotNul(pTokenInfo))
                return false;

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
                continue;

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                return true; // normal stop


            #if 0
            if (isAnyNumber(pTokenInfo->tokenType) || isAnyType(pTokenInfo->tokenType))
            {
                PacketDiagramItemType item;
                ChecksumOptions checksumOptions;

                item.pTokenInfo = pTokenInfo; // На всякий случай инфу сохраняем
                
                try
                {
                    pTokenInfo = parseRegularLine(tokenPos, pTokenInfo, item, checksumOptions);
                }
                catch(const marty::mem::address_wrap &)
                {
                    return BaseClass::logMessage( pTokenInfo, "address-wrap", "address wrapping detected. Check your 'org' directives" ), false;
                }
                
                if (!pTokenInfo)
                    return false;

                if (checksumOptions.kind!=ChecksumKind::undefined)
                {
                    item.checksumIndex = diagram.checksumList.size();
                    diagram.checksumList.emplace_back(checksumOptions);
                }

                if (!addDiagramItem(item))
                    return false;
            }

            else if (pTokenInfo->tokenType==MERMAID_TOKEN_DIRECTIVE_PACKET_BETA)
            {
                if (!diagram.isDataEmpty())
                    return BaseClass::logMessage( pTokenInfo, "unexpected-packet-beta", "'packet-beta' directive must follow before the data records definition" ), false;
                pTokenInfo = parsePacketBetaDirective(tokenPos, pTokenInfo);
            }

            else if (pTokenInfo->tokenType==MERMAID_TOKEN_DIRECTIVE_TITLE)
            {
                if (!diagram.isDataEmpty())
                    return BaseClass::logMessage( pTokenInfo, "unexpected-title", "'title' directive must follow before the data records definition" ), false;
                pTokenInfo = parseTitleDirective(tokenPos, pTokenInfo);
            }

            else if (pTokenInfo->tokenType==MERMAID_TOKEN_OPERATOR_EXTRA)
            {
                pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                if (!pTokenInfo)
                    return 0; // Сообщение уже выведено, просто возвращаем ошибку

                if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
                    continue;
    
                if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                    return true; // normal stop
    
                if (!umba::TheValue(pTokenInfo->tokenType).oneOf( MERMAID_TOKEN_DIRECTIVE_NATIVE
                                                                , MERMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH
                                                                , MERMAID_TOKEN_DIRECTIVE_DISPLAY_OPTIONS
                                                                , MERMAID_TOKEN_DIRECTIVE_ORG
                                                                )
                   )
                {
                     expectedReachedMsg(pTokenInfo, {MERMAID_TOKEN_DIRECTIVE_NATIVE } /* , msg */ );
                     return 0;
                }
                // if (!checkExactTokenType(pTokenInfo, {MERMAID_TOKEN_DIRECTIVE_NATIVE, MERMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH} /* , "invalid 'packet-beta' directive options" */ ))
                //     return 0;

                if (pTokenInfo->tokenType==MERMAID_TOKEN_DIRECTIVE_NATIVE)
                {
                    if (!diagram.isDataEmpty())
                        return BaseClass::logMessage( pTokenInfo, "unexpected-native", "'native' directive must follow before the data records definition" ), false;
                    pTokenInfo = parseNativeDirective(tokenPos, pTokenInfo);
                }
    
                else if (pTokenInfo->tokenType==MERMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH)
                {
                    if (!diagram.isDataEmpty())
                        return BaseClass::logMessage( pTokenInfo, "unexpected-display-width", "'display-width' directive must follow before the data records definition" ), false;
                    pTokenInfo = parseDisplayWidthDirective(tokenPos, pTokenInfo);
                }

                else if (pTokenInfo->tokenType==MERMAID_TOKEN_DIRECTIVE_DISPLAY_OPTIONS)
                {
                    if (!diagram.isDataEmpty())
                        return BaseClass::logMessage( pTokenInfo, "unexpected-display-options", "'display-options' directive must follow before the data records definition" ), false;
                    pTokenInfo = parseDisplayOptionsDirective(tokenPos, pTokenInfo);
                }

                else if (pTokenInfo->tokenType==MERMAID_TOKEN_DIRECTIVE_ORG)
                {
                    PacketDiagramItemType item;
                    item.pTokenInfo = pTokenInfo; // На всякий случай инфу сохраняем
                    // pTokenInfo = parseOrgDirective(tokenPos, pTokenInfo, item);
                    try
                    {
                        pTokenInfo = parseOrgDirective(tokenPos, pTokenInfo, item);
                    }
                    catch(const marty::mem::address_wrap &)
                    {
                        return BaseClass::logMessage( pTokenInfo, "address-wrap", "address wrapping detected. Check your 'org' directives" ), false;
                    }

                    if (!pTokenInfo)
                        return false;
                    if (!addDiagramItem(item))
                        return false;
                }

                // const TokenInfoType* parseOrgDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, PacketDiagramItemType &item) 
            }

            else
            {
                expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, MERMAID_TOKEN_SET_TYPES_FIRST, MERMAID_TOKEN_DIRECTIVE_PACKET_BETA, MERMAID_TOKEN_OPERATOR_EXTRA /* , MERMAID_TOKEN_DIRECTIVE_TITLE */ } /* , msg */ );
                return false;
            }
            #endif

            if (!pTokenInfo)
                return false;
    
            // FIN может быть и в конце выражения, если последняя строка не заканчивается переводом строки, это нормально
            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                return true; // normal stop

            if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_LINEFEED) // Разбор выражения закончился не строкой?
            {
                expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_LINEFEED} /* , msg */ );
                return false;
            }
        }

        return false;
    }


}; // class USketchParser



} // namespace usketch
} // namespace tokenizer
} // namespace umba

