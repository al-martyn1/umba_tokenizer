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
        auto pTokenParsedData = BaseClass::getTokenParsedData(pTokenInfo);
        auto identifierData = std::get<typename TokenizerType::IdentifierDataHolder>(*pTokenParsedData);
        return identifierData.pData->value;
    }

    std::string extractLiteral(const TokenInfoType *pTokenInfo)
    {
        auto pTokenParsedData = BaseClass::getTokenParsedData(pTokenInfo);
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

    bool checkExactTokenType(const TokenInfoType *pTokenInfo, std::initializer_list<umba::tokenizer::payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        for(auto e : payloadExpectedList)
        {
            if (e==pTokenInfo->tokenType) // tkReached
                return true;
        }
        return expectedReachedMsg(pTokenInfo, payloadExpectedList, msg);
    }

    // //! Разбираем токен числового литерала. Возвращается следующий токен для анализа и продолжения разбора строки или ноль при ошибке
    // const TokenInfoType* parseNumber(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, std::uint64_t &val, const std::string &msg=std::string())
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //  
    //     // if (!checkNotNul(pTokenInfo))
    //     //     return 0;
    //  
    //     using umba::tokenizer::ParserWaitForTokenFlags;
    //  
    //     if (!isAnyNumber(pTokenInfo->tokenType))
    //         return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, msg), (const TokenInfoType*)0;
    //  
    //     const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
    //     auto numericLiteralData = std::get<typename tokenizer_type::IntegerNumericLiteralDataHolder>(*pParsedData);
    //     if (numericLiteralData.pData->fOverflow)
    //         return BaseClass::logSimpleMessage(pTokenInfo, "integer-overflow", msg.empty() ? "integer overflow" : msg + ": integer overflow"), (const TokenInfoType*)0;
    //  
    //     val = std::uint64_t(numericLiteralData.pData->value);
    //  
    //     return BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    // }

    // //! Разбираем диапазон. Возвращается следующий токен для анализа и продолжения разбора строки или ноль при ошибке
    // const TokenInfoType* parseRange(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, AddressRange &addressRange, EPacketDiagramItemType &itemType, const std::string &msg=std::string())
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //     // Хотя не всегда, иногда мы ожидаем, что у нас диапазон, и переходим сюда без проверки
    //  
    //     auto makeMsg = [&](const std::string &details)
    //     {
    //         return msg.empty() ? details : msg + ": " + details;
    //     };
    //  
    //     if (!checkNotNul(pTokenInfo))
    //         return 0;
    //  
    //     if (!isAnyNumber(pTokenInfo->tokenType))
    //         return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, makeMsg("failed reading range start")), (const TokenInfoType*)0;
    //  
    //  
    //     using umba::tokenizer::ParserWaitForTokenFlags;
    //  
    //     pTokenInfo = parseNumber(tokenPos, pTokenInfo, addressRange.start, makeMsg("start offset value"));
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     // У нас не обязательно будет на входе range, за которым идёт MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER
    //     // Так что выше должны это проверить
    //     if (pTokenInfo->tokenType!=MERMAID_TOKEN_OPERATOR_RANGE)
    //     {
    //         itemType = EPacketDiagramItemType::singleValue;
    //         return pTokenInfo;
    //     }
    //  
    //     // Читаем второе число из диапазона
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (!isAnyNumber(pTokenInfo->tokenType))
    //         return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, makeMsg("failed reading range end")), (const TokenInfoType*)0;
    //  
    //     pTokenInfo = parseNumber(tokenPos, pTokenInfo, addressRange.end, makeMsg("range end value"));
    //     itemType = EPacketDiagramItemType::range;
    //  
    //     return pTokenInfo;
    // }


    // //! Разбор обычной строки. Может возвращать ноль при ошибке, или токен LF/FIN
    // const TokenInfoType* readStringLiteral(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, std::string &str, const std::string &msg=std::string())
    // {
    //     if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL}, msg))
    //         return 0;
    //  
    //     const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
    //     auto stringLiteralData = std::get<typename tokenizer_type::StringLiteralDataHolder>(*pParsedData);
    //     str = stringLiteralData.pData->asString();
    //  
    //     return BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    // }


    // //! Разбираем диапазон, заданный строками. Возвращается следующий токен для анализа и продолжения разбора строки или ноль при ошибке
    // const TokenInfoType* parseNameRange(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, AddressRangeLabels &addressRange, const std::string &msg=std::string())
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //     // Хотя не всегда, иногда мы ожидаем, что у нас диапазон, и переходим сюда без проверки
    //  
    //     auto makeMsg = [&](const std::string &details)
    //     {
    //         return msg.empty() ? details : msg + ": " + details;
    //     };
    //  
    //     if (!checkNotNul(pTokenInfo))
    //         return 0;
    //  
    //     if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_STRING_LITERAL)
    //         return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL}, makeMsg("failed reading range start name")), (const TokenInfoType*)0;
    //  
    //  
    //     using umba::tokenizer::ParserWaitForTokenFlags;
    //  
    //     pTokenInfo = readStringLiteral(tokenPos, pTokenInfo, addressRange.start, makeMsg("range start name"));
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (pTokenInfo->tokenType!=MERMAID_TOKEN_OPERATOR_RANGE)
    //         return expectedReachedMsg(pTokenInfo, {MERMAID_TOKEN_OPERATOR_RANGE}, makeMsg("failed reading range end name")), (const TokenInfoType*)0;
    //  
    //     // Читаем второе число из диапазона
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_STRING_LITERAL)
    //         return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL}, makeMsg("failed reading range end name")), (const TokenInfoType*)0;
    //  
    //     pTokenInfo = readStringLiteral(tokenPos, pTokenInfo, addressRange.end, makeMsg("range end name"));
    //  
    //     return pTokenInfo;
    // }



    // //! Разбираем тип. Возвращается следующий токен для анализа и продолжения разбора строки или ноль при ошибке
    // const TokenInfoType* parseType(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, PacketDiagramItemType &item)
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //  
    //     using umba::tokenizer::ParserWaitForTokenFlags;
    //  
    //     item.itemType           = EPacketDiagramItemType::explicitType;
    //     item.explicitTypeTokenId = pTokenInfo->tokenType;
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (pTokenInfo->tokenType!=UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN)
    //         return pTokenInfo; // Не массив
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "invalid array size"))
    //         return 0;
    //  
    //     pTokenInfo = parseNumber(tokenPos, pTokenInfo, item.arraySize, "array size");
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE}, "invalid array size"))
    //         return 0;
    //  
    //     return BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    // }

    // //! Разбор директивы packet-beta. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    // const TokenInfoType* parsePacketBetaDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (pTokenInfo->tokenType!=MERMAID_TOKEN_OPERATOR_EXTRA)
    //         return pTokenInfo; // Что-то пришло, но не расширенные опции
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     // В данном контексте MERMAID_TOKEN_ATTR_BYTE_DIA==MERMAID_TOKEN_ATTR_BYTE
    //     // и MERMAID_TOKEN_ATTR_BIT_DIA==MERMAID_TOKEN_ATTR_BIT
    //  
    //     if (!checkExactTokenType(pTokenInfo, {MERMAID_TOKEN_ATTR_BYTE_DIA, MERMAID_TOKEN_ATTR_BIT_DIA, MERMAID_TOKEN_ATTR_MEMORY_DIA, MERMAID_TOKEN_ATTR_BYTE, MERMAID_TOKEN_ATTR_BIT, UMBA_TOKENIZER_TOKEN_LINEFEED}, "invalid 'packet-beta' directive options"))
    //         return 0;
    //  
    //     if ((diagram.parsingOptions&PacketDiagramParsingOptions::allowOverrideType)!=0u)
    //     {
    //         switch(pTokenInfo->tokenType)
    //         {
    //             case MERMAID_TOKEN_ATTR_BYTE      : [[fallthrough]]; // error C2416: attribute 'fallthrough' cannot be applied in this context
    //             case MERMAID_TOKEN_ATTR_BYTE_DIA  : diagram.diagramType = EPacketDiagramType::byteDiagram; break;
    //             case MERMAID_TOKEN_ATTR_BIT       : [[fallthrough]]; // error C2416: attribute 'fallthrough' cannot be applied in this context
    //             case MERMAID_TOKEN_ATTR_BIT_DIA   : diagram.diagramType = EPacketDiagramType::bitDiagram ; break;
    //  
    //             case MERMAID_TOKEN_ATTR_MEMORY_DIA: diagram.diagramType = EPacketDiagramType::memDiagram ; break;
    //             case UMBA_TOKENIZER_TOKEN_LINEFEED: return pTokenInfo;
    //         }
    //     }
    //  
    //     return BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    // }

    // //! Разбор директивы display-width. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    // const TokenInfoType* parseDisplayWidthDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "invalid 'display-width' directive: missing width value"))
    //         return 0;
    //  
    //     std::uint64_t displayWidth = 0;
    //     pTokenInfo = parseNumber(tokenPos, pTokenInfo, displayWidth, "'display-width' directive");
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if ((diagram.parsingOptions&PacketDiagramParsingOptions::allowOverrideDisplayWidth)!=0u)
    //     {
    //         diagram.displayWidth = displayWidth;
    //     }
    //  
    //     return pTokenInfo;
    // }

    // //! Разбор директивы display-options. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    // const TokenInfoType* parseDisplayOptionsDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //  
    //     std::size_t optCnt = 0;
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     while(pTokenInfo)
    //     {
    //         if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER, UMBA_TOKENIZER_TOKEN_LINEFEED}, "'display-options' directive: invalid option value"))
    //             return 0;
    //  
    //         if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
    //         {
    //             if (!optCnt)
    //                 return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER}, "'display-options' directive: use 'none' for empty options list"), (const TokenInfoType*)0;
    //             return pTokenInfo;
    //         }
    //  
    //         // Тут у нас только UMBA_TOKENIZER_TOKEN_IDENTIFIER
    //         ++optCnt;
    //  
    //         auto pTokenParsedData = BaseClass::getTokenParsedData(pTokenInfo);
    //  
    //         // Тут обрабатываем данные идентификатора
    //         // UMBA_USED(pTokenParsedData);
    //  
    //         auto identifierData = std::get<typename TokenizerType::IdentifierDataHolder>(*pTokenParsedData);
    //         string_type identifierStr = string_type(identifierData.pData->value);
    //         // UMBA_USED(identifierData);
    //         // UMBA_USED(identifierStr);
    //  
    //         auto optVal = enum_deserialize(identifierStr, PacketDiagramDisplayOptions::invalid);
    //         if (optVal==PacketDiagramDisplayOptions::invalid)
    //             return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER}, "'display-options' directive: unknown option"), (const TokenInfoType*)0;
    //  
    //         diagram.setOptionFlag(optVal);
    //  
    //         pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //  
    //     }
    //  
    //     return pTokenInfo;
    // }

    // //! Разбор директивы org. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    // const TokenInfoType* parseOrgDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, PacketDiagramItemType &item) 
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //  
    //     // %%#! org + 0x20 : "name" - смещение от предыдущего org
    //     // %%#! org 0x800200 : "name" - абсолютный org
    //     // %%#! org auto : "Some Entry" - автоматическое вычисление адреса
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     bool hasPlus = false;
    //  
    //     if (pTokenInfo->tokenType==MERMAID_TOKEN_OPERATOR_PLUS)
    //     {
    //         hasPlus = true;
    //         pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //         if (!pTokenInfo)
    //             return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //     }
    //  
    //     if (isAnyNumber(pTokenInfo->tokenType))
    //     {
    //         pTokenInfo = parseNumber(tokenPos, pTokenInfo, item.orgAddress, "'org' directive");
    //         item.orgType = hasPlus ? EOrgType::orgRel : EOrgType::orgAbs;
    //         if (item.orgType==EOrgType::orgAbs && diagram.memoryModel==EMemoryModel::segmented)
    //         {
    //             //pTokenInfo = BaseClass::getToken(&tokenPos);
    //             if (!pTokenInfo)
    //                 return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //             if (pTokenInfo->tokenType!=MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER)
    //                 return expectedReachedMsg(pTokenInfo, {MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER}, "'org' directive: reading segment address"), (const TokenInfoType*)0;
    //                 //return BaseClass::logSimpleMessage(pTokenInfo, "unexpected-token", "'org' directive: unexpected token while reading segment address"), (const TokenInfoType*)0;
    //  
    //             pTokenInfo = BaseClass::getToken(&tokenPos);
    //             if (!pTokenInfo)
    //                 return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //             if (!isAnyNumber(pTokenInfo->tokenType))
    //                 return expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC}, "'org' directive: reading segment address"), (const TokenInfoType*)0;
    //                 //return BaseClass::logSimpleMessage(pTokenInfo, "unexpected-token", "'org' directive: unexpected token while reading segment address"), (const TokenInfoType*)0;
    //  
    //             pTokenInfo = parseNumber(tokenPos, pTokenInfo, item.orgOffset, "'org' directive");
    //  
    // // bool expectedReachedMsg(const TokenInfoType *pTokenInfo, std::initializer_list<umba::tokenizer::payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    // // {
    // //     BaseClass::logUnexpected( pTokenInfo, payloadExpectedList, msg, [&](umba::tokenizer::payload_type tk) { return getTokenIdStr(tk); } );
    //  
    //         }
    //     }
    //     else if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_AUTO)
    //     {
    //         if (hasPlus)
    //             return BaseClass::logSimpleMessage(pTokenInfo, "unexpected-auto", "'org' directive: unexpected 'auto' after relative address option ('+')"), (const TokenInfoType*)0;
    //  
    //         item.orgType = EOrgType::orgAuto;
    //  
    //         pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     }
    //  
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     item.addressRange.start = 0;
    //     item.addressRange.end   = 0;
    //     item.itemType           = EPacketDiagramItemType::org;
    //  
    //  
    //     if (pTokenInfo->tokenType!=MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER) // В отличие от регулярной записи, тут название опционально, нагенерим, если надо будет
    //         return pTokenInfo;
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     return readStringLiteral(tokenPos, pTokenInfo, item.text, "'org' directive");
    // }

    // //! Разбор директивы title. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    // const TokenInfoType* parseTitleDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    // {
    //     // На старте нет нужды проверять тип токена, сюда мы попадаем только по подходящей ветке
    //  
    //     //auto pTextStart = BaseClass::getTextPointer(pTokenInfo);
    //  
    //     BaseClass::setRawModeAutoStop(umba::tokenizer::TokenizerRawAutoStopMode::stopOnCharExcluding, string_type("\r\n"));
    //     BaseClass::setRawMode(true);
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //     if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_RAW_DATA}, "invalid 'title' directive"))
    //         return 0;
    //  
    //  
    //     //allowOverrideType
    //  
    //     if ((diagram.parsingOptions&PacketDiagramParsingOptions::allowOverrideTitle)!=0u)
    //     {
    //         const token_parsed_data_type* pParsedData = BaseClass::getTokenParsedData(pTokenInfo);
    //         auto rawData = std::get<typename tokenizer_type::RawDataHolder>(*pParsedData);
    //         diagram.title = umba::string::trim_copy(rawData.pData->asString());
    //     }
    //  
    //     return BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    // }

    // const TokenInfoType* parseBitSize(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, std::uint64_t &bitSize, const std::string &msg=std::string())
    // {
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     pTokenInfo = parseNumber(tokenPos, pTokenInfo, bitSize, msg);
    //     if (!pTokenInfo)
    //         return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //     if (!umba::TheValue(pTokenInfo->tokenType).oneOf(MERMAID_TOKEN_ATTR_BYTE, MERMAID_TOKEN_ATTR_BIT))
    //     {
    //         BaseClass::logUnexpected( pTokenInfo, {MERMAID_TOKEN_ATTR_BYTE, MERMAID_TOKEN_ATTR_BIT}, msg
    //                                 , [&](umba::tokenizer::payload_type tk)
    //                                   {   
    //                                       if (tk==MERMAID_TOKEN_ATTR_BYTE)
    //                                           return std::string("bytes");
    //                                       else if (tk==MERMAID_TOKEN_ATTR_BIT)
    //                                           return std::string("bits");
    //                                       else 
    //                                           return getTokenIdStr(tk);
    //                                   }
    //                                 );
    //         return 0;
    //     }
    //  
    //     auto savedBitSize = bitSize;
    //     if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_BYTE)
    //         bitSize *= 8;
    //     if (savedBitSize>bitSize)
    //         return BaseClass::logSimpleMessage(pTokenInfo, "integer-overflow", msg.empty() ? "integer overflow" : msg + ": integer overflow"), (const TokenInfoType*)0;
    //  
    //     return BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    // }


    // , {"segment"                    , MERMAID_TOKEN_ATTR_SEGMENT               }
    // , {"segment-shift"              , MERMAID_TOKEN_ATTR_SEGMENT_SHIFT         }
    // , {"offset"                     , MERMAID_TOKEN_ATTR_OFFSET                }
    // , {"data"                       , MERMAID_TOKEN_ATTR_DATA                  }

    // // native le 32 bits bit byte bytes
    // //! Разбор директивы native. Разбор полного выражения. Может возвращать ноль при ошибке, или токен LF/FIN
    // const TokenInfoType* parseNativeDirective(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo)
    // {
    //     //bool endiannessTaken    = false;
    //     bool dataSizeTaken      = false;
    //     bool segmentSizeTaken   = false;
    //     bool offsetSizeTaken    = false;
    //     bool segmentShiftTaken  = false;
    //  
    //     //bool sizeTaken       = false;
    //     //const TokenInfoType* pTokenInfoBitSize = 0;
    //     const TokenInfoType* pTokenInfoLastSegmentOption = 0;
    //  
    //     // std::uint64_t       bitSize = 0;
    //     Endianness          endianness = Endianness::undefined; // littleEndian;
    //     // EPacketDiagramType  bitSizeSizeType = EPacketDiagramType::unknown; // Используем не по назначению, а для хринения типа биты или байты в числе (bitDiagram, byteDiagram)
    //  
    //     pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //  
    //     auto optionsCounter = -1;
    //  
    //     //------------------------------
    //     auto returnCheckUpdateOptions = [&]() -> const TokenInfoType*
    //     {
    //         if (optionsCounter<1)
    //             return BaseClass::logMessage(pTokenInfo, "native-options", "there is no options taken in the 'native' directive"), (const TokenInfoType*)0;
    //  
    //         if (pTokenInfoLastSegmentOption)
    //         {
    //             if (!segmentSizeTaken || !segmentShiftTaken || !offsetSizeTaken)
    //                 return BaseClass::logMessage(pTokenInfo, "native-options", "not all segmented mode definitions taken ('segment', 'segment-shift', 'offset')"), (const TokenInfoType*)0;
    //  
    //             diagram.memoryModel = EMemoryModel::segmented;
    //         }
    //  
    //         #if 0
    //         if (pTokenInfoBitSize)
    //         {
    //             if (bitSizeSizeType==EPacketDiagramType::unknown)
    //             {
    //                 // детектим бит/байт размер, 16/32/64/128/256 - размер в битах
    //                 //                           1/2/4    - размер в байтах
    //                 //                           8        - неоднозначно, 8 бит или 8 байт (64 бита?)
    //                 //                           16 и 32 в качестве размера в байтах никто уже не задаст в здравом уме - будут использовать битность 128/256
    //                 if (!umba::TheValue(bitSize).oneOf(1u,2u,4u,8u,16u,32u,64u,128u,256u))
    //                     return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size must be 1/2/4/8 bytes ot 8/16/32/64/128/256 bits"), (const TokenInfoType*)0;
    //  
    //                 if (bitSize==8)
    //                     return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size 8 bits or bytes?"), (const TokenInfoType*)0;
    //  
    //                 if (!umba::TheValue(bitSize).oneOf(1u,2u,4u))
    //                     bitSize *= 8; // Из байт в биты
    //             }
    //             else if (bitSizeSizeType==EPacketDiagramType::byteDiagram)
    //             {
    //                 if (!umba::TheValue(bitSize).oneOf(1u,2u,4u,8u))
    //                     return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size must be 1/2/4/8 bytes ot 8/16/32/64/128/256 bits"), (const TokenInfoType*)0;
    //                 bitSize *= 8; // Из байт в биты
    //             }
    //             else // bits
    //             {
    //                 if (!umba::TheValue(bitSize).oneOf(8u,16u,32u,64u,128u,256u))
    //                     return BaseClass::logMessage(pTokenInfoBitSize, "native-options", "'native' directive: bit size must be 1/2/4/8 bytes ot 8/16/32/64/128/256 bits"), (const TokenInfoType*)0;
    //                 // Только проверка, перевода нет, и так биты
    //             }
    //  
    //             if ((diagram.parsingOptions&DiagramParsingOptions::allowOverrideBitSize)!=0u)
    //             {
    //                 diagram.bitSize = bitSize;
    //             }
    //         }
    //         #endif
    //  
    //         if (endianness!=Endianness::undefined)
    //         {
    //             if ((diagram.parsingOptions&PacketDiagramParsingOptions::allowOverrideEndianness)!=0u)
    //             {
    //                 diagram.endianness = endianness;
    //             }
    //         }
    //  
    //         return pTokenInfo;
    //     };
    //  
    //     //------------------------------
    //     auto updateEndianness = [&](Endianness e)
    //     {
    //         if (endianness!=Endianness::undefined)
    //             return BaseClass::logMessage(pTokenInfo, "native-options", "endianness already taken"), false;
    //         endianness = e;
    //         return true;
    //     };
    //  
    //     //------------------------------
    //     while(true)
    //     {
    //         if (!pTokenInfo)
    //             return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //         ++optionsCounter;
    //  
    //         if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
    //         {
    //             return returnCheckUpdateOptions();
    //         }
    //  
    //         if (!umba::TheValue(pTokenInfo->tokenType).oneOf( MERMAID_TOKEN_ATTR_LE
    //                                                         , MERMAID_TOKEN_ATTR_BE
    //                                                         , MERMAID_TOKEN_ATTR_DATA
    //                                                         , MERMAID_TOKEN_ATTR_SEGMENT
    //                                                         , MERMAID_TOKEN_ATTR_SEGMENT_SHIFT
    //                                                         , MERMAID_TOKEN_ATTR_OFFSET
    //                                                         , UMBA_TOKENIZER_TOKEN_LINEFEED
    //                                                         )
    //            )
    //         {
    //             expectedReachedMsg(pTokenInfo, {UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, MERMAID_TOKEN_ATTR_LE /* , MERMAID_TOKEN_ATTR_BE */  | UMBA_TOKENIZER_TOKEN_LINEFEED }, "invalid 'native' directive options" );
    //             return 0;
    //         }
    //  
    //         if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_LINEFEED)
    //         {
    //             return returnCheckUpdateOptions();
    //         }
    //  
    //         if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_LE)
    //         {
    //             if (!updateEndianness(Endianness::littleEndian))
    //                 return 0;
    //             pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //         }
    //  
    //         else if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_BE)
    //         {
    //             if (!updateEndianness(Endianness::bigEndian))
    //                 return 0;
    //             pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //         }
    //  
    //         else if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_DATA)
    //         {
    //             if (dataSizeTaken)
    //                 return BaseClass::logMessage(pTokenInfo, "native-options", "data size ('data' option) already taken"), (const TokenInfoType*)0;
    //             dataSizeTaken = true;
    //             pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //             pTokenInfo = parseBitSize(tokenPos, pTokenInfo, diagram.dataBitSize, "'native' directive 'data' option");
    //         }
    //  
    //         else if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_SEGMENT)
    //         {
    //             if (segmentSizeTaken)
    //                 return BaseClass::logMessage(pTokenInfo, "native-options", "segment size ('segment' option) already taken"), (const TokenInfoType*)0;
    //             segmentSizeTaken = true;
    //             pTokenInfoLastSegmentOption = pTokenInfo;
    //             pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //             pTokenInfo = parseBitSize(tokenPos, pTokenInfo, diagram.segmentBitSize, "'native' directive 'segment' option");
    //         }
    //  
    //         else if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_SEGMENT_SHIFT)
    //         {
    //             if (segmentShiftTaken)
    //                 return BaseClass::logMessage(pTokenInfo, "native-options", "segment shift ('segment-shift' option) already taken"), (const TokenInfoType*)0;
    //             segmentShiftTaken = true;
    //             pTokenInfoLastSegmentOption = pTokenInfo;
    //             pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //             pTokenInfo = parseBitSize(tokenPos, pTokenInfo, diagram.segmentShift, "'native' directive 'segment-shift' option");
    //         }
    //  
    //         else if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_OFFSET)
    //         {
    //             if (offsetSizeTaken)
    //                 return BaseClass::logMessage(pTokenInfo, "native-options", "offset size ('offset' option) already taken"), (const TokenInfoType*)0;
    //             offsetSizeTaken = true;
    //             pTokenInfoLastSegmentOption = pTokenInfo;
    //             pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //             pTokenInfo = parseBitSize(tokenPos, pTokenInfo, diagram.offsetBitSize, "'native' directive 'offset' option");
    //         }
    //  
    //         #if 0
    //         else // Остались только числа
    //         {
    //             if (pTokenInfoBitSize)
    //                 return BaseClass::logMessage(pTokenInfo, "native-options", "bit size already taken"), (const TokenInfoType*)0;
    //  
    //             pTokenInfoBitSize = pTokenInfo;
    //  
    //             pTokenInfo = parseNumber(tokenPos, pTokenInfo, bitSize, "'native' directive");
    //             if (!pTokenInfo)
    //                 return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //             pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //             if (!pTokenInfo)
    //                 return 0; // Сообщение уже выведено, просто возвращаем ошибку
    //  
    //             if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_BYTE || pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_BIT )
    //             {
    //                 bitSizeSizeType = (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_BYTE) ? EPacketDiagramType::byteDiagram : EPacketDiagramType::bitDiagram;
    //                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
    //             }
    //         }
    //         #endif
    //     }
    //  
    // }

