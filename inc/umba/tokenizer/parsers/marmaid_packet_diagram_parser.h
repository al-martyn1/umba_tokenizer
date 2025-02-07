/*! \file
    \brief Парсер marmaid packet diagram, расширенная версия, с использованием типов и массивов
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
#include "parser_base.h"
#include "marmaid_packet_diagram_parser_types.h"
#include "umba/tokenizer/lang/marmaid_packet_diagram.h"

//
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
#include "umba/the.h"


//
#include <stdexcept>
#include <initializer_list>


//----------------------------------------------------------------------------
// umba::tokenizer::marmaid::
namespace umba {
namespace tokenizer {
namespace marmaid {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType>
class PacketDiagramParser : public umba::tokenizer::ParserBase<TokenizerType>
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

    using PacketDiagramItemType    = PacketDiagramItem<TokenCollectionItemType>;
    using PacketDiagramType        = PacketDiagram<TokenCollectionItemType>;


    PacketDiagramType              diagram;
    shared_log_type                log    ;


    UMBA_RULE_OF_FIVE_COPY_DELETE(PacketDiagramParser);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(PacketDiagramParser);

    PacketDiagramParser() {}

    explicit
    PacketDiagramParser(std::shared_ptr<TokenCollectionType> tc, shared_log_type a_log)
    : BaseClass(tc)
    , log(a_log)
    {
    }


    bool addDiagramItem(PacketDiagramItemType item)
    {
        if (item.text.empty())
        {
            BaseClass::logMessage(item.pTokenInfo, "diagram", "name is empty");
            return false;
        }

        std::uint64_t calculatedStart = 0;

        if (!diagram.data.empty())
        {
            calculatedStart = diagram.data.back().end+1;
        }

        if (item.rangeType==EPacketDiagramRangeType::range)
        {
            if (item.start>item.end)
            {
                BaseClass::logMessage( item.pTokenInfo, "diagram", "entry '$(Name)': range start offset is greater than range end offset"
                                     , umba::FormatMessage<std::string>().arg("Name", item.text).values()
                                     );
                return false;
            }
        }

        if (item.rangeType==EPacketDiagramRangeType::singleValue || item.rangeType==EPacketDiagramRangeType::range)
        {
            if (item.start!=calculatedStart)
            {
                if (item.start<calculatedStart)
                {
                    if (item.rangeType==EPacketDiagramRangeType::singleValue)
                        BaseClass::logMessage( item.pTokenInfo, "diagram", "entry '$(Name)': range start offset value is less than previous value end"
                                             , umba::FormatMessage<std::string>().arg("Name", item.text).values()
                                             );
                    else
                        BaseClass::logMessage(item.pTokenInfo, "diagram", "entry '$(Name)': entry offset value is less than previous value end"
                                             , umba::FormatMessage<std::string>().arg("Name", item.text).values()
                                             );
                }
                else
                {
                    BaseClass::logMessage(item.pTokenInfo, "diagram", "entry '$(Name)': gap detected: previous entry '$(PrevName)' ends with $(PrevEnd), but this entry starts with $(CurStart)"
                                         , umba::FormatMessage<std::string>().arg("Name", item.text)
                                                                             .arg("PrevName", diagram.data.back().text)
                                                                             .arg("PrevEnd", diagram.data.back().end)
                                                                             .arg("CurStart", item.start)
                                                                             .values()
                                         );
                }

                return false;
            }
        
        }

        // Вроде всё проверили

        // Теперь фиксим поля

        if (item.rangeType==EPacketDiagramRangeType::explicitType)
        {
            item.start = calculatedStart;
            item.end   = item.start + item.getTypeFieldSize();
        }
        else if (item.rangeType==EPacketDiagramRangeType::singleValue)
        {
            item.end = item.start;
        }

        diagram.data.emplace_back(item);

        return true;
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
            /*
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return "type"; // return "char"  ; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return "type"; // return "int8"  ; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return "type"; // return "int16" ; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return "type"; // return "int32" ; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return "type"; // return "int64" ; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return "type"; // return "uint8" ; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return "type"; // return "uint16"; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return "type"; // return "uint32"; 
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return "type"; // return "uint64"; 
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
            case MARMAID_TOKEN_OPERATOR_EXTRA  : return "extra-options-operator";
            
            case UMBA_TOKENIZER_TOKEN_CTRL_FIN: return "EOF";

            case MARMAID_TOKEN_OPERATOR_RANGE: return "range-operator";
            case MARMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER: return "follow-delimiter";

            // case : return "";
            // case : return "";
            // case : return "";
            // case : return "";

            default: 
                 if (tk>=MARMAID_TOKEN_SET_DIRECTIVES_FIRST && tk<=MARMAID_TOKEN_SET_DIRECTIVES_LAST)
                     return "directive";

                 if (tk>=MARMAID_TOKEN_SET_TYPES_FIRST && tk<=MARMAID_TOKEN_SET_TYPES_LAST)
                     return "type";

                 if (tk>=MARMAID_TOKEN_SET_OPERATORS_FIRST && tk<=MARMAID_TOKEN_SET_OPERATORS_LAST )
                     return "operator";

                 if (tk>=MARMAID_TOKEN_SET_ATTRS_FIRST && tk<=MARMAID_TOKEN_SET_ATTRS_LAST )
                     return "attribute/option";

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

    //! Разбираем диапазон. Возвращается следующий токен для анализа и продолжения разбора строки или ноль при ошибке
    const TokenInfoType* parseRange(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, PacketDiagramItemType &item)
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        // if (!checkNotNul(pTokenInfo))
        //     return 0;

        using umba::tokenizer::ParserWaitForTokenFlags;

        pTokenInfo = parseNumber(tokenPos, pTokenInfo, item.start, "start offset value");
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!checkExactTokenType(pTokenInfo, {MARMAID_TOKEN_OPERATOR_RANGE, MARMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER}, "failed reading offset/range"))
            return 0;

        if (pTokenInfo->tokenType==MARMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER)
        {
            item.rangeType = EPacketDiagramRangeType::singleValue;
            return pTokenInfo;
        }

        // range sign '-' (MARMAID_TOKEN_OPERATOR_RANGE)
        // Читаем второе число из диапазона
        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!isAnyNumber(pTokenInfo->tokenType))
            return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "failed reading range end"), (const TokenInfoType*)0;

        pTokenInfo = parseNumber(tokenPos, pTokenInfo, item.end, "range end value");
        item.rangeType = EPacketDiagramRangeType::range;

        return pTokenInfo;
    }

    //! Разбираем тип. Возвращается следующий токен для анализа и продолжения разбора строки или ноль при ошибке
    const TokenInfoType* parseType(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, PacketDiagramItemType &item)
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        using umba::tokenizer::ParserWaitForTokenFlags;

        item.rangeType           = EPacketDiagramRangeType::explicitType;
        item.explicitTypeTokenId = pTokenInfo->tokenType;

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN)
            return pTokenInfo; // Не массив

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "invalid array size"))
            return 0;

        pTokenInfo = parseNumber(tokenPos, pTokenInfo, item.arraySize, "array size");
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE}, "invalid array size"))
            return 0;

        return BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    }

    //! Разбор директивы packet-beta. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    const TokenInfoType* parsePacketBetaDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (pTokenInfo->tokenType!=MARMAID_TOKEN_OPERATOR_EXTRA)
            return pTokenInfo; // Что-то пришло, но не расширенные опции

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        // В данном контексте MARMAID_TOKEN_ATTR_BYTE_DIA==MARMAID_TOKEN_ATTR_BYTE
        // и MARMAID_TOKEN_ATTR_BIT_DIA==MARMAID_TOKEN_ATTR_BIT

        if (!checkExactTokenType(pTokenInfo, {MARMAID_TOKEN_ATTR_BYTE_DIA, MARMAID_TOKEN_ATTR_BIT_DIA, MARMAID_TOKEN_ATTR_MEMORY_DIA, MARMAID_TOKEN_ATTR_BYTE, MARMAID_TOKEN_ATTR_BIT, UMBA_TOKENIZER_TOKEN_LINEFEED}, "invalid 'packet-beta' directive options"))
            return 0;

        if ((diagram.parsingOptions&DiagramParsingOptions::allowOverrideType)!=0u)
        {
            switch(pTokenInfo->tokenType)
            {
                case MARMAID_TOKEN_ATTR_BYTE      :  [[fallthrough]]
                case MARMAID_TOKEN_ATTR_BYTE_DIA  :  diagram.diagramType = EPacketDiagramType::byteDiagram; break;
                case MARMAID_TOKEN_ATTR_BIT       :  [[fallthrough]]
                case MARMAID_TOKEN_ATTR_BIT_DIA   :  diagram.diagramType = EPacketDiagramType::bitDiagram ; break;

                case MARMAID_TOKEN_ATTR_MEMORY_DIA:  diagram.diagramType = EPacketDiagramType::memDiagram ; break;
                case UMBA_TOKENIZER_TOKEN_LINEFEED:  return pTokenInfo;
            }
        }

        return BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    }

    //! Разбор директивы packet-beta. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    const TokenInfoType* parseDisplayWidthDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "invalid 'display-width' directive: missing width value"))
            return 0;

        std::uint64_t displayWidth = 0;
        pTokenInfo = parseNumber(tokenPos, pTokenInfo, displayWidth, "'display-width' directive");
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if ((diagram.parsingOptions&DiagramParsingOptions::allowOverrideDisplayWidth)!=0u)
        {
            diagram.displayWidth = displayWidth;
        }

        return pTokenInfo;
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

        if ((diagram.parsingOptions&DiagramParsingOptions::allowOverrideTitle)!=0u)
        {
            const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
            auto rawData = std::get<typename tokenizer_type::RawDataHolder>(*pParsedData);
            diagram.title = umba::string::trim_copy(rawData.pData->asString());
        }

        return BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    }

    // native le 32 bits bit byte bytes
    //! Разбор директивы native. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    const TokenInfoType* parseNativeDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    {
        bool endiannessTaken = false;
        //bool sizeTaken       = false;
        const TokenInfoType* pTokenInfoBitSize = 0;

        std::uint64_t       bitSize = 0;
        Endianness          endianness = Endianness::littleEndian;
        EPacketDiagramType  bitSizeSizeType = EPacketDiagramType::unknown; // Используем не по назначению, а для хринения типа биты или байты в числе (bitDiagram, byteDiagram)

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);

        auto optionsCounter = -1;

        //------------------------------
        auto returnCheckUpdateOptions = [&]() -> const TokenInfoType*
        {
            if (optionsCounter<1)
                return BaseClass::logMessage(pTokenInfo, "native-options", "there is no options taken in the 'native' directive"), (const TokenInfoType*)0;

            if (pTokenInfoBitSize)
            {
                if (bitSizeSizeType==EPacketDiagramType::unknown)
                {
                    // детектим бит/байт размер, 16/32/64/128/256 - размер в битах
                    //                           1/2/4    - размер в байтах
                    //                           8        - неоднозначно, 8 бит или 8 байт (64 бита?)
                    //                           16 и 32 в качестве размера в байтах никто уже не задаст в здравом уме - будут использовать битность 128/256
                    if (!umba::TheValue(bitSize).oneOf(1u,2u,4u,8u,16u,32u,64u,128u,256u))
                        return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size must be 1/2/4/8 bytes ot 8/16/32/64/128/256 bits"), (const TokenInfoType*)0;

                    if (bitSize==8)
                        return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size 8 bits or bytes?"), (const TokenInfoType*)0;

                    if (!umba::TheValue(bitSize).oneOf(1u,2u,4u))
                        bitSize *= 8; // Из байт в биты
                }
                else if (bitSizeSizeType==EPacketDiagramType::byteDiagram)
                {
                    if (!umba::TheValue(bitSize).oneOf(1u,2u,4u,8u))
                        return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size must be 1/2/4/8 bytes ot 8/16/32/64/128/256 bits"), (const TokenInfoType*)0;
                    bitSize *= 8; // Из байт в биты
                }
                else // bits
                {
                    if (!umba::TheValue(bitSize).oneOf(8u,16u,32u,64u,128u,256u))
                        return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size must be 1/2/4/8 bytes ot 8/16/32/64/128/256 bits"), (const TokenInfoType*)0;
                    // Только проверка, перевода нет, и так биты
                }

                if ((diagram.parsingOptions&DiagramParsingOptions::allowOverrideBitSize)!=0u)
                {
                    diagram.bitSize = bitSize;
                }

            }

            if (endiannessTaken)
            {
                if ((diagram.parsingOptions&DiagramParsingOptions::allowOverrideEndianness)!=0u)
                {
                    diagram.endianness = endianness;
                }
            }

            return pTokenInfo;
        };

        //------------------------------
        auto updateEndianness = [&](Endianness e)
        {
            if (endiannessTaken)
                return BaseClass::logMessage(pTokenInfo, "native-options", "endianness already taken"), false;

            endianness = e;
            endiannessTaken = true;

            return true;
        };


        //------------------------------
        while(true)
        {
            if (!pTokenInfo)
                return 0; // Сообщение уже выведено, просто возвращаем ошибку

            ++optionsCounter;

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
            {
                return returnCheckUpdateOptions();
            }

            if (!isAnyNumber(pTokenInfo->tokenType) && !umba::TheValue(pTokenInfo->tokenType).oneOf(MARMAID_TOKEN_ATTR_LE,MARMAID_TOKEN_ATTR_BE, UMBA_TOKENIZER_TOKEN_LINEFEED))
            {
                expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, MARMAID_TOKEN_ATTR_LE /* , MARMAID_TOKEN_ATTR_BE */  | UMBA_TOKENIZER_TOKEN_LINEFEED }, "invalid 'native' directive options" );
                return 0;
            }

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
            {
                return returnCheckUpdateOptions();
            }

            if (pTokenInfo->tokenType==MARMAID_TOKEN_ATTR_LE)
            {
                if (!updateEndianness(Endianness::littleEndian))
                    return 0;
                pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            }

            else if (pTokenInfo->tokenType==MARMAID_TOKEN_ATTR_BE)
            {
                if (!updateEndianness(Endianness::bigEndian))
                    return 0;
                pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            }

            else // Остались толькор числа
            {
                if (pTokenInfoBitSize)
                    return BaseClass::logMessage(pTokenInfo, "native-options", "bit size already taken"), (const TokenInfoType*)0;

                pTokenInfoBitSize = pTokenInfo;

                pTokenInfo = parseNumber(tokenPos, pTokenInfo, bitSize, "'native' directive");
                if (!pTokenInfo)
                    return 0; // Сообщение уже выведено, просто возвращаем ошибку

                pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                if (!pTokenInfo)
                    return 0; // Сообщение уже выведено, просто возвращаем ошибку

                if (pTokenInfo->tokenType==MARMAID_TOKEN_ATTR_BYTE || pTokenInfo->tokenType==MARMAID_TOKEN_ATTR_BIT )
                {
                    bitSizeSizeType = (pTokenInfo->tokenType==MARMAID_TOKEN_ATTR_BYTE) ? EPacketDiagramType::byteDiagram : EPacketDiagramType::bitDiagram;
                    pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                }
            }
        }

    }

    //! Разбор обычной строки. Может возвращать ноль при ошибке, или токен LF/FIN
    const TokenInfoType* parseRegularLine(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, PacketDiagramItemType &item)
    {
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (isAnyNumber(pTokenInfo->tokenType))
            pTokenInfo = parseRange(tokenPos, pTokenInfo, item);
        else if (isAnyType(pTokenInfo->tokenType))
            pTokenInfo = parseType(tokenPos, pTokenInfo, item);
        else
            return 0;

        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку
        if (!checkExactTokenType(pTokenInfo, {MARMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER}, "invalid record definition"))
            return 0;

        // Читаем следующий токен
        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
        if (!checkNotNul(pTokenInfo))
            return 0;

        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL}, "invalid record definition"))
            return 0;

        const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
        auto stringLiteralData = std::get<typename tokenizer_type::StringLiteralDataHolder>(*pParsedData);
        item.text = stringLiteralData.pData->asString();

        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);

        if (pTokenInfo->tokenType!=MARMAID_TOKEN_OPERATOR_EXTRA)
            return pTokenInfo;

        Endianness endianness = Endianness::unknown;
        bool hasCrc   = false;
        bool hasSeed  = false;
        bool hasPoly  = false;
        //bool hasRange = false;

        std::uint64_t crcRangeStart = 0;
        std::uint64_t crcRangeEnd   = 0;

        auto returnCheckUpdateOptions = [&]()
        {
            return pTokenInfo;
        };


        // %%#! le be middle-endian le-me be-me crc 0-10 seed 10 poly 0x1234

        while(true)
        {
            if (!pTokenInfo)
                return 0; // Сообщение уже выведено, просто возвращаем ошибку

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
            {
                return returnCheckUpdateOptions();
            }

            if ( !isAnyNumber(pTokenInfo->tokenType)
              && !umba::TheValue(pTokenInfo->tokenType)
                     .oneOf( MARMAID_TOKEN_ATTR_LE, MARMAID_TOKEN_ATTR_BE
                           , MARMAID_TOKEN_ATTR_ME, MARMAID_TOKEN_ATTR_LE_ME, MARMAID_TOKEN_ATTR_BE_ME
                           , UMBA_TOKENIZER_TOKEN_LINEFEED
                           , MARMAID_TOKEN_ATTR_CRC, MARMAID_TOKEN_ATTR_SEED, MARMAID_TOKEN_ATTR_POLY
                           )
               )
            {
                expectedReachedMsg(pTokenInfo, { /* UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, */  MARMAID_TOKEN_ATTR_LE /* , MARMAID_TOKEN_ATTR_BE */  | UMBA_TOKENIZER_TOKEN_LINEFEED }, "invalid 'native' directive options" );
                return 0;
            }

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
            {
                return returnCheckUpdateOptions();
            }



        }
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


            if (isAnyNumber(pTokenInfo->tokenType) || isAnyType(pTokenInfo->tokenType))
            {
                PacketDiagramItemType item;
                item.pTokenInfo = pTokenInfo; // На всякий случай инфу сохраняем
                pTokenInfo = parseRegularLine(tokenPos, pTokenInfo, item);
                if (!pTokenInfo)
                    return false;
                if (!addDiagramItem(item))
                    return false;
            }

            else if (pTokenInfo->tokenType==MARMAID_TOKEN_DIRECTIVE_PACKET_BETA)
            {
                pTokenInfo = parsePacketBetaDirective(tokenPos, pTokenInfo);
            }

            else if (pTokenInfo->tokenType==MARMAID_TOKEN_DIRECTIVE_TITLE)
            {
                pTokenInfo = parseTitleDirective(tokenPos, pTokenInfo);
            }

            else if (pTokenInfo->tokenType==MARMAID_TOKEN_OPERATOR_EXTRA)
            {
                pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                if (!pTokenInfo)
                    return 0; // Сообщение уже выведено, просто возвращаем ошибку

                if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
                    continue;
    
                if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                    return true; // normal stop
    
                if (!umba::TheValue(pTokenInfo->tokenType).oneOf(MARMAID_TOKEN_DIRECTIVE_NATIVE, MARMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH))
                {
                     expectedReachedMsg(pTokenInfo, {MARMAID_TOKEN_DIRECTIVE_NATIVE } /* , msg */ );
                     return 0;
                }
                // if (!checkExactTokenType(pTokenInfo, {MARMAID_TOKEN_DIRECTIVE_NATIVE, MARMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH} /* , "invalid 'packet-beta' directive options" */ ))
                //     return 0;

                if (pTokenInfo->tokenType==MARMAID_TOKEN_DIRECTIVE_NATIVE)
                {
                    pTokenInfo = parseNativeDirective(tokenPos, pTokenInfo);
                }
    
                else if (pTokenInfo->tokenType==MARMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH)
                {
                    pTokenInfo = parseDisplayWidthDirective(tokenPos, pTokenInfo);
                }
            }

            else
            {
                expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, MARMAID_TOKEN_SET_TYPES_FIRST, MARMAID_TOKEN_DIRECTIVE_PACKET_BETA, MARMAID_TOKEN_OPERATOR_EXTRA /* , MARMAID_TOKEN_DIRECTIVE_TITLE */ } /* , msg */ );
                return false;
            }

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


}; // class PacketDiagramParser



} // namespace marmaid
} // namespace tokenizer
} // namespace umba

