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
#include "umba/tokenizer/lang/marmaid_packet_diagram.h"

//
#include <stdexcept>
#include <initializer_list>


// umba::tokenizer::marmaid::
namespace umba {
namespace tokenizer {
namespace marmaid {


enum class EPacketDiagramType
{
    unknown,
    bitDiagram,
    byteDiagram
};


enum class EPacketDiagramRangeType
{
    singleValue,
    range      ,
    explicitType
};

inline
std::string makeCppNameFromText(const std::string &text, bool bCpp)
{
    UMBA_USED(bCpp);

    std::string res = text;
    for(auto &ch : res)
    {
        if (ch>='A' && ch<='Z')
            continue;
        if (ch>='a' && ch<='z')
            continue;
        if (ch>='0' && ch<='9')
            continue;

        ch = '_';
    }

    return res;
}




template<typename TokenCollectionItemType>
struct PacketDiagramItem
{
    EPacketDiagramRangeType         rangeType;
    umba::tokenizer::payload_type   explicitTypeTokenId;
    std::string                     text;

    std::uint64_t                   start; // Вычисляем при добавлении, если указан тип
    std::uint64_t                   end  ; // Вычисляем при добавлении, если указан тип; ходит в диапазон
    std::uint64_t                   arraySize = std::uint64_t(-1);

    std::string                     realTypeName; // For C++ output, full qualified

    const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки

    // Только размер типа
    std::uint64_t getTypeSize() const
    {
        UMBA_ASSERT(rangeType==EPacketDiagramRangeType::explicitType);
        return std::size_t(explicitTypeTokenId&0x0F);
    }

    // Размер типа * кол-во элеметов
    std::uint64_t getTypeFieldSize() const
    {
        if (arraySize==std::uint64_t(-1))
            return getTypeSize();
        return arraySize*getTypeSize();
    }

    bool isArray() const
    {
        if (rangeType==EPacketDiagramRangeType::explicitType)
        {
            if (arraySize==std::uint64_t(-1))
                return false;
            else
                return true;
        }

        if (rangeType==EPacketDiagramRangeType::singleValue)
            return false;

        return true;
    }

    std::uint64_t getArraySize() const
    {
        if (rangeType==EPacketDiagramRangeType::explicitType)
        {
            if (arraySize==std::uint64_t(-1))
                return 0;
            else
                return arraySize;
        }

        if (rangeType==EPacketDiagramRangeType::singleValue)
            return 0;

        return end-start+1;
    }

    std::string getPlainTypeName() const // For plain C
    {
        if (!realTypeName.empty())
            return realTypeName;

        if (rangeType!=EPacketDiagramRangeType::explicitType)
            return "uint8_t"; // threat singles and ranges as bytes

        switch(explicitTypeTokenId)
        {
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return "char"   ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return "int8_t" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return "int16_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return "int32_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return "int64_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return "uint8_t" ;
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return "uint16_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return "uint32_t";
            case MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return "uint64_t";

            default: return "unknown";
        }

    }

    std::string getCppTypeName() const // For C++
    {
        std::string name = getPlainTypeName();
        if (!realTypeName.empty())
            return name;

        if (rangeType!=EPacketDiagramRangeType::explicitType)
            return "std::" + name;

        if (explicitTypeTokenId!=MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR)
            return "std::" + name;

        return name;
    }

    std::string getCppOrCTypeName(bool bCpp) const
    {
        return bCpp ? getCppTypeName() : getPlainTypeName();
    }

    std::string getCppOrCFieldName(bool bCpp) const
    {
        return makeCppNameFromText(text, bCpp);
    }

    

}; // struct PacketDiagramItem



template<typename TokenCollectionItemType>
struct PacketDiagram
{
    using PacketDiagramItemType = PacketDiagramItem<TokenCollectionItemType>;

    EPacketDiagramType                     diagramType = EPacketDiagramType::unknown;
    std::string                            title;
    std::vector<PacketDiagramItemType>     data ;
    bool                                   allowOverrideTitle = true;

    std::string getCppOrCTitle(bool bCpp) const
    {
        return title.empty() ? std::string("untitled") : makeCppNameFromText(title, bCpp);
    }

}; // struct PacketDiagram



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

        // считываем коммент, если есть
        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed | ParserWaitForTokenFlags::stopOnComment);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (pTokenInfo->tokenType<UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST || pTokenInfo->tokenType>UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST)
            return pTokenInfo;

        const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
        auto commentData = std::get<typename tokenizer_type::CommentDataHolder>(*pParsedData);
        auto commentDataLower = umba::string::tolower_copy(commentData.pData->asString());

        // Заранее читаем следующий токен
        pTokenInfo = BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);

        if (commentDataLower.empty() || commentDataLower[0]!='!')
            return pTokenInfo;

        commentDataLower.erase(0, 1);
        commentDataLower = umba::string::trim_copy(commentDataLower);

        if (commentDataLower.empty())
            return pTokenInfo;

        if (commentDataLower=="bit" || commentDataLower=="bit-diagram")
            diagram.diagramType = EPacketDiagramType::bitDiagram;
        else if (commentDataLower=="byte" || commentDataLower=="byte-diagram")
            diagram.diagramType = EPacketDiagramType::byteDiagram;

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
        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_RAW_DATA}, "invalid title directive"))
            return 0;


        //auto pTextEnd   = BaseClass::getTextPointer(pTokenInfo);

        if (diagram.allowOverrideTitle)
        {
            //pTextStart += 5; // скипнули сам токен title
            // if (pTextStart<=pTextEnd)
            // diagram.title = umba::string::trim_copy(std::string(pTextStart, pTextEnd));
            const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
            auto rawData = std::get<typename tokenizer_type::RawDataHolder>(*pParsedData);
            diagram.title = umba::string::trim_copy(rawData.pData->asString());
        }

        return BaseClass::waitForSignificantToken( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
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
            else
            {
                expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, MARMAID_TOKEN_SET_TYPES_FIRST, MARMAID_TOKEN_DIRECTIVE_PACKET_BETA /* , MARMAID_TOKEN_DIRECTIVE_TITLE */ } /* , msg */ );
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