//     //! Разбор обычной строки. Может возвращать ноль при ошибке, или токен LF/FIN
//     const TokenInfoType* parseRegularLine(TokenPosType &tokenPos, const TokenInfoType *pTokenInfo, PacketDiagramItemType &item, ChecksumOptions &checksumOptions)
//     {
//         if (!pTokenInfo)
//             return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//         bool bRange = false;
//         bool forceTypedRange = false;
//  
//         if (isAnyNumber(pTokenInfo->tokenType))
//         {
//             pTokenInfo = parseRange(tokenPos, pTokenInfo, item.addressRange, item.itemType);
//             bRange = true;
//         }
//  
//         else if (isAnyType(pTokenInfo->tokenType))
//         {
//             pTokenInfo = parseType(tokenPos, pTokenInfo, item);
//         }
//  
//         else
//             return 0;
//  
//  
//         if (bRange && diagram.testDisplayOption(PacketDiagramDisplayOptions::rangeAsChars))
//         {
//             item.charsRange = true;
//         }
//  
//  
//         if (!pTokenInfo)
//             return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//         if (!checkExactTokenType(pTokenInfo, {MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER}, "invalid record definition"))
//             return 0;
//  
//         // Читаем следующий токен
//         pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//         if (!checkNotNul(pTokenInfo))
//             return 0;
//  
//         if (!checkExactTokenType(pTokenInfo, {UMBA_TOKENIZER_TOKEN_STRING_LITERAL}, "invalid record definition"))
//             return 0;
//  
//         pTokenInfo = readStringLiteral(tokenPos, pTokenInfo, item.text, "invalid record definition");
//         if (!pTokenInfo)
//             return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//         if (pTokenInfo->tokenType!=MERMAID_TOKEN_OPERATOR_EXTRA)
//             return pTokenInfo;
//  
//         pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//  
//         if (!pTokenInfo)
//             return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//         Endianness &endianness = item.endianness; // = Endianness::unknown;
//         bool hasChecksum   = false; // было ли ключевое слово checksum
//         // bool hasCsKind     = false; // определим по значению поля kind
//         //bool hasCrc        = false; // потом заменим 
//         bool hasSeed       = false;
//         bool hasPoly       = false;
//  
//         checksumOptions.crcConfig.seed = 0;
//         endianness = Endianness::undefined;
//  
//         umba::tokenizer::payload_type itemValueType = MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8;
//  
//         if ((item.isArray() || item.isRange()) && item.getTypeSize()==1)
//         {
//             // у нас однобайтный массив или рендж, надо проверить тип, не отображать ли его блоком по типу
//             if (item.isRange())
//             {
//                 if (item.charsRange)
//                     itemValueType = MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR;
//                 else
//                     itemValueType = MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8;
//             }
//             else // массив заданного явно типа
//             {
//                 itemValueType = item.explicitTypeTokenId;
//             }
//  
//             if ( (diagram.testDisplayOption(PacketDiagramDisplayOptions::uintBytesAsBlock) && itemValueType==MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8) 
//               || (diagram.testDisplayOption(PacketDiagramDisplayOptions::intBytesAsBlock)  && itemValueType==MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8 )
//               || (diagram.testDisplayOption(PacketDiagramDisplayOptions::charBytesAsBlock) && itemValueType==MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR )
//                )
//             {
//                 item.blockMode = true;
//             }
//         }
//  
//         auto returnCheckUpdateOptions = [&]() -> const TokenInfoType*
//         {
//             auto tfs = item.getTypeFieldSize();
//  
//             if (checksumOptions.kind==ChecksumKind::crc)
//             {
//                 // seed по умолчанию 0
//                 // if (!hasSeed)
//                 //     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: 'crc' option taken, but no 'seed' option taken" ), (const TokenInfoType*)0;
//                 if (!hasPoly)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: 'crc' option taken, but no 'poly' option taken" ), (const TokenInfoType*)0;
//  
//                 if (tfs!=1 && tfs!=2 && tfs!=4)
//                 {
//                     BaseClass::logMessage( item.pTokenInfo, "r-definition", "record definition: 'crc' option taken, but size of field is invalid ($(SizeOfField)). Size of field can be only 1, 2 or 4"
//                                          , umba::FormatMessage<std::string>().arg("SizeOfField", tfs).values()
//                                          );
//                     return 0;
//                 }
//             }
//  
//             if (umba::TheValue(checksumOptions.kind).oneOf(ChecksumKind::simpleSum, ChecksumKind::simpleSumComplement, ChecksumKind::simpleSumInvert, ChecksumKind::simpleXor, ChecksumKind::simpleXorComplement, ChecksumKind::simpleXorInvert))
//             {
//                 if (tfs!=1)
//                 {
//                     BaseClass::logMessage( item.pTokenInfo, "r-definition", "record definition: 'simple-*' checksum option taken, but size of field is invalid ($(SizeOfField)). Size of field can be only 1"
//                                          , umba::FormatMessage<std::string>().arg("SizeOfField", tfs).values()
//                                          );
//                     return 0;
//                 }
//  
//                 // item.crcIndex = 0; // сигналим, что CRC опции валидны. Это поле потом следует установить в реальный индекс CRC опций.
//             }
//  
//             if (item.isAsciiZet())
//             {
//                 if (item.getTypeSize()!=1 || !(item.isArray() || item.isRange()))
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: 'ascii-z' option can be set only for char/int8/uint8 arrays/ranges" ), (const TokenInfoType*)0;
//             }
//  
//             if (item.endianness!=Endianness::undefined)
//             {
//                 if (item.getTypeSize()==1)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: endianness option can't be set for char/int8/uint8 values/arrays/ranges" ), (const TokenInfoType*)0;
//             }
//  
//             // item.endianness = Endianness::littleEndian
//  
//             return pTokenInfo;
//         };
//  
// // crc, 
// // struct ChecksumOptions
// // {
// //     ChecksumKind    kind = ChecksumKind::undefined;
// //     // std::size_t          cellIndex  = std::size_t(-1); // Индекс в массиве data, по которому расположена информация о ячейке
// //     CrcConfig       crcConfig; // Размер crc (size) высчитывается от размера элемента, в который помещается, если размер не 1/2/4 - это ошибка
// //     AddressRange    addressRange  ; // Индексы начального и конечного байт данных, сам CRC не должен туда входить
// //  
// // }; // struct ChecksumOptions
//  
//  
//  
//         // %%#! le be middle-endian le-me be-me crc 0-10 seed 10 poly 0x1234
//  
//         while(true)
//         {
//             if (!pTokenInfo)
//                 return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//             if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)
//             {
//                 return returnCheckUpdateOptions();
//             }
//  
//             item.pTokenInfo = pTokenInfo;
//  
//             const auto tokenTypeVal = UMBA_THE_VALUE(pTokenInfo->tokenType); // umba::TheValue<std::decay< decltype(pTokenInfo->tokenType)> >(pTokenInfo->tokenType); // UMBA_THE_VALUE(pTokenInfo->tokenType);
//  
//             if ( !isAnyNumber(pTokenInfo->tokenType)
//               && !tokenTypeVal.oneOf( MERMAID_TOKEN_ATTR_LE, MERMAID_TOKEN_ATTR_BE
//                                     , MERMAID_TOKEN_ATTR_ME, MERMAID_TOKEN_ATTR_LE_ME, MERMAID_TOKEN_ATTR_BE_ME
//                                     , UMBA_TOKENIZER_TOKEN_LINEFEED
//                                     , MERMAID_TOKEN_ATTR_ASCII_Z
//                                     , MERMAID_TOKEN_ATTR_CRC, MERMAID_TOKEN_ATTR_SEED, MERMAID_TOKEN_ATTR_POLY
//                                     , MERMAID_TOKEN_ATTR_CHECKSUM
//                                     , MERMAID_TOKEN_ATTR_SIMPLE_SUM, MERMAID_TOKEN_ATTR_SIMPLE_SUM_COMPLEMENT, MERMAID_TOKEN_ATTR_SIMPLE_SUM_INVERT
//                                     , MERMAID_TOKEN_ATTR_SIMPLE_XOR, MERMAID_TOKEN_ATTR_SIMPLE_XOR_COMPLEMENT, MERMAID_TOKEN_ATTR_SIMPLE_XOR_INVERT
//                                     , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR, MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8
//                                     , MERMAID_TOKEN_ATTR_BLOCK
//                                     )
//                )
//             {
//                 expectedReachedMsg(pTokenInfo, { /* UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, */  MERMAID_TOKEN_ATTR_LE /* , MERMAID_TOKEN_ATTR_BE */  | UMBA_TOKENIZER_TOKEN_LINEFEED } /*, "invalid 'native' directive options" */ );
//                 return 0;
//             }
//  
//             if (tokenTypeVal.oneOf(UMBA_TOKENIZER_TOKEN_LINEFEED))
//             {
//                 return returnCheckUpdateOptions();
//             }
//  
//             if (tokenTypeVal.oneOf(MERMAID_TOKEN_ATTR_BLOCK))
//             {
//                 if (item.getTypeSize()!=1 || !(item.isArray() || item.isRange()))
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: 'block' option can be set only for char/int8/uint8 arrays/ranges" ), (const TokenInfoType*)0;
//                 item.blockMode = true;
//                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//                 continue;
//             }
//  
//             if (tokenTypeVal.oneOf(MERMAID_TOKEN_ATTR_ASCII_Z))
//             {
//                 item.asciiZet = true;
//                 item.charsRange = true; // asciiZet автоматом задаёт тип ренджа
//                 forceTypedRange = true;
//                 // Берем следующий токен и пилим по циклу дальше
//                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//                 continue;
//             }
//  
//             if (tokenTypeVal.oneOf(MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR, MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8))
//             {
//                 if (!bRange)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: trying to set range type for non-range entry" ), (const TokenInfoType*)0;
//  
//                 if (forceTypedRange)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: range type already was set (by type name or by 'ascii-z' option)" ), (const TokenInfoType*)0;
//             
//                 forceTypedRange = true;
//  
//                 if (tokenTypeVal.oneOf(MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR))
//                     item.charsRange = true;
//                 else
//                     item.charsRange = false;
//  
//                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//                 continue;
//             }
//  
//  
//             if (tokenTypeVal.oneOf(MERMAID_TOKEN_ATTR_LE, MERMAID_TOKEN_ATTR_BE, MERMAID_TOKEN_ATTR_ME, MERMAID_TOKEN_ATTR_LE_ME, MERMAID_TOKEN_ATTR_BE_ME))
//             {
//                 if (endianness!=Endianness::unknown)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: endianness option already taken" ), (const TokenInfoType*)0;
//  
//                 if (tokenTypeVal.oneOf(MERMAID_TOKEN_ATTR_ME))
//                 {
//                     if (diagram.endianness==Endianness::unknown)
//                         return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: middle-endian option taken, but project endianness not set" ), (const TokenInfoType*)0;
//  
//                     else if (diagram.endianness==Endianness::littleEndian)
//                         item.endianness = Endianness::leMiddleEndian;
//  
//                     else if (diagram.endianness==Endianness::bigEndian)
//                         item.endianness = Endianness::beMiddleEndian;
//  
//                     else
//                         return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: unknown byte order in project definition" ), (const TokenInfoType*)0;
//                 }
//                 else
//                 {
//                     switch(pTokenInfo->tokenType)
//                     {
//                         case MERMAID_TOKEN_ATTR_LE    : item.endianness = Endianness::littleEndian  ; break;
//                         case MERMAID_TOKEN_ATTR_BE    : item.endianness = Endianness::bigEndian     ; break;
//                         case MERMAID_TOKEN_ATTR_LE_ME : item.endianness = Endianness::leMiddleEndian; break;
//                         case MERMAID_TOKEN_ATTR_BE_ME : item.endianness = Endianness::beMiddleEndian; break;
//                         default:
//                             return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: something goes wrong" ), (const TokenInfoType*)0;
//                     }
//                 }
//  
//                 // Берем следующий токен и пилим по циклу дальше
//                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//  
//                 continue;
//             }
//  
//             if (tokenTypeVal.oneOf(MERMAID_TOKEN_ATTR_CHECKSUM))
//             {
//                 if (hasChecksum)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: `checksum` option already taken" ), (const TokenInfoType*)0;
//  
//                 hasChecksum = true;
//                 pTokenInfo  = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//  
//                 continue;
//             }
//  
//             // Все опции типа контрольной суммы могут не предваряться ключевым словом checksum, но лучше его использовать
//             // для повышения читаемости
//  
//             // else if (pTokenInfo->tokenType==MERMAID_TOKEN_ATTR_CRC)
//             if (tokenTypeVal.oneOf( MERMAID_TOKEN_ATTR_SIMPLE_SUM, MERMAID_TOKEN_ATTR_SIMPLE_SUM_COMPLEMENT, MERMAID_TOKEN_ATTR_SIMPLE_SUM_INVERT
//                                   , MERMAID_TOKEN_ATTR_SIMPLE_XOR, MERMAID_TOKEN_ATTR_SIMPLE_XOR_COMPLEMENT, MERMAID_TOKEN_ATTR_SIMPLE_XOR_INVERT
//                                   , MERMAID_TOKEN_ATTR_CRC
//                                   )
//                     )
//             {
//                 // или можно не надо?
//                 // if (!hasChecksum)
//                 //     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: 'crc' option: check sum kind already taken" ), (const TokenInfoType*)0;
//  
//                 if (checksumOptions.kind!=ChecksumKind::undefined)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: check sum kind already taken" ), (const TokenInfoType*)0;
//  
//                 switch(pTokenInfo->tokenType)
//                 {
//                     case MERMAID_TOKEN_ATTR_SIMPLE_SUM           : checksumOptions.kind = ChecksumKind::simpleSum          ; break;
//                     case MERMAID_TOKEN_ATTR_SIMPLE_SUM_COMPLEMENT: checksumOptions.kind = ChecksumKind::simpleSumComplement; break;
//                     case MERMAID_TOKEN_ATTR_SIMPLE_SUM_INVERT    : checksumOptions.kind = ChecksumKind::simpleSumInvert    ; break;
//                     case MERMAID_TOKEN_ATTR_SIMPLE_XOR           : checksumOptions.kind = ChecksumKind::simpleXor          ; break;
//                     case MERMAID_TOKEN_ATTR_SIMPLE_XOR_COMPLEMENT: checksumOptions.kind = ChecksumKind::simpleXorComplement; break;
//                     case MERMAID_TOKEN_ATTR_SIMPLE_XOR_INVERT    : checksumOptions.kind = ChecksumKind::simpleXorInvert    ; break;
//                     case MERMAID_TOKEN_ATTR_CRC                  : checksumOptions.kind = ChecksumKind::crc                ; break;
//                 }
//  
//                 //checksumOptions.kind = ChecksumKind::crc;
//                 // hasCrc = true;
//  
//                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//                 if (!pTokenInfo)
//                     return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//                 // Хотим тут range
//                 // !!! Тут надо делать селект - если токен не числовой внутри орга, то литеральный глобальный
//                 // Пока только числовой делаем
//  
//                 EPacketDiagramItemType itemType = EPacketDiagramItemType::invalid;
//                 checksumOptions.addressRangeKind = AddressRangeKind::undefined;
//                 if (isAnyNumber(pTokenInfo->tokenType))
//                 {
//                     pTokenInfo = parseRange(tokenPos, pTokenInfo, checksumOptions.addressRange, itemType, "record definition 'checksum' option");
//                     checksumOptions.addressRangeKind = AddressRangeKind::localIndexes;
//                 }
//                 else
//                 {
//                     pTokenInfo = parseNameRange(tokenPos, pTokenInfo, checksumOptions.addressRangeLabels, "record definition 'checksum' option");
//                     checksumOptions.addressRangeKind = AddressRangeKind::globalLabels;
//                 }
//  
//                 if (!pTokenInfo)
//                     return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//                 if (itemType!=EPacketDiagramItemType::range)
//                 {
//                     // parseRange тут немного херню даёт, и диагностика хромает, показывает следующую запись, но пока так
//                     // !!! Надо разобраться, почему диагностика не туда пырит
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: expected adress range, but got a single value or something else" ), (const TokenInfoType*)0;
//                 }
//  
//                 continue;
//             }
//  
//             if (tokenTypeVal.oneOf(MERMAID_TOKEN_ATTR_SEED))
//             {
//                 if (hasSeed)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: 'seed' option already taken" ), (const TokenInfoType*)0;
//  
//                 hasSeed = true;
//  
//                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//                 if (!pTokenInfo)
//                     return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//                 pTokenInfo = parseNumber(tokenPos, pTokenInfo, checksumOptions.crcConfig.seed, "record definition 'seed' option");
//                 if (!pTokenInfo)
//                     return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//                 continue;
//             }
//  
//             if (tokenTypeVal.oneOf(MERMAID_TOKEN_ATTR_POLY))
//             {
//                 if (hasPoly)
//                     return BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: 'poly' option already taken" ), (const TokenInfoType*)0;
//  
//                 hasPoly = true;
//  
//                 pTokenInfo = BaseClass::waitForSignificantTokenChecked( &tokenPos, ParserWaitForTokenFlags::stopOnLinefeed);
//                 if (!pTokenInfo)
//                     return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//                 pTokenInfo = parseNumber(tokenPos, pTokenInfo, checksumOptions.crcConfig.poly, "record definition 'poly' option");
//                 if (!pTokenInfo)
//                     return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//                 continue;
//             }
//  
//             // else
//             {
//                 BaseClass::logMessage( pTokenInfo, "r-definition", "record definition: something goes wrong - unexpected token" );
//                 return 0;
//             }
//  
//         }
//  
//         if (!pTokenInfo)
//             return 0; // Сообщение уже выведено, просто возвращаем ошибку
//  
//         return returnCheckUpdateOptions();
//     }


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
            commonFlags = EventFlags::override;
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
                bool waitSep = false;

                for( readNextToken()
                   ; umba::TheValue(m_pTokenInfo->tokenType).oneOf(UFSM_TOKEN_OP_OR, UMBA_TOKENIZER_TOKEN_IDENTIFIER)
                   ; readNextToken()
                   )
                {
                    if (waitSep)
                    {
                        if (m_pTokenInfo->tokenType!=UFSM_TOKEN_OP_OR)
                            break;

                        waitSep = false;
                    }
                    else // ждём имя (идентификатор), после разделителя или в начале
                    {
                        if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                            return false; // а пришло хз что
                        //fqn.append(extractIdentifierName(m_pTokenInfo));
                        evd.basicEvents.emplace_back(extractIdentifierName(m_pTokenInfo));
                        waitSep = true;
                    }
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
            commonFlags = ActionFlags::override;
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

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_EXTERNAL)
                {
                    ad.flags |= ActionFlags::external;
                    readNextToken();
                }

                if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_GENERATES)
                {
                    ad.flags |= ActionFlags::external | ActionFlags::generates;
                    // readNextToken();
                    bool waitSep = false;
                    for( readNextToken()
                       ; umba::TheValue(m_pTokenInfo->tokenType).oneOf(UFSM_TOKEN_OP_COMMA, UMBA_TOKENIZER_TOKEN_IDENTIFIER)
                       ; readNextToken()
                       )
                    {
                        if (waitSep)
                        {
                            if (m_pTokenInfo->tokenType!=UFSM_TOKEN_OP_COMMA)
                                break;
                            waitSep = false;
                        }
                        else // ждём имя (идентификатор), после разделителя или в начале
                        {
                            if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                                return false; // а пришло хз что
                            ad.generates.emplace_back(extractIdentifierName(m_pTokenInfo));
                            waitSep = true;
                        }
                    }

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
                bool waitSep = false;

                for( readNextToken()
                   ; umba::TheValue(m_pTokenInfo->tokenType).oneOf(UFSM_TOKEN_OP_COMMA, UMBA_TOKENIZER_TOKEN_IDENTIFIER)
                   ; readNextToken()
                   )
                {
                    if (waitSep)
                    {
                        if (m_pTokenInfo->tokenType!=UFSM_TOKEN_OP_COMMA)
                            break;
                        waitSep = false;
                    }
                    else // ждём имя (идентификатор), после разделителя или в начале
                    {
                        if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                            return false; // а пришло хз что
                        ad.basicActions.emplace_back(extractIdentifierName(m_pTokenInfo));
                        waitSep = true;
                    }
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

    bool parseStateMachineStates(StateMachineDefinition &  /* sm */ )
    {
        return false;
    }

    bool parseStateMachinePredicates(StateMachineDefinition &  /* sm */ )
    {
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

       // Пришел неизвестный токен, но то, что пришло, было у нужном порядке через '::', а пришедший токен проверяем выше, после возврата
       return true;
    }

    //----------------------------------------------------------------------------
    // Останавливается на любом неизвестном токене
    bool parseInheritanceOverrideFlags(InheritanceOverrideFlags &parsedOverrideFlags)
    {
        bool waitComma = false;
        
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

            // После имени может идти override, а после override - обязательно должен идти блок с атрибутами, он не может быть опциональным
            if (m_pTokenInfo->tokenType==UFSM_TOKEN_KWD_OVERRIDE)
            {
                readNextToken(); 
                if (m_pTokenInfo->tokenType!=UFSM_TOKEN_BRACKET_SCOPE_OPEN) // override без блока - это all
                {
                    ple.overrideFlags = InheritanceOverrideFlags::all;
                }
                else
                {
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
            }
            else
            {
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
            }

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

