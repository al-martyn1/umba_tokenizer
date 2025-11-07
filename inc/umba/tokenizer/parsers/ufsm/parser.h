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
#include "../parser_base2.h"
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


//!!! В definitions нельзя добавлять переходы. Сейчас это не проверяется

//!!! Целевое состояние перехода может быть self - надо обработать
// Тут ещё такой нюанс. Если используется self - то действия состояния (state actions) -
// используются действия self-enter/self-leave. Если целевым состоянием перехода задано
// именованное состояние, то, даже если целевое состояние совпадает с исходным,
// то используются действия enter/leave.

//!!! Если у нас список исходных состояний, или там есть ANY-состояние, а в качестве
// целевого состояния задан self - то всё нормально, сложно-составной переход будет разложен
// на элементарные, с одним исх. состоянием, и с одним событием, и там self будет понятно куда
// переходит.


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
    : curNsName(FullQualifiedName::Scheme::absolute)
    {
        //curNsName.flags |= FullQualifiedNameFlags::absolute;
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
template<typename TokenizerType, typename FilenameSetUserDataType=void*>
class Parser : public umba::tokenizer::ParserBase2<TokenizerType, FilenameSetUserDataType>
{

public: // types

    using BaseClass                = umba::tokenizer::ParserBase2<TokenizerType, FilenameSetUserDataType>;
    using tokenizer_type           = typename BaseClass::tokenizer_type             ;
    using TokenCollectionType      = typename BaseClass::TokenCollectionType        ;
    using TokenCollectionItemType  = typename BaseClass::TokenCollectionItemType    ;
    using TokenInfoType            = typename BaseClass::TokenInfoType              ;
    using TokenPosType             = typename BaseClass::TokenPosType               ;
    using token_parsed_data_type   = typename BaseClass::token_parsed_data_type     ;
    using shared_log_type          = typename BaseClass::shared_log_type            ;
    using string_type              = typename BaseClass::string_type                ;
    using FilenameSetType          = typename BaseClass::FilenameSetType            ;
    using SharedFilenameSetType    = typename BaseClass::SharedFilenameSetType      ;
    using filename_set_type        = typename BaseClass::filename_set_type          ;
    using shared_filename_set_type = typename BaseClass::shared_filename_set_type   ;
    using payload_type             = typename BaseClass::payload_type               ;

    using plid_list                = std::initializer_list<payload_type>;


protected:

    ParsingContext                 ctx;


public:

    UMBA_RULE_OF_FIVE_COPY_DELETE(Parser);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(Parser);

    Parser() {}

    explicit
    Parser(std::shared_ptr<TokenCollectionType> tc, SharedFilenameSetType pFilenameSet, shared_log_type a_log)
    : BaseClass(tc, pFilenameSet, a_log)
    , ctx()
    {
    }

    NamespaceDefinition getParsedData() const
    {
        return ctx.rootNs;
    }

    FullTokenPosition getFullPos(TokenPosType pos) const  { return BaseClass::getFullPos(pos); }
    FullTokenPosition getFullPos() const                  { return BaseClass::getFullPos();    }
    payload_type getTokenType() const                     { return BaseClass::getTokenType(); }
    void readNextToken(ParserWaitForTokenFlags readFlags=ParserWaitForTokenFlags::none)   { BaseClass::readNextToken(readFlags); }
    string_type extractIdentifierName() const { return BaseClass::extractIdentifierName(); }
    string_type extractLiteral() const        { return BaseClass::extractLiteral() ;}

    const token_parsed_data_type* getTokenParsedDataPtr() const { return BaseClass::getTokenParsedDataPtr(); }
    token_parsed_data_type getTokenParsedData() const           { return BaseClass::getTokenParsedData(); }

    template <typename... TArgs>
    bool isTokenOneOf(TArgs&&... args) const
    {
        return ((getTokenType() == args) || ...);
    }

    void logSimpleMessage(const std::string &msgId, const std::string &msg) const { BaseClass::logSimpleMessage(msgId, msg); }
    void logSimpleMessage(TokenPosType pos, const std::string &msgId, const std::string &msg) const { BaseClass::logSimpleMessage(pos, msgId, msg); }
    void logSimpleMessage(TokenPosType pos, payload_type tokenType, const std::string &msgId, const std::string &msg) const { BaseClass::logSimpleMessage(pos, tokenType, msgId, msg); }
    void logSimpleUnexpected(payload_type payloadUnexpected) const { BaseClass::logSimpleUnexpected(payloadUnexpected); }
    void logSimpleUnexpected() const                               { BaseClass::logSimpleUnexpected(); }
    void logMessage(const std::string &msgId, const std::string &msg, const typename umba::FormatMessage<std::string>::macros_map_type &args ) { BaseClass::logMessage(msgId, msg, args ); }
    void logMessage(const std::string &msgId, const std::string &msg) { BaseClass::logMessage(msgId, msg ); }

    bool expectedReachedMsg(std::initializer_list<payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        return BaseClass::expectedReachedMsg(payloadExpectedList, msg);
    }

    bool expectedReachedMsg(payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        return BaseClass::expectedReachedMsg(payloadExpected, msg);
    }

    bool checkExactTokenType(std::initializer_list<payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        return BaseClass::checkExactTokenType(payloadExpectedList, msg);
    }

    bool checkExactTokenType(payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        return BaseClass::checkExactTokenType(payloadExpected, msg);
    }

    template<typename TokenHandler>
    bool readHomogeneousTokensList( payload_type tokenToken, payload_type tokenSep
                                  , bool readNextOnStart
                                  , TokenHandler handler
                                  , bool initialWaitSep = false
                                  )
    {
         return BaseClass::readHomogeneousTokensList(tokenToken, tokenSep, readNextOnStart, handler, initialWaitSep);
    }



    NamespaceDefinition* getCurrentNamespace()
    {
        NamespaceDefinition* pCurNs = ctx.rootNs.getNamespace(ctx.curNsName, &ctx.rootNs, ctx.lastNsPos);
        if (!pCurNs)
            throw std::runtime_error("Something goes wrong");
        return pCurNs;
    }


    virtual
    std::string getTokenIdStr(umba::tokenizer::payload_type tk) const override
    {
        switch(tk)
        {
            case UFSM_TOKEN_KWD_NAMESPACE           : return "namespace";
            case UFSM_TOKEN_KWD_FSM                 : return "state-machine";
            case UFSM_TOKEN_KWD_DEFINITIONS         : return "definitions";
            case UFSM_TOKEN_BRACKET_SCOPE_OPEN      : return "scope-open-bracket";
            case UFSM_TOKEN_BRACKET_SCOPE_CLOSE     : return "scope-close-bracket";

            case UMBA_TOKENIZER_TOKEN_IDENTIFIER    : return "identifier";
            case UMBA_TOKENIZER_TOKEN_STRING_LITERAL: return "string-literal";
            case UMBA_TOKENIZER_TOKEN_BACKTICK_LITERAL: return "charset-definition";

            case UFSM_TOKEN_KWD_EVENTS              : return "events";
            case UFSM_TOKEN_KWD_ACTIONS             : return "actions";
            case UFSM_TOKEN_KWD_STATES              : return "states";
            case UFSM_TOKEN_KWD_PREDICATES          : return "predicates";
            case UFSM_TOKEN_KWD_TRANSITIONS         : return "transitios";
            case UFSM_TOKEN_KWD_ALL                 : return "all";

            case UFSM_TOKEN_KWD_USES                : return "uses";
            case UFSM_TOKEN_KWD_INHERITS            : return "inherits";
            case UFSM_TOKEN_KWD_OVERRIDE            : return "override";

            case UFSM_TOKEN_KWD_GENERATED           : return "generated";
            case UFSM_TOKEN_KWD_GENERATES           : return "generates";
            //case          : return "";

            case UFSM_TOKEN_KWD_SELF                : return "self";
            

            case UFSM_TOKEN_OP_DESCR_FOLLOWS        : return "description-separator";
            case UFSM_TOKEN_OP_ANY                  : return "op-any";
            case UFSM_TOKEN_OP_NOT                  : return "op-not";
            case UFSM_TOKEN_OP_AND                  : return "op-and";
            case UFSM_TOKEN_OP_OR                   : return "op-or";
            case UFSM_TOKEN_OP_NOT_ALTER            : return "op-not";
            case UFSM_TOKEN_OP_AND_ALTER            : return "op-and";
            case UFSM_TOKEN_OP_OR_ALTER             : return "op-or";
            case UFSM_TOKEN_OP_ASSIGN               : return "op-assign";
            case UFSM_TOKEN_OP_COLON                : return "colon";
            case UFSM_TOKEN_OP_COMMA                : return "comma";
            case UFSM_TOKEN_OP_SCOPE                : return "op-scope-resolution";
            case UFSM_TOKEN_OP_SEMICOLON            : return "semicolon";
            case UFSM_TOKEN_OP_TRANSITION_ARROW     : return "transition-arrow";
            case UFSM_TOKEN_OP_TRANSITION_CONDITION : return "transition-condition";


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

    // bool expectedReachedMsg(const TokenInfoType *pTokenInfo, std::initializer_list<umba::tokenizer::payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    // {
    //     BaseClass::logUnexpected( pTokenInfo, payloadExpectedList, msg, [&](umba::tokenizer::payload_type tk) { return getTokenIdStr(tk); } );
    //     return false;
    // }
    //  
    // bool expectedReachedMsg(const TokenInfoType *pTokenInfo, umba::tokenizer::payload_type payloadExpected, const std::string &msg=std::string()) const
    // {
    //     BaseClass::logUnexpected( pTokenInfo, payloadExpected, msg, [&](umba::tokenizer::payload_type tk) { return getTokenIdStr(tk); } );
    //     return false;
    // }
    //  
    // bool checkExactTokenType(const TokenInfoType *pTokenInfo, std::initializer_list<umba::tokenizer::payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    // {
    //     for(auto e : payloadExpectedList)
    //     {
    //         if (e==pTokenInfo->tokenType) // tkReached
    //             return true;
    //     }
    //     return expectedReachedMsg(pTokenInfo, payloadExpectedList, msg);
    // }
    //  
    // bool checkExactTokenType(const TokenInfoType *pTokenInfo, umba::tokenizer::payload_type payloadExpected, const std::string &msg=std::string()) const
    // {
    //     // for(auto e : payloadExpectedList)
    //     // {
    //     //     if (e==pTokenInfo->tokenType) // tkReached
    //     //         return true;
    //     // }
    //  
    //     if (payloadExpected==pTokenInfo->tokenType)
    //         return true;
    //  
    //     return expectedReachedMsg(pTokenInfo, payloadExpected, msg);
    // }
    //  
    //  
    // template<typename TokenHandler>
    // bool readHomogeneousTokensList( umba::tokenizer::payload_type tokenToken, umba::tokenizer::payload_type tokenSep
    //                               , bool readNextOnStart
    //                               , TokenHandler handler
    //                               , bool initialWaitSep = false
    //                               )
    // {
    //     if (readNextOnStart)
    //         readNextToken();
    //  
    //     bool waitSep = initialWaitSep;
    //  
    //     for( 
    //        ; umba::TheValue(m_pTokenInfo->tokenType).oneOf(tokenSep, tokenToken)
    //        ; readNextToken()
    //        )
    //     {
    //         if (waitSep)
    //         {
    //             if (m_pTokenInfo->tokenType!=tokenSep)
    //                 break;
    //             waitSep = false;
    //         }
    //         else // ждём tokenToken, после разделителя или в начале
    //         {
    //             if (!checkExactTokenType(m_pTokenInfo, tokenToken /* , "'display-options' directive: invalid option value" */ ))
    //                 return false; // а пришло хз что
    //             //evd.basicEvents.emplace_back(extractIdentifierName(m_pTokenInfo));
    //             handler();
    //             waitSep = true;
    //         }
    //     }
    //  
    //     return true;
    // }


    //----------------------------------------------------------------------------

    // cmdStopTraffic : external - "The RED light (stop) mode is on";
    // tmStopTraffic : external generated;
    // evIntegralLiteral = evBoolLiteral | evIntLiteral - "Пришел интегральный литерал";

    bool parseStateMachineEvents(StateMachineDefinition & sm)
    {
        // Пропускаем ключевое слово events, вычитываем следующий токен - должна быть открывающая фигурная скобка
        readNextToken();

        EventFlags commonFlags = EventFlags::none;

        if (getTokenType()==UFSM_TOKEN_OP_COLON)
        {
            readNextToken();
            if (!checkExactTokenType({UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= EventFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (getTokenType()==UFSM_TOKEN_BRACKET_SCOPE_CLOSE)
                return true;

            if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            EventDefinition evd;
            evd.positionInfo = getFullPos();
            evd.name  = extractIdentifierName();
            evd.flags = commonFlags;

            readNextToken();

            if (isTokenOneOf(UFSM_TOKEN_OP_SEMICOLON))
            {
                // Ничего дополнительно не задано, считаем событие как external
                evd.flags |= EventFlags::external;
                sm.addDefinition(evd);
                continue;
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
            {
                readNextToken();
                if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                evd.description = extractLiteral();
                evd.flags |= EventFlags::external;
                sm.addDefinition(evd);
                readNextToken();
                if (!checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                    return false;
                continue;
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
            {
                readNextToken();

                if (isTokenOneOf(UFSM_TOKEN_KWD_EXTERNAL))
                {
                    evd.flags |= EventFlags::external;
                    readNextToken();
                }

                if (isTokenOneOf(UFSM_TOKEN_KWD_GENERATED))
                {
                    evd.flags |= EventFlags::external | EventFlags::generated;
                    readNextToken();
                }

                if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
                {
                    readNextToken();
                    if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    evd.description = extractLiteral();
                    readNextToken();
                }

                sm.addDefinition(evd);
            }
            else if (isTokenOneOf(UFSM_TOKEN_OP_ASSIGN))
            {
                if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_OR
                                              , true /* readNextOnStart */
                                              , [&]() { evd.basicEvents.emplace_back(extractIdentifierName()); } ))
                    return false;

                if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
                {
                    readNextToken();
                    if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    evd.description = extractLiteral();
                    readNextToken();
                }

                sm.addDefinition(evd);

            }
            else
            {
                return checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON,UFSM_TOKEN_OP_DESCR_FOLLOWS,UFSM_TOKEN_OP_COLON,UFSM_TOKEN_OP_ASSIGN} /* , "'display-options' directive: invalid option value" */ );
            }

            // Тут у нас должна быть точка с запятой
            if (!checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
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

        if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
        {
            readNextToken();
            if (!checkExactTokenType({UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= ActionFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_CLOSE))
                return true;

            if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            ActionDefinition ad;
            ad.positionInfo = getFullPos();
            ad.name  = extractIdentifierName();
            ad.flags = commonFlags;

            readNextToken();

            if (isTokenOneOf(UFSM_TOKEN_OP_SEMICOLON))
            {
                // Ничего дополнительно не задано, считаем событие как external
                ad.flags |= ActionFlags::external;
                sm.addDefinition(ad);
                continue;
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
            {
                readNextToken();
                if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                ad.description = extractLiteral();
                ad.flags |= ActionFlags::external;
                sm.addDefinition(ad);
                readNextToken();
                if (!checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                    return false;
                continue;
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
            {
                readNextToken();

                ad.flags |= ActionFlags::external;

                if (isTokenOneOf(UFSM_TOKEN_KWD_EXTERNAL))
                {
                    readNextToken();
                }

                if (isTokenOneOf(UFSM_TOKEN_KWD_GENERATES))
                {
                    ad.flags |= ActionFlags::external | ActionFlags::generates;

                    if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                                  , true /* readNextOnStart */
                                                  , [&]() { ad.generates.emplace_back(extractIdentifierName()); } ))
                        return false;

                }

                if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
                {
                    readNextToken();
                    if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    ad.description = extractLiteral();
                    readNextToken();
                }

                sm.addDefinition(ad);
            }
            else if (isTokenOneOf(UFSM_TOKEN_OP_ASSIGN))
            {
                if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                              , true /* readNextOnStart */
                                              , [&]() { ad.basicActions.emplace_back(extractIdentifierName()); } ))
                    return false;


                if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
                {
                    readNextToken();
                    if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    ad.description = extractLiteral();
                    readNextToken();
                }

                sm.addDefinition(ad);

            }
            else
            {
                return checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON,UFSM_TOKEN_OP_DESCR_FOLLOWS,UFSM_TOKEN_OP_COLON,UFSM_TOKEN_OP_ASSIGN} /* , "'display-options' directive: invalid option value" */ );
            }

            // Тут у нас должна быть точка с запятой
            if (!checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

        } // while(true)

        return false;
    }

    bool parseStateMachineStates(StateMachineDefinition &sm )
    {
        // Пропускаем ключевое слово actions, вычитываем следующий токен - должна быть открывающая фигурная скобка
        readNextToken();

        StateFlags commonFlags = StateFlags::none;

        if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
        {
            readNextToken();
            if (!checkExactTokenType({UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= StateFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_CLOSE))
                return true;

            if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            StateDefinition sd;
            sd.positionInfo = getFullPos();
            sd.name  = extractIdentifierName();
            sd.flags = commonFlags;

            readNextToken();

            if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
            {
                readNextToken(); // вычитываем следующий токен
                // может быть только признак initial/final/error
                if (!checkExactTokenType({UFSM_TOKEN_KWD_INITIAL, UFSM_TOKEN_KWD_FINAL, UFSM_TOKEN_KWD_ERROR} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что

                if (isTokenOneOf(UFSM_TOKEN_KWD_INITIAL))
                    sd.flags |= StateFlags::initial;
                else if (isTokenOneOf(UFSM_TOKEN_KWD_FINAL))
                    sd.flags |= StateFlags::final;
                else // UFSM_TOKEN_KWD_ERROR
                    sd.flags |= StateFlags::final | StateFlags::error;

                readNextToken();
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
            {
                readNextToken();
                if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                sd.description = extractLiteral();
                readNextToken();
            }

            // Теперь ожидаем блок с различными типами действий: enter/leave/self-enter/self-leave,
            // или ожидаем завершение выражения точкой с запятой

            if (isTokenOneOf(UFSM_TOKEN_OP_SEMICOLON))
            {
                // ничего не делаем
            }
            else if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_OPEN))
            {
                while(true)
                {
                    // Ждём идентификатор типа действия или конец блока
                    readNextToken();
                    if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_CLOSE))
                        break;
      
                    // Ждём идентификатор типа действия
                    if (!checkExactTokenType({UFSM_TOKEN_KWD_ENTER, UFSM_TOKEN_KWD_LEAVE, UFSM_TOKEN_KWD_SELF_ENTER, UFSM_TOKEN_KWD_SELF_LEAVE} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
    
                    StateActionKind saKind = StateActionKind::none;
    
                    switch(getTokenType())
                    {
                        case UFSM_TOKEN_KWD_ENTER     : saKind = StateActionKind::stateEnter; break;
                        case UFSM_TOKEN_KWD_LEAVE     : saKind = StateActionKind::stateLeave; break;
                        case UFSM_TOKEN_KWD_SELF_ENTER: saKind = StateActionKind::selfEnter ; break;
                        case UFSM_TOKEN_KWD_SELF_LEAVE: saKind = StateActionKind::selfLeave ; break;
                    }
    
                    readNextToken();
                    // Ждем разделитель перед списком действий
                    if (!checkExactTokenType({UFSM_TOKEN_OP_COLON} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
                    
                    if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                                  , true /* readNextOnStart */
                                                  , [&]() { sd.actionRefs[saKind].push_back(extractIdentifierName()); } ))
                        return false;
    
                    // Тут у нас должна быть точка с запятой
                    if (!checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что
    
                } // while(true)
            }    
            else
            {
                if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN, UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
            }

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
            logSimpleMessage("expression-parsering", "parser initialization failed");
            return false;
        }

        using marty::expressions::tokenizer_helpers::convertTokenizerEvent;
        using marty::expressions::tokenizer_helpers::IdentifierConvertTraits;


        if (readNextOnStart)
            readNextToken();
         
        std::size_t tokensCount = 0;
        for( 
           ; umba::TheValue(getTokenType())
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
            if (!convertTokenizerEvent<tokenizer_type>( inputItem, getTokenType()
                                      , getFullPos(), getTokenParsedData()
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
                logSimpleMessage("expression-parsing", parser.getErrorMessage(parserErr));
                return false;
            }
        }

        if (!tokensCount)
        {
            //BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "expression-parsering", "expression is empty");
            logSimpleMessage("expression-parsing", "expression is empty");
            return false;
        }

        // ParserErrorType 
        parserErr = parser.finalize();
        if (parserErr!= ParserErrorType::none)
        {
            //BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "expression-parsering", parser.getErrorMessage(parserErr));
            logSimpleMessage("expression-parsing", parser.getErrorMessage(parserErr));
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

        if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
        {
            readNextToken();
            if (!checkExactTokenType({UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= PredicateFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        while(true)
        {
            // Ждём идентификатор или конец блока
            readNextToken();
            if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_CLOSE))
                return true;

            if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что

            PredicateDefinition pd;
            pd.positionInfo = getFullPos();
            pd.name  = extractIdentifierName();
            pd.flags = commonFlags;

            readNextToken();

            //bool hasExpression = false;

            if (isTokenOneOf(UFSM_TOKEN_OP_ASSIGN))
            {
                readNextToken();

                if (isTokenOneOf(UMBA_TOKENIZER_TOKEN_BACKTICK_LITERAL))
                {
                    pd.flags |= PredicateFlags::charSet;
                    std::string bktckStr = extractLiteral();
                    std::size_t errPos = 0;
                    if (!char_class::utils::parseCharClassDefinition(bktckStr, pd.charSet, &errPos))
                    {
                        // Тупо прибавить позицию с ошибкой (+errPos) не прокатило, надо подумать, что не так
                        logSimpleMessage("predicate-charset", "predicate char set definition is invalid");
                        return false;
                    }

                    readNextToken();

                    // !!! Дублирование кода, вынести в отдельную лямбду
                    if (isTokenOneOf(UFSM_TOKEN_KWD_VALID_FOR))
                    {
                        pd.flags |= PredicateFlags::validFor;
    
                        readNextToken();
                        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                            return false; // а пришло хз что
    
                        if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                                      , true /* readNextOnStart */
                                                      , [&]() { pd.validForList.push_back(extractIdentifierName()); } ))
                            return false;
    
                        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                            return false; // а пришло хз что
    
                        // Нужно проверить на пустоту список valid-for
    
                        if (pd.validForList.empty())
                        {
                            // BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "valid-for", "empty 'valid-for' list");
                            logSimpleMessage("valid-for", "empty 'valid-for' list");
                            return false;
                        }
    
                        readNextToken(); // скипнули закрывающую скобку
                    }

                }
                else
                {
                    if (!readLogicExpression(pd.expression, false /* !readNextOnStart */ ))
                        return false;
                }
            }
            else if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
            {
                pd.flags |= PredicateFlags::external;

                readNextToken();

                if (isTokenOneOf(UFSM_TOKEN_KWD_EXTERNAL))
                {
                    readNextToken();
                }

                if (isTokenOneOf(UFSM_TOKEN_KWD_VALID_FOR))
                {
                    pd.flags |= PredicateFlags::validFor;

                    readNextToken();
                    if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что

                    if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                                  , true /* readNextOnStart */
                                                  , [&]() { pd.validForList.push_back(extractIdentifierName()); } ))
                        return false;

                    if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз что

                    // Нужно проверить на пустоту список valid-for

                    if (pd.validForList.empty())
                    {
                        // BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "valid-for", "empty 'valid-for' list");
                        logSimpleMessage("valid-for", "empty 'valid-for' list");
                        return false;
                    }

                    readNextToken(); // скипнули закрывающую скобку
                }
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
            {
                readNextToken();
                if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                pd.description = extractLiteral();
                readNextToken();
            }

            if (!checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                return false;

            sm.addDefinition(pd);

            continue;

        }

        return false;
    }

    // СписокИсхСостояний - Состояние1 [, Состояние2 [, Состояние3... ] ]
    // СостояниеN         - */Состояние/!Состояние
    // СписокСобытий      - Событие1 [, Событие2 [, Событие3... ] ]
    // СобытиеN           - */Событие/!Событие
    template<typename HandlerType>
    bool parseStateOrEventsListWithAny(HandlerType handler)
    {
        bool notFlag = false;
        while(true)
        {
            notFlag = false;

            auto positionInfo = getFullPos();

            if (isTokenOneOf(UFSM_TOKEN_OP_NOT, UFSM_TOKEN_OP_NOT_ALTER))
            {
                notFlag = true;
                readNextToken();
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_ANY))
            {
                if (notFlag)
                {
                    logSimpleMessage("not-any", "the ANY ('*') value cannot be used with negation");
                    return false;
                }

                if (!handler(positionInfo, notFlag, getTokenType(), std::string()))
                    return false;
            }
            else if (isTokenOneOf(UMBA_TOKENIZER_TOKEN_IDENTIFIER))
            {
                if (!handler(positionInfo, notFlag, getTokenType(), extractIdentifierName()))
                    return false;
            }
            else
            {
                return checkExactTokenType({UFSM_TOKEN_OP_ANY, UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ );
            }

            readNextToken();

            if (isTokenOneOf(UFSM_TOKEN_OP_COMMA))
            {
                readNextToken();
                continue;
            }

            return true;
        }

        // UFSM_TOKEN_OP_ANY
        // UFSM_TOKEN_OP_COMMA
        // UMBA_TOKENIZER_TOKEN_IDENTIFIER
    }

    bool parseStateMachineTransitions(StateMachineDefinition &sm)
    {
        // Пропускаем ключевое слово transitions, вычитываем следующий токен - должна быть открывающая фигурная скобка
        readNextToken();

        TransitionFlags commonFlags = TransitionFlags::none; // conditional

        if (isTokenOneOf(UFSM_TOKEN_OP_COLON)) // optional sequence
        {
            readNextToken();
            if (!checkExactTokenType({UFSM_TOKEN_KWD_OVERRIDE} /* , "'display-options' directive: invalid option value" */ ))
                return false; // а пришло хз что
            commonFlags |= TransitionFlags::override;
            readNextToken();
        }

        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false; // а пришло хз что

        // turnedOff : cmdStopTraffic   -> trafficStopped;
        // turnedOff : cmdAllowTraffic  -> trafficAllowed;
        // turnedOff : * -> self; // cmdTurnOff,cmdUncontrolledMode и пр -> turnedOff
        //  
        // trafficAllowed : cmdStopTraffic      -> stopNotice;
        // trafficAllowed : * -> self; // cmdAllowTraffic и пр -> trafficAllowed, 
        //  
        // stopNotice : tmToggleGreen ?  greenLightIsOn -> self : greenOff;
        // stopNotice : tmToggleGreen ? !greenLightIsOn -> self : greenOn;
        // stopNotice : tmStopTraffic                   -> trafficStopped;
        // stopNotice : * -> self;
        //  
        // trafficStopped : cmdAllowTraffic -> trafficAllowed;
        // trafficStopped : cmdTurnOff      -> turnedOff;
        // trafficStopped : * -> self;
        //  
        // * : cmdUncontrolledMode -> turnedOff;
        // * : cmdTurnOff          -> turnedOff;
        // * : tmAlivePoll -> self : reportAlive;

        // State1, State2 : Event          -> TargetState ? Condition - "Описание"
        // *, State2      : Event          -> TargetState ? Condition - "Описание"
        // *, !State2     : Event1, Event2 -> TargetState ? Condition - "Описание"
        // *, !State2     : *              -> TargetState ? Condition - "Описание"

        // Синтаксис описания переходов:
        // СписокИсхСостояний : СписокСобытий [? ДопУсловие] -> ЦелевоеСостояние [? ДопУсловие] [- Описание];
        // где
        //   СписокИсхСостояний - Состояние1 [, Состояние2 [, Состояние3... ] ]
        //   СостояниеN         - */Состояние/!Состояние
        //   СписокСобытий      - Событие1 [, Событие2 [, Событие3... ] ]
        //   СобытиеN           - */Событие/!Событие

        // struct TransitionDefinition
        // {
        //     PositionInfo               positionInfo;
        //     std::string                description ;
        //     TransitionSourceStates     sourceStates; 
        //     TransitionEvents           events      ;
        //     TransitionFlags            flags;
        //     LogicExpression            additionalCondition;

        while(true)
        {
            // Ждём конец блока
            readNextToken();
            if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_CLOSE))
                return true;

            // if (!checkExactTokenType(m_pTokenInfo, {UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
            //     return false; // а пришло хз что

            TransitionDefinition td;
            td.positionInfo = getFullPos();
            // td.name  = extractIdentifierName(m_pTokenInfo);
            td.flags = commonFlags;

            if (!parseStateOrEventsListWithAny( [&](PositionInfo positionInfo, bool bNot, auto tokenType, const std::string &ident)
                {

                    TransitionSourceState tss;
                    tss.positionInfo = positionInfo;
                    tss.flags = TransitionSourceStateFlags::none;
                    // TransitionSourceStateFlags flags = TransitionSourceStateFlags::none;
                    if (tokenType==UFSM_TOKEN_OP_ANY)
                    {
                        if (bNot)
                            throw std::runtime_error("something goes wrong while calling parseStateOrEventsListWithAny");
                
                        if (td.sourceStates.checkForAny())
                        {
                            logSimpleMessage(positionInfo.pos, "already-any", "the ANY ('*') value already in list");
                            return false;
                        }

                        if (!td.sourceStates.empty())
                        {
                            logSimpleMessage(positionInfo.pos, "first-any", "the ANY ('*') value must be added first");
                            return false;
                        }
                
                        tss.flags |= TransitionSourceStateFlags::any;
                    }
                    else // identifier
                    {
                        if (bNot!=td.sourceStates.checkForAny())
                        {
                            // есть отрицание, но нет ANY
                            // нет отрицания, но есть ANY
                            // Итого в список мы можем добавить
                            //   если есть ANY - только с отрицанием
                            //   если нет ANY  - только без отрицания
                            if (bNot)
                                logSimpleMessage(positionInfo.pos, "missing-any", "the ANY ('*') value must be first in list before adding exclusions");
                            else
                                logSimpleMessage(positionInfo.pos, "already-any", "adding normal value (with no exclusion) to list with ANY ('*') is wrong");

                            return false;
                        }

                        if (bNot)
                            tss.flags |= TransitionSourceStateFlags::excluded;

                        tss.name = ident;
                    }

                    td.sourceStates.push_back(tss);

                    return true;
                }
               ))
            {
                return false;
            }

            if (!checkExactTokenType({UFSM_TOKEN_OP_COLON} /* , "'display-options' directive: invalid option value" */ ))
                return false;

            readNextToken();


            if (!parseStateOrEventsListWithAny( [&](PositionInfo positionInfo, bool bNot, auto tokenType, const std::string &ident)
                {

                    TransitionEvent te;
                    te.positionInfo = positionInfo;
                    te.flags = TransitionEventFlags::none;
                    // TransitionSourceStateFlags flags = TransitionSourceStateFlags::none;
                    if (tokenType==UFSM_TOKEN_OP_ANY)
                    {
                        if (bNot)
                            throw std::runtime_error("something goes wrong while calling parseStateOrEventsListWithAny");
                
                        if (td.events.checkForAny())
                        {
                            logSimpleMessage(positionInfo.pos, "already-any", "the ANY ('*') value already in list");
                            return false;
                        }

                        if (!td.events.empty())
                        {
                            logSimpleMessage(positionInfo.pos, "first-any", "the ANY ('*') value must be added first");
                            return false;
                        }
                
                        te.flags |= TransitionEventFlags::any;
                    }
                    else // identifier
                    {
                        if (bNot!=td.events.checkForAny())
                        {
                            // есть отрицание, но нет ANY
                            // нет отрицания, но есть ANY
                            // Итого в список мы можем добавить
                            //   если есть ANY - только с отрицанием
                            //   если нет ANY  - только без отрицания
                            if (bNot)
                                logSimpleMessage(positionInfo.pos, "missing-any", "the ANY ('*') value must be first in list before adding exclusions");
                            else
                                logSimpleMessage(positionInfo.pos, "already-any", "adding normal value (with no exclusion) to list with ANY ('*') is wrong");

                            return false;
                        }

                        if (bNot)
                            te.flags |= TransitionEventFlags::excluded;

                        te.name = ident;
                    }

                    td.events.push_back(te);

                    return true;
                }
               ))
            {
                return false;
            }


            if (isTokenOneOf(UFSM_TOKEN_OP_TRANSITION_CONDITION))
            {
                // Читаем условие
                if (!readLogicExpression(td.additionalCondition, true /* readNextOnStart */ ))
                    return false;

                td.flags |= TransitionFlags::conditional;
            }

            if (!checkExactTokenType({UFSM_TOKEN_OP_TRANSITION_ARROW} /* , "'display-options' directive: invalid option value" */ ))
                return false;

            readNextToken();

            if (isTokenOneOf(UFSM_TOKEN_KWD_SELF, UMBA_TOKENIZER_TOKEN_IDENTIFIER))
            {
                if (isTokenOneOf(UFSM_TOKEN_KWD_SELF))
                {
                    td.flags |= TransitionFlags::selfTarget;
                }
                else // identifier
                {
                    td.targetState = extractIdentifierName();
                }
            }

            readNextToken();

            if (isTokenOneOf(UFSM_TOKEN_OP_TRANSITION_CONDITION))
            {
                if ((td.flags&TransitionFlags::conditional)!=0)
                {
                    BaseClass::logSimpleMessage("already-tr-condition", "transition condition is already set");
                    return false;
                }

                // Читаем условие
                if (!readLogicExpression(td.additionalCondition, true /* readNextOnStart */ ))
                    return false;

                td.flags |= TransitionFlags::conditional;
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
            {
                if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_COMMA
                                              , true /* readNextOnStart */
                                              , [&]() { td.actionRefs.push_back(extractIdentifierName()); } ))
                    return false;
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
            {
                readNextToken();
                if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                td.description = extractLiteral();
                readNextToken();
            }

            sm.addDefinition(td);

            if (!checkExactTokenType({UFSM_TOKEN_OP_SEMICOLON} /* , "'display-options' directive: invalid option value" */ ))
                return false;
            
            //readNextToken();
        }

        return false;
    }

    bool parseStateMachineBody(StateMachineDefinition & sm)
    {
        // Скобка уже открыта
        while(true)
        {
            readNextToken(); 

            switch(getTokenType())
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
                     // sm.prioritySortTransitions(); // тут не сортируем, пусть тут всё остаётся так, как введено
                     break;

                case UFSM_TOKEN_BRACKET_SCOPE_CLOSE:
                {
                     // выбрали закрывающую скобку стейт-машины со входа
                     // readNextToken(); // или не надо
                     return true;
                }

                default:
                {
                     if (!checkExactTokenType({UFSM_TOKEN_KWD_EVENTS,UFSM_TOKEN_KWD_ACTIONS,UFSM_TOKEN_KWD_STATES,UFSM_TOKEN_KWD_PREDICATES,UFSM_TOKEN_KWD_TRANSITIONS,UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
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

        if (isTokenOneOf(UFSM_TOKEN_OP_SCOPE))
        {
            // В начале у нас оператор скоупа, значит имя абсолютное
            waitSep = true;
            //fqn.flags = FullQualifiedNameFlags::absolute;
            fqn.makeAbsolute();
        }
        else
        {
            //fqn.flags = FullQualifiedNameFlags::none;
            fqn.makeRelative();
        }

        if (!readHomogeneousTokensList( UMBA_TOKENIZER_TOKEN_IDENTIFIER, UFSM_TOKEN_OP_SCOPE
                                      , false /* readNextOnStart */
                                      , [&]() { fqn.append(extractIdentifierName()); } 
                                      , waitSep ))
            return false;

       // Пришел неизвестный токен, но то, что пришло, было у нужном порядке через '::', а пришедший токен проверяем выше, после возврата
       return true;
    }

    //----------------------------------------------------------------------------
    // Останавливается на любом неизвестном токене
    bool parseInheritanceOverrideFlags(InheritanceOverrideFlags &parsedOverrideFlags)
    {
        bool waitComma = false;

        std::size_t flagsCount = 0;
        
        for( readNextToken(ParserWaitForTokenFlags::none) // пропустили открывающую скобку
           ; umba::TheValue(getTokenType()).oneOf( UFSM_TOKEN_OP_COMMA
                                                          , UFSM_TOKEN_KWD_ACTIONS    
                                                          , UFSM_TOKEN_KWD_EVENTS     
                                                          , UFSM_TOKEN_KWD_PREDICATES 
                                                          , UFSM_TOKEN_KWD_STATES     
                                                          , UFSM_TOKEN_KWD_TRANSITIONS
                                                          , UFSM_TOKEN_KWD_ALL
                                                          )
           ; readNextToken(ParserWaitForTokenFlags::none)
           )
        {
            if (waitComma)
            {
                if (!checkExactTokenType({UFSM_TOKEN_OP_COMMA} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // пришло что-то другое - ошибка
                waitComma = false;
            }
            else // ждём флаг (кейворд флага)
            {
                if (!checkExactTokenType({UFSM_TOKEN_KWD_EVENTS, UFSM_TOKEN_KWD_ACTIONS, UFSM_TOKEN_KWD_STATES, UFSM_TOKEN_KWD_PREDICATES, UFSM_TOKEN_KWD_TRANSITIONS, UFSM_TOKEN_KWD_ALL} /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришла запятая
                InheritanceOverrideFlags newFlagVal = InheritanceOverrideFlags::none;
                switch(getTokenType())
                {
                    case UFSM_TOKEN_KWD_ACTIONS    : ++flagsCount; newFlagVal = InheritanceOverrideFlags::actions    ; break;
                    case UFSM_TOKEN_KWD_EVENTS     : ++flagsCount; newFlagVal = InheritanceOverrideFlags::events     ; break;
                    case UFSM_TOKEN_KWD_PREDICATES : ++flagsCount; newFlagVal = InheritanceOverrideFlags::predicates ; break;
                    case UFSM_TOKEN_KWD_STATES     : ++flagsCount; newFlagVal = InheritanceOverrideFlags::states     ; break;
                    case UFSM_TOKEN_KWD_TRANSITIONS: ++flagsCount; newFlagVal = InheritanceOverrideFlags::transitions; break;
                    case UFSM_TOKEN_KWD_ALL        : ++flagsCount; newFlagVal = InheritanceOverrideFlags::all        ; break;
                };

                //!!! Нужно проверить наличие этого флага, если уже установлен, то это ошибка
                // Пока просто устанавливаем без проверки
                parsedOverrideFlags |= newFlagVal;
                waitComma = true;
            }
        }

        if (!flagsCount)
        {
            // BaseClass::logSimpleMessage(getFullPos(), m_pTokenInfo->tokenType, "override-list", "empty 'override' list");
            logSimpleMessage("override-list", "empty 'override' list");
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
            if (umba::TheValue(getTokenType()).oneOf(UFSM_TOKEN_KWD_USES, UFSM_TOKEN_KWD_INHERITS))
            {
                if (isTokenOneOf(UFSM_TOKEN_KWD_USES))
                    inheritanceListMode = InheritanceListMode::uses;
                else
                    inheritanceListMode = InheritanceListMode::inherits;

                readNextToken(); 
            }

            // Тут ждём идентификатор
            ParentListEntry ple;
            ple.flags = inheritanceListMode==InheritanceListMode::uses ? ParentListEntryFlags::uses : ParentListEntryFlags::inherits;

            if (!checkExactTokenType({UFSM_TOKEN_OP_SCOPE, UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                return false;

            ple.positionInfo = getFullPos();

            // У нас - идентификатор или оператор скоупа, парсим полное имя
            if (!parseFullQualifiedName(ple.name))
            {
                // Ошибку вроде вывели в parseFullQualifiedName
                //expectedReachedMsg(m_pTokenInfo, {UFSM_TOKEN_OP_SCOPE, UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , msg */ );
                return false;
            }

            // После имени может идти override, а после override - опционально идёт блок с атрибутами
            if (isTokenOneOf(UFSM_TOKEN_KWD_OVERRIDE))
            {
                readNextToken(); 

                if (!isTokenOneOf(UFSM_TOKEN_OP_COLON)) // override без блока - это all
                {
                    ple.overrideFlags = InheritanceOverrideFlags::all;
                }
                else
                {
                    readNextToken(); 
                    if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                        return false; // а пришло хз

                    if (!parseInheritanceOverrideFlags(ple.overrideFlags))
                        return false;

                    if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_CLOSE} /* , "'display-options' directive: invalid option value" */ ))
                        return false;
                    readNextToken(); 
                }

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
            if (isTokenOneOf(UFSM_TOKEN_OP_COMMA))
            {
                readNextToken(); 
                continue;
            }

            if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_OPEN))
            {
                return true;
            }

            if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
            {
                readNextToken(); 
                if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL, } /* , "'display-options' directive: invalid option value" */ ))
                    return false;

                sm.description = extractLiteral();
                readNextToken(); 
            }

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

        if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
            return false;

        sm.name = extractIdentifierName();

        readNextToken(); 
        if (isTokenOneOf(UFSM_TOKEN_OP_COLON))
        {
            if (!parseStateMachineInheritanceList(sm, inheritanceListMode))
                return false;
        }
        // else 
        
        if (isTokenOneOf(UFSM_TOKEN_OP_DESCR_FOLLOWS))
        {
            readNextToken(); 
            if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_STRING_LITERAL, } /* , "'display-options' directive: invalid option value" */ ))
                return false;

            sm.description = extractLiteral();
            readNextToken(); 
        }

        // Тут у нас должна быть открывающая блок скобка
        if (!checkExactTokenType({UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
            return false;

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
                if (!checkExactTokenType({UMBA_TOKENIZER_TOKEN_IDENTIFIER} /* , "'display-options' directive: invalid option value" */ ))
                    return false;

                nsName.append(extractIdentifierName());
                waitSep = true;
                continue;
            }
            else // wait for name sep or open curly
            {
                if (!checkExactTokenType({UFSM_TOKEN_OP_SCOPE, UFSM_TOKEN_BRACKET_SCOPE_OPEN} /* , "'display-options' directive: invalid option value" */ ))
                    return false;

                if (isTokenOneOf(UFSM_TOKEN_OP_SCOPE)) // ::
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

            if (isTokenOneOf(UMBA_TOKENIZER_TOKEN_LINEFEED))
                continue;

            if (isTokenOneOf(UMBA_TOKENIZER_TOKEN_CTRL_FIN))
                return true; // normal stop

            if (isTokenOneOf(UFSM_TOKEN_KWD_NAMESPACE))
            {
                ctx.lastNsPos = getFullPos();
                if (!parseNamespace())
                    return false;
            }
            else if (isTokenOneOf(UFSM_TOKEN_KWD_FSM))
            {
                if (!parseStateMachine())
                    return false;
            }
            else if (isTokenOneOf(UFSM_TOKEN_KWD_DEFINITIONS))
            {
                if (!parseDefinitions())
                    return false;
            }
            else if (isTokenOneOf(UFSM_TOKEN_BRACKET_SCOPE_CLOSE))
            {
                std::size_t nsPopCount = 0;
                if (!ctx.nsOpenLevels.empty())
                {
                    nsPopCount = ctx.nsOpenLevels.back();
                    ctx.nsOpenLevels.pop_back();
                }
                else
                {
                    BaseClass::logSimpleUnexpected();
                    return false;
                }

                if (nsPopCount>ctx.curNsName.size())
                {
                    throw std::runtime_error("Too many closing curly bracket to pop");
                }

                ctx.curNsName.removeTail(nsPopCount);

            }

            else
            {
                expectedReachedMsg({ UFSM_TOKEN_KWD_NAMESPACE, UFSM_TOKEN_KWD_FSM, UFSM_TOKEN_KWD_DEFINITIONS } /* , msg */ );
                return false;
            }

            //if (!m_pTokenInfo)
            //    return false;
    
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
            return logMessage( "generic-error", e.what() ), false;
        }
        catch(...)
        {
            return logMessage( "generic-error", "unknown error" ), false;
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

