/*! \file
    \brief Парсер marmaid packet diagram, расширенная версия, с использованием типов и массивов
 */


#pragma once


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


enum class PacketDiagramRangeType
{
    singleValue,
    range      ,
    explicitType
};

enum class PacketDiagramType
{
    unknown,
    bitDiagram,
    byteDiagram
};


template<typename TokenCollectionItemType>
struct PacketDiagramItem
{
    PacketDiagramRangeType          rangeType;
    umba::tokenizer::payload_type   explicitTypeTokenId;
    std::string                     text;

    std::uint64_t                   start;
    std::uint64_t                   end  ; // входит в диапазон
    std::uint64_t                   arraySize = std::uint64_t(-1);

    const TokenCollectionItemType   *pTokenInfo = 0; // стартовый токен, можно получить номер строки
};


template<typename TokenCollectionItemType>
struct PacketDiagram
{
    using PacketDiagramItemType = PacketDiagramItem<TokenCollectionItemType>;

    PacketDiagramType                      diagramType = PacketDiagramType::unknown;
    std::string                            title;
    std::vector<PacketDiagramItemType>     data ;

};




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
            case UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION: return "range-operator";
            case UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE: return "follow-delimiter";
            case UMBA_TOKENIZER_TOKEN_STRING_LITERAL     : return "string";
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN : return "array-size-start-bracket";
            case UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE: return "array-size-end-bracket";
            case UMBA_TOKENIZER_TOKEN_LINEFEED : return "linefeed";
            case UMBA_TOKENIZER_TOKEN_SPACE    : return "space";
            case UMBA_TOKENIZER_TOKEN_TAB      : return "tab";
            case UMBA_TOKENIZER_TOKEN_FORM_FEED: return "form-feed";
            
            case UMBA_TOKENIZER_TOKEN_CTRL_FIN: return "EOF";
            // case : return "";
            // case : return "";
            // case : return "";
            // case : return "";

            default: 
                 if (tk>=MARMAID_TOKEN_SET_DIRECTIVES_FIRST && tk<=MARMAID_TOKEN_SET_DIRECTIVES_LAST)
                     return "directive";

                 if (tk>=MARMAID_TOKEN_SET_TYPES_FIRST && tk<=MARMAID_TOKEN_SET_TYPES_LAST)
                     return "type";

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

    //! Разбираем токен числового литерала. Возвращается следующий токен
    const TokenInfoType *pTokenInfo parseNumber(const TokenInfoType *pTokenInfo, std::uint64_t &val, const std::string &msg=std::string())
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        // if (!checkNotNul(pTokenInfo))
        //     return 0;

        using umba::tokenizer::ParserWaitForTokenFlags;

        if (!isAnyNumber(pTokenInfo->tokenType))
            return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, msg), 0;

        const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
        auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
        if (numericLiteralData.pData->fOverflow)
            return BaseClass::logSimpleMessage(pTokenInfo, "integer-overflow", msg.empty() ? "integer overflow" : msg + ": integer overflow"), 0;

        val = std::uint64_t(numericLiteralData.pData->value);

        return BaseClass::waitForSignificantToken( 0 /* &tokenPos */ , ParserWaitForTokenFlags::stopOnLinefeed);
    }

    //! Разбираем диапазон. Возвращается следующий токен
    const TokenInfoType *pTokenInfo parseRange(const TokenInfoType *pTokenInfo, PacketDiagramItemType &item)
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        // if (!checkNotNul(pTokenInfo))
        //     return 0;

        using umba::tokenizer::ParserWaitForTokenFlags;

        pTokenInfo = parseNumber(pTokenInfo, item.start, "start offset value");
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION, UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE}, "reading offset/range"))
            return 0;

        if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE)
        {
            item.rangeType = PacketDiagramRangeType::singleValue;
            return pTokenInfo;
        }

        // range sign '-' (UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION)
        // Читаем второе число из диапазона
        pTokenInfo = BaseClass::waitForSignificantToken( 0 /* &tokenPos */ , ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!isAnyNumber(pTokenInfo->tokenType))
            return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "reading range end"), 0;

        pTokenInfo = parseNumber(pTokenInfo, item.end, "range end value");
        item.rangeType = PacketDiagramRangeType::range;

        return pTokenInfo;
    }

    //! Разбираем тип. Возвращается следующий токен
    const TokenInfoType *pTokenInfo parseType(const TokenInfoType *pTokenInfo, PacketDiagramItemType &item)
    {
        // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке

        using umba::tokenizer::ParserWaitForTokenFlags;

        item.rangeType           = PacketDiagramRangeType::explicitType;
        item.explicitTypeTokenId = pTokenInfo->tokenType;

        pTokenInfo = BaseClass::waitForSignificantToken( 0 /* &tokenPos */ , ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN)
            return pTokenInfo; // Не массив

        pTokenInfo = BaseClass::waitForSignificantToken( 0 /* &tokenPos */ , ParserWaitForTokenFlags::stopOnLinefeed);
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "invalid array size"))
            return 0;

        pTokenInfo = parseNumber(pTokenInfo, item.arraySize, "array size");
        if (!pTokenInfo)
            return 0; // Сообщение уже выведено, просто возвращаем ошибку

        if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE}, "invalid array size"))
            return 0;

        return BaseClass::waitForSignificantToken( 0 /* &tokenPos */ , ParserWaitForTokenFlags::stopOnLinefeed);
    }

    const TokenInfoType *pTokenInfo parsePacketBetaDirective(const TokenInfoType *pTokenInfo, PacketDiagramItemType &item)









    bool parse()
    {
        using umba::tokenizer::ParserWaitForTokenFlags;
        // TokenPosType tokenPos;

        while(true)
        {
            PacketDiagramItemType item;

            const TokenInfoType *pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            if (!checkNotNul(pTokenInfo))
                return false;

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
                continue;

            if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                return true; // normal stop

            item.pTokenInfo = pTokenInfo; // На всякий случай инфу сохраняем

            if (isAnyNumber(pTokenInfo->tokenType))
            {
                pTokenInfo = parseRange(pTokenInfo, item);
            }

            else if (isAnyType(pTokenInfo->tokenType))
            {
            
            }


            if (isAnyNumber(pTokenInfo->tokenType))
            {
                const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
                auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
                if (numericLiteralData.pData->fOverflow)
                {
                    BaseClass::logSimpleMessage(pTokenInfo, "integer-oveflow", "integer oveflow");
                    return false;
                }

                item.rangeType = PacketDiagramRangeType::singleValue;
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
                        return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "invalid range");

                    pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
                    // auto 
                    numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
                    if (numericLiteralData.pData->fOverflow)
                    {
                        BaseClass::logSimpleMessage(pTokenInfo, "integer-oveflow", "integer oveflow");
                        return false;
                    }
        
                    item.rangeType = PacketDiagramRangeType::range;
                    item.end       = std::uint64_t(numericLiteralData.pData->value);

                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                }
            }

            else if (isAnyType(pTokenInfo->tokenType))
            {
                item.rangeType           = PacketDiagramRangeType::explicitType;
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
                        return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "invalid array size");

                    const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
                    auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
                    if (numericLiteralData.pData->fOverflow)
                    {
                        BaseClass::logSimpleMessage(pTokenInfo, "integer-oveflow", "integer oveflow");
                        return false;
                    }
        
                    item.arraySize = std::uint64_t(numericLiteralData.pData->value);

                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
                    if (!checkNotNul(pTokenInfo))
                        return false;

                    if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE)
                        return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE}, "invalid array size");
                
                    pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);

                }
            }

            else if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
            {
                continue;
            }

            else if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
            {
                return false; // true;
            }

            else
            {
                return expectedReachedMsg(pTokenInfo, {MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR, UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC} /* , std::string() */ );
            }

            // Тут у нас в pTokenInfo должен быть следующий токен
            if (!checkNotNul(pTokenInfo))
                return false;

            if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE}, "invalid record definition"))
                return false;

            pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            if (!checkNotNul(pTokenInfo))
                return false;

            if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL}, "invalid record definition"))
                return false;

            const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
            auto stringLiteralData = std::get<typename tokenizer_type::StringLiteralDataHolder>(*pParsedData);
            item.text = stringLiteralData.pData->asString();

            pTokenInfo = BaseClass::waitForSignificantToken(&tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
            if (!checkNotNul(pTokenInfo))
                return false;

            if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_LINEFEED, UMBA_TOKENIZER_TOKEN_CTRL_FIN}, "invalid record definition"))
                return false;

            diagram.data.emplace_back(item);
        }

        return false;
    }


}; // class PacketDiagramParser



} // namespace marmaid
} // namespace tokenizer
} // namespace umba

