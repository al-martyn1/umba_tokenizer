/*! \file
    \brief Парсер языка Umba FSM
 */


#pragma once

#include "umba/string_plus.h"
//

#include "umba/tokenizer.h"
//
#include "umba/tokenizer/tokenizer_log.h"
#include "umba/tokenizer/tokenizer_log_console.h"
#include "umba/tokenizer/token_collection.h"
#include "marty_expressions/tokenizer_helpers.h"
//
#include "../utils.h"
#include "../parser_base.h"
#include "types.h"
#include "umba/tokenizer/lexers/ufsm.h"

//
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
#include "umba/the.h"


//
#include <stdexcept>
#include <initializer_list>


//----------------------------------------------------------------------------
// #include "umba/tokenizer/parsers/ufsm/parser.h"
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



struct ParsingContext
{
    NamespaceDefinition       rootNs   ;

    FullQualifiedName         curNsName;

    // При открытии NS может использоваться сразу пачка имён, 
    // `namespace aa::bb::cc`, и нам надо знать, сколько имён
    // удалять из текущего имени
    std::vector<std::size_t>  nsOpenLevels;

    FullTokenPosition         lastNsPos;


    ParsingContext()
    {
        curNsName.flags |= FullQualifiedNameFlags::absolute;
    }

    void appendCurNsName(const FullQualifiedName &n)
    {
        nsOpenLevels.emplace_back(n.size());
        curNsName.append(n);
    }

}; // struct ParsingContext

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------



//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
template<typename TokenizerType>
class Parser : public umba::tokenizer::ParserBase<TokenizerType>
{

public:

    using BaseClass                = umba::tokenizer::ParserBase<TokenizerType>;
    using tokenizer_type           = TokenizerType;
    using TokenCollectionType      = typename BaseClass::TokenCollectionType     ;
    using TokenCollectionItemType  = typename BaseClass::TokenCollectionItemType ;
    using TokenInfoType            = typename BaseClass::TokenInfoType           ;
    using TokenPosType             = typename BaseClass::TokenPosType            ;
    using token_parsed_data_type   = typename tokenizer_type::token_parsed_data_type;
    using shared_log_type          = umba::tokenizer::log::SharedParserErrorLog;
    using string_type              = typename BaseClass::string_type;
    using SharedFilenameSetType    = typename BaseClass::SharedFilenameSetType   ;

    // using PacketDiagramItemType    = PacketDiagramItem<TokenCollectionItemType>;
    // using PacketDiagramType        = PacketDiagram<TokenCollectionItemType>;


    shared_log_type                log    ;


protected:

    ParsingContext                 ctx;
    const TokenInfoType            *m_pTokenInfo = 0;
    TokenPosType                   m_tokenPos;


public:

    UMBA_RULE_OF_FIVE_COPY_DELETE(Parser);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(Parser);

    Parser() {}

    explicit
    Parser(std::shared_ptr<TokenCollectionType> tc, SharedFilenameSetType pFilenameSet, shared_log_type a_log)
    : BaseClass(tc, pFilenameSet)
    , log(a_log)
    , ctx()
    {
    }

    NamespaceDefinition getParsedData() const
    {
        return ctx.rootNs;
    }

    std::string extractIdentifierName(const TokenInfoType *pTokenInfo)
    {
        auto pTokenParsedData = BaseClass::getTokenParsedDataPtr(pTokenInfo);
        auto identifierData = std::get<typename TokenizerType::IdentifierDataHolder>(*pTokenParsedData);
        return identifierData.pData->value;
    }

    std::string extractLiteral(const TokenInfoType *pTokenInfo)
    {
        auto pTokenParsedData = BaseClass::getTokenParsedDataPtr(pTokenInfo);
        auto literalData = std::get<typename TokenizerType::StringLiteralDataHolder>(*pTokenParsedData);
        return literalData.pData->value;
    }

    FullTokenPosition getFullPos(TokenPosType pos) const  { return BaseClass::getFullTokenPosition(pos); }
    FullTokenPosition getFullPos() const                  { return getFullPos(m_tokenPos); }


    NamespaceDefinition* getCurrentNamespace()
    {
        NamespaceDefinition* pCurNs = ctx.rootNs.getNamespace(ctx.curNsName, &ctx.rootNs, ctx.lastNsPos);
        if (!pCurNs)
            throw std::runtime_error("Something goes wrong");
        return pCurNs;
    }

    void readNextToken(ParserWaitForTokenFlags readFlags=ParserWaitForTokenFlags::none)
    {
        m_pTokenInfo = BaseClass::waitForSignificantTokenChecked( &m_tokenPos, readFlags);
    }

    // m_pTokenInfo = BaseClass::waitForSignificantTokenChecked( &m_tokenPos, ParserWaitForTokenFlags::none);
    // readNextToken();
    
    


//    template<typename TVal>
//    using TheVal = umba::TheValue<TVal>;


    // void setDiagramTitle(const std::string &t)
    // {
    //      diagram.title = t;
    // }

    // PacketDiagramType getDiagram() const
    // {
    //     auto diagramCopy = diagram;
    //     diagramCopy.removeEmptyOrgs();
    //     return diagramCopy;
    // }


    // bool addDiagramItem(PacketDiagramItemType item)
    // {
    //     try
    //     {
    //         return addDiagramItemImpl(item);
    //     }
    //     catch(const marty::mem::address_wrap &)
    //     {
    //         return BaseClass::logMessage( item.pTokenInfo, "address-wrap", "address wrapping detected. Check your 'org' directives" ), false;
    //     }
    // }

    // bool addDiagramItemImpl(PacketDiagramItemType item)
    // {
    //     if (item.text.empty() && item.itemType!=EPacketDiagramItemType::org) // Для org директив имя/текст необязателен
    //     {
    //         BaseClass::logMessage(item.pTokenInfo, "diagram", "data entry name is empty");
    //         return false;
    //     }
    //  
    //     auto fieldId = item.getFieldId();
    //     if (fieldId.empty() || fieldId=="_")
    //     {
    //         if (item.itemType==EPacketDiagramItemType::org)
    //         {
    //             if (fieldId=="_")
    //                return BaseClass::logMessage(item.pTokenInfo, "diagram", "'org' directive name contains only insignificant symbols"), false;
    //             // Анонимный ORG - генерим имя на базе текущего заголовка диаграмы
    //             item.text = diagram.generateOrgName(orgCounter);
    //             item.textGenerated = true;
    //             fieldId = item.getFieldId();
    //         }
    //         else
    //         {
    //             BaseClass::logMessage(item.pTokenInfo, "diagram", "data entry name is empty or contains only insignificant symbols");
    //             return false;
    //         }
    //     }
    //  
    //     // С наличием имени поля и годного идентификатора этого поля разобрались
    //  
    //     std::unordered_map<std::string, std::size_t> &entriesMap = (item.itemType==EPacketDiagramItemType::org) ? diagram.orgNames : diagram.entryNames;
    //  
    //     std::string entryTypeStr = (item.itemType==EPacketDiagramItemType::org) ? "'org' directive" : "data entry";
    //  
    //     if (entriesMap.find(fieldId)!=entriesMap.end())
    //     {
    //         BaseClass::logMessage( item.pTokenInfo, "diagram", entryTypeStr + " with same name already exists ('$(Name)')"
    //                                , umba::FormatMessage<std::string>().arg("Name", item.text)
    //                                                                    .values()
    //                              );
    //         return false;
    //     }
    //  
    //     entriesMap[fieldId] = diagram.data.size();
    //  
    //  
    //  
    //     std::uint64_t calculatedStart = 0;
    //  
    //     // Все адреса у нас не абсолютные, а относительно базы.
    //     // Директива/запись org задаёт новую базу
    //     if (!diagram.data.empty())
    //     {
    //         if (diagram.data.back().itemType==EPacketDiagramItemType::org)
    //             calculatedStart = 0;
    //         else
    //             calculatedStart = diagram.data.back().addressRange.end+1;
    //     }
    //  
    //     if (item.itemType==EPacketDiagramItemType::range)
    //     {
    //         if (item.addressRange.start>item.addressRange.end)
    //         {
    //             BaseClass::logMessage( item.pTokenInfo, "diagram", "entry '$(Name)': range start offset is greater than range end offset"
    //                                  , umba::FormatMessage<std::string>().arg("Name", item.text).values()
    //                                  );
    //             return false;
    //         }
    //     }
    //  
    //     if (item.itemType==EPacketDiagramItemType::singleValue || item.itemType==EPacketDiagramItemType::range)
    //     {
    //         if (item.addressRange.start!=calculatedStart)
    //         {
    //             if (item.addressRange.start<calculatedStart)
    //             {
    //                 std::string msg = std::string("entry '$(Name)': ") 
    //                                 + (item.itemType==EPacketDiagramItemType::singleValue ? "entry" : "range start")
    //                                 + " offset value is less than previous value end+1 ($(Start) < $(End))";
    //                 BaseClass::logMessage( item.pTokenInfo, "diagram", msg
    //                                      , umba::FormatMessage<std::string>().arg("Name" , item.text)
    //                                                                          .arg("Start", item.addressRange.start)
    //                                                                          .arg("End"  , calculatedStart)
    //                                                                          .values()
    //                                      );
    //             }
    //             else
    //             {
    //                 BaseClass::logMessage(item.pTokenInfo, "diagram", "entry '$(Name)': gap detected: previous entry '$(PrevName)' ends with $(PrevEnd), but current entry starts with $(CurStart) instead of $(ExpectedStart)"
    //                                      , umba::FormatMessage<std::string>().arg("Name"    , item.text)
    //                                                                          .arg("PrevName", diagram.data.back().text)
    //                                                                          .arg("PrevEnd" , diagram.data.back().addressRange.end)
    //                                                                          .arg("CurStart", item.addressRange.start)
    //                                                                          .arg("ExpectedStart", calculatedStart)
    //                                                                          .values()
    //                                      );
    //             }
    //  
    //             return false;
    //         }
    //     }
    //  
    //     if (item.itemType==EPacketDiagramItemType::org)
    //     {
    //         // Вычислить новый org
    //         // std::uint64_t baseAddress = diagram.calcBaseAddress();
    //         auto baseIt = diagram.getBaseAddressIterator();
    //  
    //         switch(item.orgType)
    //         {
    //             case EOrgType::orgAuto:
    //             {
    //                 // item.orgAddress = baseAddress + calculatedStart;
    //                 baseIt += std::int64_t(calculatedStart);
    //                 auto addressInfo = baseIt.getAddressInfo();
    //                 item.orgAddress  = addressInfo.base;
    //                 item.orgOffset   = addressInfo.offset;
    //                 break;
    //             }
    //  
    //             case EOrgType::orgAbs : // Ничего не делаем, задан абсолютный адрес
    //                 baseIt = diagram.createConstMemoryIterator(item);
    //                 break;
    //  
    //             case EOrgType::orgRel :
    //             {
    //                 //item.orgAddress += baseAddress; // прибавили базовый адрес, который высчитан по предыдущим org директивам
    //                 baseIt += std::int64_t(item.orgAddress);
    //                 auto addressInfo = baseIt.getAddressInfo();
    //                 item.orgAddress  = addressInfo.base;
    //                 item.orgOffset   = addressInfo.offset;
    //                 break;
    //             }
    //  
    //             case EOrgType::invalid  : [[fallthrough]];
    //  
    //             default:
    //                 return BaseClass::logMessage(item.pTokenInfo, "diagram", "'org': invalid 'org' type"), false;
    //  
    //         }
    //  
    //         auto itemAddrIt = diagram.createConstMemoryIterator(item); // итератор для вновь добавляемого элемента - итератор ORG'а
    //         auto calculatedFinalAddressIt = diagram.getBaseAddressIterator() + std::int64_t(calculatedStart); // итератор подсчитанного адреса для вновь добавляемого элемента
    //         // auto calculatedFinalAddress = baseAddress+calculatedStart;
    //         // // Проверить, не налез ли он на уже размеченную память
    //         // if (item.orgAddress<calculatedFinalAddress)
    //         if (std::uint64_t(itemAddrIt) < std::uint64_t(calculatedFinalAddressIt))
    //         {
    //             // попадает в уже размеченную память - falls into already mapped memory
    //             // попадает в уже неразмеченную память - ends up in already unallocated memory
    //             // 
    //             BaseClass::logMessage(item.pTokenInfo, "diagram", "'org' address hits into previous layout");
    //             return false;
    //         }
    //  
    //         // Если новый org создаёт gap - добавить fill
    //         if (std::uint64_t(itemAddrIt)>std::uint64_t(calculatedFinalAddressIt) && !diagram.data.empty()) // а данные не пусты, значит, надо заполнить место
    //         {
    //             std::size_t gapSize = std::uint64_t(itemAddrIt) - std::uint64_t(calculatedFinalAddressIt); // от итератора ORG отнимаем итератор подсчитанного адреса для вновь добавляемого элемента
    //  
    //             auto gapItem = PacketDiagramItemType();
    //  
    //             auto addressInfo   = calculatedFinalAddressIt.getAddressInfo();
    //             gapItem.orgAddress = addressInfo.base;
    //             gapItem.orgOffset  = addressInfo.offset;
    //  
    //             gapItem.itemType   = EPacketDiagramItemType::range;
    //             gapItem.fillEntry  = true; // поле для заполнения
    //  
    //             gapItem.addressRange.start = calculatedStart;
    //             gapItem.addressRange.end   = gapItem.addressRange.start + gapSize - 1;
    //  
    //             gapItem.pTokenInfo = item.pTokenInfo;
    //  
    //             gapItem.blockMode  = true;
    //  
    //             // Вроде всё необходимое задано для гэп записи
    //  
    //             gapItem.text = "Fill_" + std::to_string(diagram.fillEntryCounter);
    //             ++diagram.fillEntryCounter;
    //  
    //             diagram.data.emplace_back(gapItem); // Добавляем gap fill
    //         }
    //  
    //         // Установить lastOrg. Задаём абсолютное значение
    //  
    //         diagram.lastOrg       = item.orgAddress;
    //         diagram.lastOrgOffset = item.orgOffset ;
    //     }
    //  
    //     // Вроде всё проверили
    //  
    //     // Теперь фиксим поля
    //  
    //     if (item.itemType==EPacketDiagramItemType::explicitType)
    //     {
    //         auto baseIt = diagram.getBaseAddressIterator();
    //         baseIt += std::int64_t(calculatedStart);
    //         //baseIt += std::int64_t(item.getTypeFieldSize()-1);
    //  
    //         auto addressInfo = baseIt.getAddressInfo();
    //         item.orgAddress  = addressInfo.base;
    //         item.orgOffset   = addressInfo.offset;
    //  
    //         item.addressRange.start = calculatedStart;
    //         item.addressRange.end   = item.addressRange.start + item.getTypeFieldSize()-1;
    //     }
    //  
    //     else if (item.itemType==EPacketDiagramItemType::singleValue)
    //     {
    //         auto baseIt = diagram.getBaseAddressIterator();
    //         baseIt += std::int64_t(calculatedStart);
    //  
    //         auto addressInfo = baseIt.getAddressInfo();
    //         item.orgAddress  = addressInfo.base;
    //         item.orgOffset   = addressInfo.offset;
    //  
    //         item.addressRange.end = item.addressRange.start;
    //     }
    //  
    //     else if (item.itemType==EPacketDiagramItemType::range)
    //     {
    //         // auto rangeSize = item.addressRange.end - item.addressRange.start + 1;
    //     
    //         auto baseIt = diagram.getBaseAddressIterator();
    //         baseIt += std::int64_t(calculatedStart);
    //  
    //         auto addressInfo = baseIt.getAddressInfo();
    //         item.orgAddress  = addressInfo.base;
    //         item.orgOffset   = addressInfo.offset;
    //     }
    //  
    //     else if (item.itemType==EPacketDiagramItemType::org)
    //     {
    //     }
    //  
    //     // Если никаких данных не было, и добавляемый элемент - не org, надо вставить org 0
    //     if (diagram.data.empty() && item.itemType!=EPacketDiagramItemType::org)
    //     {
    //         PacketDiagramItemType orgItem;
    //         orgItem.pTokenInfo    = item.pTokenInfo;
    //         orgItem.itemType      = EPacketDiagramItemType::org;
    //         orgItem.orgType       = EOrgType::orgAbs;
    //         orgItem.text          = diagram.generateOrgName(orgCounter);
    //         orgItem.textGenerated = true;
    //         orgItem.addressRange.start = 0;
    //         orgItem.addressRange.end   = 0;
    //         orgItem.orgAddress         = 0;
    //         orgItem.orgOffset          = 0;
    //  
    //         diagram.data.emplace_back(orgItem);
    //         diagram.orgAddress    = 0;
    //         diagram.orgOffset     = 0;
    //         diagram.lastOrg       = 0;
    //         diagram.lastOrgOffset = 0;
    //     }
    //  
    //     if (item.itemType==EPacketDiagramItemType::org && diagram.orgAddress==std::uint64_t(-1))
    //     {
    //         diagram.orgAddress = item.orgAddress;
    //         diagram.orgOffset  = item.orgOffset ;
    //     }
    //  
    //  
    //     diagram.data.emplace_back(item);
    //  
    //     return true;
    // }


    // static bool isAnyType(umba::tokenizer::payload_type tk)
    // {
    //     switch(tk)
    //     {
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR  : return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8  : return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16 : return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32 : return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64 : return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8 : return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16: return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32: return true;
    //         case MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64: return true;
    //         default: return false;
    //     }
    // }

    // static bool isAnyNumber(umba::tokenizer::payload_type tk)
    // {
    //     switch(tk)
    //     {
    //         case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN: return true;
    //         //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN: return true;
    //         case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC: return true;
    //         //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC: return true;
    //         case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT: return true;
    //         case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX: return true;
    //         //case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX: return true;
    //         case UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT: return true;
    //         default: return false;
    //     }
    // }


    static
    std::string getTokenIdStr(umba::tokenizer::payload_type tk)
    {
        switch(tk)
        {
            case UFSM_TOKEN_KWD_NAMESPACE        : return "namespace";
            case UFSM_TOKEN_KWD_FSM              : return "state-machine";
            case UFSM_TOKEN_KWD_DEFINITIONS      : return "definitions";
            case UFSM_TOKEN_BRACKET_SCOPE_OPEN   : return "scope-open-bracket";
            case UFSM_TOKEN_BRACKET_SCOPE_CLOSE  : return "scope-close-bracket";

            case UMBA_TOKENIZER_TOKEN_IDENTIFIER : return "identifier";
            case UMBA_TOKENIZER_TOKEN_STRING_LITERAL: return "string-literal";

            case UFSM_TOKEN_KWD_EVENTS           : return "events";
            case UFSM_TOKEN_KWD_ACTIONS          : return "actions";
            case UFSM_TOKEN_KWD_STATES           : return "states";
            case UFSM_TOKEN_KWD_PREDICATES       : return "predicates";
            case UFSM_TOKEN_KWD_TRANSITIONS      : return "transitios";
            case UFSM_TOKEN_KWD_ALL              : return "all";

            case UFSM_TOKEN_KWD_USES             : return "uses";
            case UFSM_TOKEN_KWD_INHERITS         : return "inherits";
            case UFSM_TOKEN_KWD_OVERRIDE         : return "override";

            case UFSM_TOKEN_KWD_GENERATED        : return "generated";
            case UFSM_TOKEN_KWD_GENERATES        : return "generates";
            //case          : return "";

            case UFSM_TOKEN_OP_SCOPE             : return "op-scope-resolution";
            case UFSM_TOKEN_OP_COMMA             : return "comma";
            case UFSM_TOKEN_OP_COLON             : return "colon";
            case UFSM_TOKEN_OP_SEMICOLON         : return "semicolon";
            case UFSM_TOKEN_OP_DESCR_FOLLOWS     : return "description-separator";
            case UFSM_TOKEN_OP_ANY               : return "op-any";
            case UFSM_TOKEN_OP_NOT               : return "op-not";
            case UFSM_TOKEN_OP_AND               : return "op-and";
            case UFSM_TOKEN_OP_OR                : return "op-or";
            case UFSM_TOKEN_OP_NOT_ALTER         : return "op-not";
            case UFSM_TOKEN_OP_AND_ALTER         : return "op-and";
            case UFSM_TOKEN_OP_OR_ALTER          : return "op-or";
            case UFSM_TOKEN_OP_ASSIGN            : return "op-assign";

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

            #if 0
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
            case MERMAID_TOKEN_OPERATOR_EXTRA  : return "extra-options-operator";
            
            case UMBA_TOKENIZER_TOKEN_CTRL_FIN: return "EOF";

            case MERMAID_TOKEN_OPERATOR_RANGE: return "range-operator";
            case MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER: return "follow-delimiter";

            // case : return "";
            // case : return "";
            // case : return "";
            // case : return "";
            #endif

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

    bool expectedReachedMsg(const TokenInfoType *pTokenInfo, umba::tokenizer::payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        BaseClass::logUnexpected( pTokenInfo, payloadExpected, msg, [&](umba::tokenizer::payload_type tk) { return getTokenIdStr(tk); } );
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

    bool checkExactTokenType(const TokenInfoType *pTokenInfo, umba::tokenizer::payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        // for(auto e : payloadExpectedList)
        // {
        //     if (e==pTokenInfo->tokenType) // tkReached
        //         return true;
        // }

        if (payloadExpected==pTokenInfo->tokenType)
            return true;

        return expectedReachedMsg(pTokenInfo, payloadExpected, msg);
    }


    template<typename TokenHandler>
    bool readHomogeneousTokensList( umba::tokenizer::payload_type tokenToken, umba::tokenizer::payload_type tokenSep
                                  , bool readNextOnStart
                                  , TokenHandler handler
                                  , bool initialWaitSep = false
                                  )
    {
        if (readNextOnStart)
            readNextToken();

        bool waitSep = initialWaitSep;

        for( 
           ; umba::TheValue(m_pTokenInfo->tokenType).oneOf(tokenSep, tokenToken)
           ; readNextToken()
           )
        {
            if (waitSep)
            {
                if (m_pTokenInfo->tokenType!=tokenSep)
                    break;
                waitSep = false;
            }
            else // ждём tokenToken, после разделителя или в начале
            {
                if (!checkExactTokenType(m_pTokenInfo, tokenToken /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                //evd.basicEvents.emplace_back(extractIdentifierName(m_pTokenInfo));
                handler();
                waitSep = true;
            }
        }

        return true;
    }


    // cmdStopTraffic : external - "The RED light (stop) mode is on";
    // tmStopTraffic : external generated;
    // evIntegralLiteral = evBoolLiteral | evIntLiteral - "Пришел интегральный литерал";

    bool parseStateMachineEvents(StateMachineDefinition & sm)
    {
        // Пропускаем ключевое слово events, вычитываем следующий токен - должна быть открывающая фигурная скобка
        readNextToken();

        EventFlags commonFlags = EventFlags::none;

        if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
        {
            readNextToken();
            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= EventFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (m_pTokenInfo->tokenType==UFSM_TOKEN_BRACKET_SCOPE_CLOSE)
                return true;

            if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            EventDefinition evd;
            evd.positionInfo = getFullPos();
            evd.name  = extractIdentifierName(m_pTokenInfo);
            evd.flags = commonFlags;

            readNextToken();

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_SEMICOLON)
            {
                // Ничего дополнительно не задано, считаем событие как external
                evd.flags |= EventFlags::external;
                sm.addDefinition(evd);
                continue;
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
            {
                readNextToken();
                if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                evd.description = extractLiteral(m_pTokenInfo);
                evd.flags |= EventFlags::external;
                sm.addDefinition(evd);
                readNextToken();
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                    return false;
                continue;
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
            {
                readNextToken();

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_EXTERNAL)
                {
                    evd.flags |= EventFlags::external;
                    readNextToken();
                }

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_GENERATED)
                {
                    evd.flags |= EventFlags::external | EventFlags::generated;
                    readNextToken();
                }

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
                {
                    readNextToken();
                    if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    evd.description = extractLiteral(m_pTokenInfo);
                    readNextToken();
                }

                sm.addDefinition(evd);
            }
            else if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_ASSIGN)
            {
                if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_OR
                                              , true /* readNextOnStart */
                                              , [&]() { evd.basicEvents.emplace_back(extractIdentifierName(m_pTokenInfo)); } ))
                    return false;

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
                {
                    readNextToken();
                    if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    evd.description = extractLiteral(m_pTokenInfo);
                    readNextToken();
                }

                sm.addDefinition(evd);

            }
            else
            {
                return checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON,UFSM_TOKEN_OP_DESCR_FOLLOWS,UFSM_TOKEN_OP_COLON,UFSM_TOKEN_OP_ASSIGN} /* , "'display-options' directive: invalid option value" */ );
            }

            // Тут у нас должна быть точка с запятой
            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

        } // while(true)

    }

    // deferStopTrafficWarning  : external generates tmStopTrafficWarning 
    //     - "Generates deferred tmStopTrafficWarning event"; // starts one shot timer for tmStopTrafficWarning
    // deferStopTraffic         : external generates tmStopTraffic; // starts one shot timer for tmStopTraffic
    // turnOff   = greenOff, redOff - "Turn all lights off"; 
    // onlyGreen = redOff, greenOn;

    bool parseStateMachineActions(StateMachineDefinition &sm)
    {
        // Пропускаем ключевое слово events, вычитываем следующий токен - должна быть открывающая фигурная скобка
        readNextToken();

        ActionFlags commonFlags = ActionFlags::none;

        if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
        {
            readNextToken();
            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= ActionFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (m_pTokenInfo->tokenType==UFSM_TOKEN_BRACKET_SCOPE_CLOSE)
                return true;

            if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            ActionDefinition ad;
            ad.positionInfo = getFullPos();
            ad.name  = extractIdentifierName(m_pTokenInfo);
            ad.flags = commonFlags;

            readNextToken();

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_SEMICOLON)
            {
                // Ничего дополнительно не задано, считаем событие как external
                ad.flags |= ActionFlags::external;
                sm.addDefinition(ad);
                continue;
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
            {
                readNextToken();
                if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                ad.description = extractLiteral(m_pTokenInfo);
                ad.flags |= ActionFlags::external;
                sm.addDefinition(ad);
                readNextToken();
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                    return false;
                continue;
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
            {
                readNextToken();

                ad.flags |= ActionFlags::external;

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_EXTERNAL)
                {
                    readNextToken();
                }

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_GENERATES)
                {
                    ad.flags |= ActionFlags::external | ActionFlags::generates;

                    if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                                  , true /* readNextOnStart */
                                                  , [&]() { ad.generates.emplace_back(extractIdentifierName(m_pTokenInfo)); } ))
                        return false;

                }

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
                {
                    readNextToken();
                    if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    ad.description = extractLiteral(m_pTokenInfo);
                    readNextToken();
                }

                sm.addDefinition(ad);
            }
            else if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_ASSIGN)
            {
                if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                              , true /* readNextOnStart */
                                              , [&]() { ad.basicActions.emplace_back(extractIdentifierName(m_pTokenInfo)); } ))
                    return false;


                if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
                {
                    readNextToken();
                    if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    ad.description = extractLiteral(m_pTokenInfo);
                    readNextToken();
                }

                sm.addDefinition(ad);

            }
            else
            {
                return checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON,UFSM_TOKEN_OP_DESCR_FOLLOWS,UFSM_TOKEN_OP_COLON,UFSM_TOKEN_OP_ASSIGN} /* , "'display-options' directive: invalid option value" */ );
            }

            // Тут у нас должна быть точка с запятой
            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

        } // while(true)

        return false;
    }

    bool parseStateMachineStates(StateMachineDefinition &sm )
    {
        // Пропускаем ключевое слово actions, вычитываем следующий токен - должна быть открывающая фигурная скобка
        readNextToken();

        StateFlags commonFlags = StateFlags::none;

        if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
        {
            readNextToken();
            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= StateFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (m_pTokenInfo->tokenType==UFSM_TOKEN_BRACKET_SCOPE_CLOSE)
                return true;

            if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            StateDefinition sd;
            sd.positionInfo = getFullPos();
            sd.name  = extractIdentifierName(m_pTokenInfo);
            sd.flags = commonFlags;

            readNextToken();

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
            {
                readNextToken(); // вычитываем следующий токен
                // может быть только признак initial/final
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_INITIAL, UFSM_TOKEN_KWD_FINAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что

                sd.flags |= m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_INITIAL ? StateFlags::initial : StateFlags::final;
                readNextToken();
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
            {
                readNextToken();
                if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                sd.description = extractLiteral(m_pTokenInfo);
                readNextToken();
            }

            // Теперь ожидаем блок с различными типами действий: enter/leave/self-enter/self-leave
            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
                
            while(true)
            {
                // Ждём идентификатор типа действия или конец блока
                readNextToken();
                if (m_pTokenInfo->tokenType==UFSM_TOKEN_BRACKET_SCOPE_CLOSE)
                    break;
  
                // Ждём идентификатор типа действия
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_ENTER, UFSM_TOKEN_KWD_LEAVE, UFSM_TOKEN_KWD_SELF_ENTER, UFSM_TOKEN_KWD_SELF_LEAVE} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что

                StateActionKind saKind = StateActionKind::none;

                switch(m_pTokenInfo->tokenType)
                {
                    case UFSM_TOKEN_KWD_ENTER     : saKind = StateActionKind::stateEnter; break;
                    case UFSM_TOKEN_KWD_LEAVE     : saKind = StateActionKind::stateLeave; break;
                    case UFSM_TOKEN_KWD_SELF_ENTER: saKind = StateActionKind::selfEnter ; break;
                    case UFSM_TOKEN_KWD_SELF_LEAVE: saKind = StateActionKind::selfLeave ; break;
                }

                readNextToken();
                // Ждем разделитель перед списком действий
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_COLON} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                
                if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                              , true /* readNextOnStart */
                                              , [&]() { sd.actionRefs[saKind].push_back(extractIdentifierName(m_pTokenInfo)); } ))
                    return false;

                // Тут у нас должна быть точка с запятой
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что

            } // while(true)

            sm.addDefinition(sd);

        } // while(true)

        // return true; // unreachable
    }

    bool readLogicExpression(LogicExpression &expr, bool readNextOnStart=true) // LogicExpression *pExprSmp, 
    {
        using ParserErrorType = typename LogicExpressionParser::ErrorType;

        LogicExpressionParser parser = LogicExpressionParser(makeLogicExpressionOperatorTraits());
        ParserErrorType parserErr = parser.initialize();
        if (parserErr!=ParserErrorType::none)
        {
            //BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "expression-parsering", "parser initialization failed");
            BaseClass::logSimpleMessage(m_tokenPos, m_pTokenInfo->tokenType, "expression-parsering", "parser initialization failed");
            return false;
        }

        using marty::expressions::tokenizer_helpers::convertTokenizerEvent;
        using marty::expressions::tokenizer_helpers::IdentifierConvertTraits;


        if (readNextOnStart)
            readNextToken();
         
        std::size_t tokensCount = 0;
        for( 
           ; umba::TheValue(m_pTokenInfo->tokenType)
             .oneOf( UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN 
                   , UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE
                   , UFSM_TOKEN_OP_NOT, UFSM_TOKEN_OP_NOT_ALTER
                   , UFSM_TOKEN_OP_AND, UFSM_TOKEN_OP_AND_ALTER
                   , UFSM_TOKEN_OP_OR , UFSM_TOKEN_OP_OR_ALTER
                   , UMBA_TOKENIZER_TOKEN_IDENTIFIER
                   )
           ; readNextToken(), ++tokensCount
           )
        {
            LogicExpressionInputItem inputItem;
            if (!convertTokenizerEvent<tokenizer_type>( inputItem, m_pTokenInfo->tokenType
                                      , getFullPos(), BaseClass::getTokenParsedData(m_pTokenInfo)
                                      , false // bCaseIgnore
                                      , true // bBoolConvert
                                      , true // bOpConvert  
                                      , IdentifierConvertTraits<std::string>() // "false"->false, "true"->true, "not"->!, "and"->&, "or"->|
                                      ))
                throw std::runtime_error("readLogicExpression: something goes wrong");

            parserErr = parser.parse(inputItem);
            if (parserErr!= ParserErrorType::none)
            {
                //BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "expression-parsering", parser.getErrorMessage(parserErr));
                BaseClass::logSimpleMessage(m_tokenPos, m_pTokenInfo->tokenType, "expression-parsering", parser.getErrorMessage(parserErr));
                return false;
            }
        }

        if (!tokensCount)
        {
            //BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "expression-parsering", "expression is empty");
            BaseClass::logSimpleMessage(m_tokenPos, m_pTokenInfo->tokenType, "expression-parsering", "expression is empty");
            return false;
        }

        // ParserErrorType 
        parserErr = parser.finalize();
        if (parserErr!= ParserErrorType::none)
        {
            //BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "expression-parsering", parser.getErrorMessage(parserErr));
            BaseClass::logSimpleMessage(m_tokenPos, m_pTokenInfo->tokenType, "expression-parsering", parser.getErrorMessage(parserErr));
            return false;
        }

        expr = parser.getExpression();

        // ExpressionNodeType simplify(const ExpressionNodeType &node) const
        //     LogicExpressionParser
        //     LogicExpressionEvaluator

        return true;
    }


    bool parseStateMachinePredicates(StateMachineDefinition &sm)
    {
        // Пропускаем ключевое слово predicates, вычитываем следующий токен - должна быть открывающая фигурная скобка
        readNextToken();

        PredicateFlags commonFlags = PredicateFlags::none;

        if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
        {
            readNextToken();
            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= PredicateFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        // struct PredicateDefinition
        // {
        //     PositionInfo    positionInfo;
        //     std::string     name        ;
        //     std::string     description ;
        //     PredicateFlags  flags = PredicateFlags::none;
        //     LogicExpression expression  ;
        //     std::vector<std::string>   validForList; 

        // greenLightIsOn  : external valid-for {toggleGreen} - "Checks the GREEN light is ON";
        // yellowLightIsOn : external valid-for {toggleYellow} - "Checks the YELLOW light is ON";
        // redLightIsOn    : external - "Checks the YELLOW light is ON"; // valid-for {toggleYellow}
        // yellowOrGreenLightIsOn = yellowLightIsOn | greenLightIsOn;

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (m_pTokenInfo->tokenType==UFSM_TOKEN_BRACKET_SCOPE_CLOSE)
                return true;

            if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            PredicateDefinition pd;
            pd.positionInfo = getFullPos();
            pd.name  = extractIdentifierName(m_pTokenInfo);
            pd.flags = commonFlags;

            readNextToken();

            bool hasExpression = false;

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_ASSIGN)
            {
                if (!readLogicExpression(pd.expression, true /* readNextOnStart */ ))
                    return false;

                hasExpression = true;
            }
            else if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
            {
                pd.flags |= PredicateFlags::external;

                readNextToken();

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_EXTERNAL)
                {
                    readNextToken();
                }

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_VALID_FOR)
                {
                    pd.flags |= PredicateFlags::validFor;

                    readNextToken();
                    if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что

                    if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                                  , true /* readNextOnStart */
                                                  , [&]() { pd.validForList.push_back(extractIdentifierName(m_pTokenInfo)); } ))
                        return false;

                    if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что

                    // Нужно проверить на пустоту список valid-for

                    if (pd.validForList.empty())
                    {
                        // BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "valid-for", "empty 'valid-for' list");
                        BaseClass::logSimpleMessage(m_tokenPos, m_pTokenInfo->tokenType, "valid-for", "empty 'valid-for' list");
                        return false;
                    }

                    readNextToken(); // скипнули закрывающую скобку
                }
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
            {
                readNextToken();
                if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                pd.description = extractLiteral(m_pTokenInfo);
            }

            sm.addDefinition(pd);
            readNextToken();

            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                return false;

            continue;

        }

        return false;
    }

    bool parseStateMachineTransitions(StateMachineDefinition &  /* sm */ )
    {
        return false;
    }

    bool parseStateMachineBody(StateMachineDefinition & sm)
    {
        // Скобка уже открыта
        while(true)
        {
            readNextToken(); 

            switch(m_pTokenInfo->tokenType)
            {
                case UFSM_TOKEN_KWD_EVENTS     : 
                     if (!parseStateMachineEvents(sm))
                         return false;
                     break;

                case UFSM_TOKEN_KWD_ACTIONS    : 
                     if (!parseStateMachineActions(sm))
                         return false;
                     break;

                case UFSM_TOKEN_KWD_STATES     : 
                     if (!parseStateMachineStates(sm))
                         return false;
                     break;

                case UFSM_TOKEN_KWD_PREDICATES :
                     if (!parseStateMachinePredicates(sm))
                         return false;
                     break;

                case UFSM_TOKEN_KWD_TRANSITIONS:
                     if (!parseStateMachineTransitions(sm))
                         return false;
                     break;

                case UFSM_TOKEN_BRACKET_SCOPE_CLOSE:
                {
                     // выбрали закрывающую скобку стейт-машины со входа
                     // readNextToken(); // или не надо
                     return true;
                }

                default:
                {
                     if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_EVENTS,UFSM_TOKEN_KWD_ACTIONS,UFSM_TOKEN_KWD_STATES,UFSM_TOKEN_KWD_PREDICATES,UFSM_TOKEN_KWD_TRANSITIONS,UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                         return false; // а пришло хз что
                }

            } // switch

            // Выбрали закрывающую скобку блока actions/events/... со входа
            //readNextToken(); 
        }

        //return true;
    }

    //----------------------------------------------------------------------------
    // Останавливается на любом неизвестном токене
    bool parseFullQualifiedName(FullQualifiedName &fqn)
    {
        fqn.clear();

        // if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
        //                               , true /* readNextOnStart */
        //                               , [&]() { ad.basicActions.emplace_back(extractIdentifierName(m_pTokenInfo)); } ))
        //    return false;

        bool waitSep = false;

        if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_SCOPE)
        {
            // В начале у нас оператор скоупа, значит имя абсолютное
            waitSep = true;
            fqn.flags = FullQualifiedNameFlags::absolute;
        }
        else
        {
            fqn.flags = FullQualifiedNameFlags::none;
        }

        if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_SCOPE
                                      , false /* readNextOnStart */
                                      , [&]() { fqn.append(extractIdentifierName(m_pTokenInfo)); } 
                                      , waitSep ))
            return false;

        #if 0
        // В начале цикла - не вычитываем, так как у нас уже идентификатор или разделитель NS
        for( // m_pTokenInfo = BaseClass::waitForSignificantTokenChecked( &m_tokenPos, ParserWaitForTokenFlags::none)
           ; umba::TheValue(m_pTokenInfo->tokenType).oneOf(UFSM_TOKEN_OP_SCOPE, UMBA_TOKENIZER_TOKEN_IDENTIFIER)
           ; m_pTokenInfo = BaseClass::waitForSignificantTokenChecked( &m_tokenPos, ParserWaitForTokenFlags::none)
           )
        {
            if (waitSep)
            {
                if (m_pTokenInfo->tokenType!=UFSM_TOKEN_OP_SCOPE)
                    return true;
                // if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SCOPE} /* , "'display-options' directive: invalid option value" */ ))
                //     return false; // пришло что-то другое - ошибка
                waitSep = false;
            }
            else // ждём имя (идентификатор), после скоуп-разделителя или в начале
            {
                if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                fqn.append(extractIdentifierName(m_pTokenInfo));
                waitSep = true;
            }
        }
        #endif

       // Пришел неизвестный токен, но то, что пришло, было у нужном порядке через '::', а пришедший токен проверяем выше, после возврата
       return true;
    }

    //----------------------------------------------------------------------------
    // Останавливается на любом неизвестном токене
    bool parseInheritanceOverrideFlags(InheritanceOverrideFlags &parsedOverrideFlags)
    {
        bool waitComma = false;

        std::size_t cnt = 0;
        
        for( m_pTokenInfo = BaseClass::waitForSignificantTokenChecked( &m_tokenPos, ParserWaitForTokenFlags::none) // пропустили открывающую скобку
           ; umba::TheValue(m_pTokenInfo->tokenType).oneOf( UFSM_TOKEN_OP_COMMA
                                                          , UFSM_TOKEN_KWD_ACTIONS    
                                                          , UFSM_TOKEN_KWD_EVENTS     
                                                          , UFSM_TOKEN_KWD_PREDICATES 
                                                          , UFSM_TOKEN_KWD_STATES     
                                                          , UFSM_TOKEN_KWD_TRANSITIONS
                                                          , UFSM_TOKEN_KWD_ALL
                                                          )
           ; m_pTokenInfo = BaseClass::waitForSignificantTokenChecked( &m_tokenPos, ParserWaitForTokenFlags::none)
           )
        {
            if (waitComma)
            {
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_COMMA} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // пришло что-то другое - ошибка
                waitComma = false;
            }
            else // ждём флаг (кейворд флага)
            {
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_KWD_EVENTS, UFSM_TOKEN_KWD_ACTIONS, UFSM_TOKEN_KWD_STATES, UFSM_TOKEN_KWD_PREDICATES, UFSM_TOKEN_KWD_TRANSITIONS, UFSM_TOKEN_KWD_ALL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришла запятая
                InheritanceOverrideFlags newFlagVal = InheritanceOverrideFlags::none;
                switch(m_pTokenInfo->tokenType)
                {
                    case UFSM_TOKEN_KWD_ACTIONS    : newFlagVal = InheritanceOverrideFlags::actions    ; break;
                    case UFSM_TOKEN_KWD_EVENTS     : newFlagVal = InheritanceOverrideFlags::events     ; break;
                    case UFSM_TOKEN_KWD_PREDICATES : newFlagVal = InheritanceOverrideFlags::predicates ; break;
                    case UFSM_TOKEN_KWD_STATES     : newFlagVal = InheritanceOverrideFlags::states     ; break;
                    case UFSM_TOKEN_KWD_TRANSITIONS: newFlagVal = InheritanceOverrideFlags::transitions; break;
                    case UFSM_TOKEN_KWD_ALL        : newFlagVal = InheritanceOverrideFlags::all        ; break;
                };

                //!!! Нужно проверить наличие этого флага, если уже установлен, то это ошибка
                // Пока просто устанавливаем без проверки
                parsedOverrideFlags |= newFlagVal;
                waitComma = true;
            }
        }

        if (!cnt)
        {
            // BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "override-list", "empty 'override' list");
            BaseClass::logSimpleMessage(m_tokenPos, m_pTokenInfo->tokenType, "override-list", "empty 'override' list");
            return false;
        }

        // Пришел неизвестный токен, но то, что пришло, было у нужном порядке через запятые, а пришедший токен проверяем выше, после возврата
        return true;
    }

    //----------------------------------------------------------------------------
    bool parseStateMachineInheritanceList(StateMachineDefinition &sm, InheritanceListMode &inheritanceListMode)
    {
        // Пропускаем ':' UFSM_TOKEN_OP_COLON - он был вычитан на уровне выше, но не удалён
        readNextToken(); 
        while(true)
        {
            if (umba::TheValue(m_pTokenInfo->tokenType).oneOf(UFSM_TOKEN_KWD_USES, UFSM_TOKEN_KWD_INHERITS))
            {
                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_USES)
                    inheritanceListMode = InheritanceListMode::uses;
                else
                    inheritanceListMode = InheritanceListMode::inherits;

                readNextToken(); 
            }

            // Тут ждём идентификатор
            // FullQualifiedName fqn;
            // InheritanceOverrideFlags overrideFlags = InheritanceOverrideFlags::none;
            // PositionInfo positionInfo;
            ParentListEntry ple;
            ple.flags = inheritanceListMode==InheritanceListMode::uses ? ParentListEntryFlags::uses : ParentListEntryFlags::inherits;

            if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SCOPE, UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                return false;

            ple.positionInfo = getFullPos(m_tokenPos);

            // У нас - идентификатор или оператор скоупа, парсим полное имя
            if (!parseFullQualifiedName(ple.name))
            {
                // Ошибку вроде вывели в parseFullQualifiedName
                //expectedReachedMsg(m_pTokenInfo, {UFSM_TOKEN_OP_SCOPE, UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , msg */ );
                return false;
            }

            // После имени может идти override, а после override - опционально идёт блок с атрибутами
            if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_OVERRIDE)
            {
                readNextToken(); 

                if (m_pTokenInfo->tokenType!=UFSM_TOKEN_OP_COLON) // override без блока - это all
                {
                    ple.overrideFlags = InheritanceOverrideFlags::all;
                }
                else
                {
                    readNextToken(); 
                    if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз

                    readNextToken(); 
                    if (!parseInheritanceOverrideFlags(ple.overrideFlags))
                        return false;

                    if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                        return false;
                    readNextToken(); 
                }

                // if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                //     return false;
                // if (!parseInheritanceOverrideFlags(ple.overrideFlags))
                //     return false;
                // if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                //     return false;
                // readNextToken(); 

                // Если имя абсолютное - проверяем наличие с корня
                // Если имя относительное, то пытаемся найти относительно текущего NS,
                // если не нашли, то ищем от корня

                // NamespaceDefinition* pCurNs = getCurrentNamespace();
                //  
                // FullQualifiedName foundName;
                // NamespaceEntryKind parentKind = pCurNs->findAnyDefinition( ple.name, ctx.curNsName, &ctx.rootNs, &foundName);
                // switch(parentKind)
                // {
                //     case NamespaceEntryKind::none         : // ple.name not found
                //          BaseClass::logSimpleMessage(ple.positionInfo.pos, UMBA_TOKENIZER_TOKEN_IDENTIFIER /* tokenType */, "definition-not-found", "'definitions' or 'state-machine' not found");
                //          return false;
                //  
                //     case NamespaceEntryKind::nsDefinition : // definitions or state-machine expected, but found namespace 
                //          BaseClass::logSimpleMessage(ple.positionInfo.pos, UMBA_TOKENIZER_TOKEN_IDENTIFIER /* tokenType */, "definition-not-found", "expected 'definitions' or 'state-machine' name, but found namespace");
                //          return false;
                //  
                //     case NamespaceEntryKind::fsmDefinition: // OK
                //          ple.name = foundName; // задаём полное имя
                //          break;
                //  
                //     case NamespaceEntryKind::invalid      : // 
                //          throw std::runtime_error("Something goes wrong");
                // }
                //  
                // sm.parents[ple.name.getCanonicalName()] = ple;
            }
            else
            {
            }

            NamespaceDefinition* pCurNs = getCurrentNamespace();

            FullQualifiedName foundName;
            NamespaceEntryKind parentKind = pCurNs->findAnyDefinition( ple.name, ctx.curNsName, &ctx.rootNs, &foundName);
            switch(parentKind)
            {
                case NamespaceEntryKind::none         : // ple.name not found
                     BaseClass::logSimpleMessage(ple.positionInfo.pos, UMBA_TOKENIZER_TOKEN_IDENTIFIER /* tokenType */, "definition-not-found", "'definitions' or 'state-machine' not found");
                     return false;

                case NamespaceEntryKind::nsDefinition : // definitions or state-machine expected, but found namespace 
                     BaseClass::logSimpleMessage(ple.positionInfo.pos, UMBA_TOKENIZER_TOKEN_IDENTIFIER /* tokenType */, "definition-not-found", "expected 'definitions' or 'state-machine' name, but found namespace");
                     return false;

                case NamespaceEntryKind::fsmDefinition: // OK
                     ple.name = foundName; // задаём полное имя
                     break;

                case NamespaceEntryKind::invalid      : // 
                     throw std::runtime_error("Something goes wrong");
            }

            sm.parents[ple.name.getCanonicalName()] = ple;


            // После имени или списка override может идти запятая, дефис перед литералом описания, или открывающая скобка блока state machide'ы
            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COMMA)
            {
                readNextToken(); 
                continue;
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_BRACKET_SCOPE_OPEN)
            {
                return true;
            }

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
            {
                readNextToken(); 
                if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL, } /* , "'display-options' directive: invalid option value" */ ))
                    return false;

                sm.description = extractLiteral(m_pTokenInfo);
                readNextToken(); 

                // После описания должна идти открывающая скобка
                // if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                //     return false;
            }

            // // Тут должна быть открывающая скобка
            // if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            //     return false;

            return true;
        }

    }

    //----------------------------------------------------------------------------
    // definitions TrafficLightCommands - "Defines commands for Traffic Light"

    // definitions TrafficLightEventsActions
    // // : inherits TrafficLightCommands 
    // - "Defines internal events and actions for Traffic Light automato"

    // state-machine TrafficLightPedestrian : 
    // //inherits TrafficLightBase,
    // uses TrafficLightCommands, 
    //      TrafficLightEventsActions override // возможные коллизии молча переписываются более поздними значениями
    // - "Pedestrian Traffic Light (Red-Green)"

    // state-machine TrafficLightRoad :
    // uses TrafficLightCommands, 
    //      TrafficLightEventsActions override // возможные коллизии молча переписываются более поздними значениями
    // - "Three-lights traffic light (Reg-Yellow-Green)"

    bool parseStateMachine(StateMachineDefinition &sm, InheritanceListMode inheritanceListMode)
    {
        readNextToken();

        if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
            return false;

        sm.name = extractIdentifierName(m_pTokenInfo);

        readNextToken(); 
        if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_COLON)
        {
            if (!parseStateMachineInheritanceList(sm, inheritanceListMode))
                return false;
        }
        // else 
        
        if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_DESCR_FOLLOWS)
        {
            readNextToken(); 
            if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL, } /* , "'display-options' directive: invalid option value" */ ))
                return false;

            sm.description = extractLiteral(m_pTokenInfo);
            readNextToken(); 
        }

        // Тут у нас должна быть открывающая блок скобка
        if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false;

        // Пропускаем открывающую скобку
        // m_pTokenInfo = 
        // readNextToken(); 

        if (!parseStateMachineBody(sm))
            return false;

        getCurrentNamespace()->addDefinition(sm);

        return true;
    }

    bool parseDefinitions()
    {
        StateMachineDefinition sm;
        sm.positionInfo = getFullPos();
        sm.flags        = StateMachineFlags::none;

        if (!parseStateMachine(sm, InheritanceListMode::uses))
            return false;

        return true;
    }

    bool parseStateMachine()
    {
        StateMachineDefinition sm;
        sm.positionInfo = getFullPos();
        sm.flags        = StateMachineFlags::stateMachine;

        if (!parseStateMachine(sm, InheritanceListMode::inherits))
            return false;

        return true;
    }

    bool parseNamespaceBody()
    {
        return parseImpl();
    }

    bool parseNamespace()
    {
        FullQualifiedName nsName;
        bool waitSep = false;

        while(true)
        {
            readNextToken();

            if (!waitSep) // wait for name
            {
                if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                    return false;

                nsName.append(extractIdentifierName(m_pTokenInfo));
                waitSep = true;
                continue;
            }
            else // wait for name sep or open curly
            {
                if (!checkExactTokenType(m_pTokenInfo, {UFSM_TOKEN_OP_SCOPE, UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                    return false;

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_OP_SCOPE) // ::
                {
                    waitSep = false; // переходим к ожиданию слдующего имени NS
                    continue;
                }
                else // {
                {
                    ctx.appendCurNsName(nsName);
                    // Просто создаём namespace
                    getCurrentNamespace(); // PositionInfo is not the same as TokenPosType
                    return parseNamespaceBody();
                }
            }

        }
    }

    bool parseImpl()
    {
        //TokenPosType tokenPos;

        while(true)
        {
            //const TokenInfoType *pTokenInfo
            readNextToken();

            if (m_pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
                continue;

            if (m_pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
                return true; // normal stop

            if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_NAMESPACE)
            {
                ctx.lastNsPos = getFullPos(m_tokenPos);
                if (!parseNamespace())
                    return false;
            }
            else if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_FSM)
            {
                if (!parseStateMachine())
                    return false;
            }
            else if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_DEFINITIONS)
            {
                if (!parseDefinitions())
                    return false;
            }
            else if (m_pTokenInfo->tokenType==UFSM_TOKEN_BRACKET_SCOPE_CLOSE)
            {
                std::size_t nsPopCount = 0;
                if (!ctx.nsOpenLevels.empty())
                {
                    nsPopCount = ctx.nsOpenLevels.back();
                    ctx.nsOpenLevels.pop_back();
                }
                else
                {
                    BaseClass::logSimpleUnexpected( m_pTokenInfo, [&](umba::tokenizer::payload_type tk) { return getTokenIdStr(tk); } );
                    return false;
                }

                if (nsPopCount>ctx.curNsName.size())
                {
                    throw std::runtime_error("Too many closing curly bracket to pop");
                }

                ctx.curNsName.tailRemove(nsPopCount);

            }

            else
            {
                expectedReachedMsg(m_pTokenInfo, { UFSM_TOKEN_KWD_NAMESPACE, UFSM_TOKEN_KWD_FSM, UFSM_TOKEN_KWD_DEFINITIONS } /* , msg */ );
                return false;
            }

            if (!m_pTokenInfo)
                return false;
    
            // // FIN может быть и в конце выражения, если последняя строка не заканчивается переводом строки, это нормально
            // if (m_pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
            //     return true; // normal stop
            //  
            // if (m_pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_LINEFEED) // Разбор выражения закончился не строкой?
            // {
            //     expectedReachedMsg(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_LINEFEED} /* , msg */ );
            //     return false;
            // }
        }

        return false;
    }

    bool parse()
    {
        try
        {
            return parseImpl();
        }
        catch(const std::exception &e) // e
        {
            return BaseClass::logMessage( m_pTokenInfo, "generic-error", e.what() ), false;
        }
        catch(...)
        {
            return BaseClass::logMessage( m_pTokenInfo, "generic-error", "unknown error" ), false;
        }

        //return false;
    }


}; // class Parser

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::
// #include "umba/tokenizer/parsers/ufsm/parser.h"

