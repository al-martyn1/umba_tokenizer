/*! \file
    \brief Базовый парсер V2
 */

#include "parser_base.h"

//
#include "umba/string_plus.h"
//
#include "umba/tokenizer/tokenizer_log.h"
#include "umba/tokenizer/tokenizer_log_console.h"
#include "umba/tokenizer/token_collection.h"
#include "marty_expressions/tokenizer_helpers.h"
//
#include "umba/enum_helpers.h"
#include "umba/flag_helpers.h"
#include "umba/the.h"

//
#include <stdexcept>
#include <initializer_list>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// umba::tokenizer::
namespace umba {
namespace tokenizer {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType, typename FilenameSetUserDataType=void*>
class ParserBase2 : public umba::tokenizer::ParserBase<TokenizerType, FilenameSetUserDataType>
{

public: // types

    using BaseClass                = umba::tokenizer::ParserBase<TokenizerType, FilenameSetUserDataType>;
    using tokenizer_type           = TokenizerType                                  ;
    using TokenCollectionType      = typename BaseClass::TokenCollectionType        ;
    using TokenCollectionItemType  = typename BaseClass::TokenCollectionItemType    ;
    using TokenInfoType            = typename BaseClass::TokenInfoType              ;
    using TokenPosType             = typename BaseClass::TokenPosType               ;
    using token_parsed_data_type   = typename tokenizer_type::token_parsed_data_type;
    using shared_log_type          = umba::tokenizer::log::SharedParserErrorLog     ;
    using string_type              = typename BaseClass::string_type                ;
    using FilenameSetType          = typename BaseClass::FilenameSetType            ;
    using SharedFilenameSetType    = typename BaseClass::SharedFilenameSetType      ;
    using filename_set_type        = typename BaseClass::filename_set_type          ;
    using shared_filename_set_type = typename BaseClass::shared_filename_set_type   ;
    using payload_type             = umba::tokenizer::payload_type                  ;


protected: // members

    shared_log_type                log    ;
    const TokenInfoType            *m_pTokenInfo = 0;
    TokenPosType                   m_tokenPos;


public: // ctors & assignment

    UMBA_RULE_OF_FIVE_COPY_DELETE(ParserBase2);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(ParserBase2);

    ParserBase2() {}

    explicit
    ParserBase2(std::shared_ptr<TokenCollectionType> tc, SharedFilenameSetType pFilenameSet, shared_log_type a_log)
    : BaseClass(tc, pFilenameSet)
    , log(a_log)
    {
    }

    virtual
    ~ParserBase2() {}


public: // info & pos methods

    virtual
    string_type getTokenIdStr(payload_type  /* tk */ ) const
    {
        return string_type();
    }

    FullTokenPosition getFullPos(TokenPosType pos) const  { return BaseClass::getFullTokenPosition(pos); }
    FullTokenPosition getFullPos() const                  { return getFullPos(m_tokenPos); }



public: // parsing basic methods

    payload_type getTokenType() const
    {
        UMBA_ASSERT(m_pTokenInfo);
        return m_pTokenInfo->tokenType;
    }

    template <typename... TArgs>
    bool isTokenOneOf(TArgs&&... args) const
    {
        return ((getTokenType() == args) || ...);
    }

    const token_parsed_data_type* getTokenParsedDataPtr() const
    {
        return BaseClass::getTokenParsedDataPtr(m_pTokenInfo);
    }

    token_parsed_data_type getTokenParsedData() const
    {
        return BaseClass::getTokenParsedData(m_pTokenInfo);
    }


    void readNextToken(ParserWaitForTokenFlags readFlags=ParserWaitForTokenFlags::none)
    {
        m_pTokenInfo = BaseClass::waitForSignificantTokenChecked( &m_tokenPos, readFlags);
    }

    string_type extractIdentifierName() const
    {
        auto pTokenParsedData = BaseClass::getTokenParsedDataPtr(m_pTokenInfo);
        auto identifierData = std::get<typename TokenizerType::IdentifierDataHolder>(*pTokenParsedData);
        return identifierData.pData->value;
    }

    string_type extractLiteral() const
    {
        auto pTokenParsedData = BaseClass::getTokenParsedDataPtr(m_pTokenInfo);
        auto literalData = std::get<typename TokenizerType::StringLiteralDataHolder>(*pTokenParsedData);
        return literalData.pData->value;
    }


public: // log & check methods

    bool expectedReachedMsg(std::initializer_list<payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        BaseClass::logUnexpected( m_pTokenInfo, payloadExpectedList, msg, [&](payload_type tk) { return getTokenIdStr(tk); } );
        return false;
    }

    bool expectedReachedMsg(payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        BaseClass::logUnexpected( m_pTokenInfo, payloadExpected, msg, [&](payload_type tk) { return getTokenIdStr(tk); } );
        return false;
    }

    bool checkExactTokenType(std::initializer_list<payload_type> payloadExpectedList, const std::string &msg=std::string()) const
    {
        for(auto e : payloadExpectedList)
        {
            if (e==m_pTokenInfo->tokenType) // tkReached
                return true;
        }
        return expectedReachedMsg(payloadExpectedList, msg);
    }

    bool checkExactTokenType(payload_type payloadExpected, const std::string &msg=std::string()) const
    {
        if (payloadExpected==m_pTokenInfo->tokenType)
            return true;

        return expectedReachedMsg(payloadExpected, msg);
    }

    void logSimpleMessage(const std::string &msgId, const std::string &msg) const
    {
        BaseClass::logSimpleMessage(m_pTokenInfo, msgId, msg);
    }

    void logSimpleMessage(TokenPosType pos, const std::string &msgId, const std::string &msg) const
    {
        BaseClass::logSimpleMessage(pos, m_pTokenInfo->tokenType, msgId, msg);
    }

    void logSimpleMessage(TokenPosType pos, payload_type tokenType, const std::string &msgId, const std::string &msg) const
    {
        BaseClass::logSimpleMessage(pos, tokenType, msgId, msg);
    }

    void logSimpleUnexpected(payload_type payloadUnexpected) const
    {
        BaseClass::logSimpleUnexpected(m_pTokenInfo, payloadUnexpected, [&](payload_type tk) { return getTokenIdStr(tk); });
    }

    void logSimpleUnexpected() const
    {
        logSimpleUnexpected(m_pTokenInfo->tokenType);
    }

    void logMessage(const std::string &msgId, const std::string &msg, const typename umba::FormatMessage<std::string>::macros_map_type &args )
    {
        BaseClass::logMessage(m_pTokenInfo, msgId, msg, args);
    }

    void logMessage(const std::string &msgId, const std::string &msg)
    {
        logSimpleMessage(msgId, msg);
    }

    // void logMessage(const TokenCollectionItemType *pTokenInfo, const std::string &msgId, const std::string &msg, const typename umba::FormatMessage<std::string>::macros_map_type &args )
    // {
    //     m_pTokens->getLog()->formatArgs = args;
    //  
    //     TextPositionInfo textPosInfo = getTokenPositionInfo(pTokenInfo);
    //     // std::string erroneousLineText = m_pTokens->getTokenTextLine(textPosInfo);
    //  
    //     m_pTokens->getLog()->logErrorEvent( umba::tokenizer::log::ParserErrorEventType::customError
    //                                       , textPosInfo
    //                                       , pTokenInfo->tokenType
    //                                       , std::string() // unexpectedTokenValue  // erroneousValue
    //                                       , std::string() // erroneousLineText // std::string() // erroneousLineText - пока не ищем
    //                                       , msgId, msg
    //                                       , 0, 0
    //                                       );
    // }
    //  
    // void logMessage(const TokenCollectionItemType *pTokenInfo, const std::string &msgId, const std::string &msg) const




public: // parsing helper methods

    template<typename TokenHandler>
    bool readHomogeneousTokensList( payload_type tokenToken, payload_type tokenSep
                                  , bool readNextOnStart
                                  , TokenHandler handler
                                  , bool initialWaitSep = false
                                  )
    {
        if (readNextOnStart)
            readNextToken();

        bool waitSep = initialWaitSep;

        for( 
           ; isTokenOneOf(tokenSep, tokenToken)
           ; readNextToken()
           )
        {
            if (waitSep)
            {
                if (getTokenType()!=tokenSep)
                    break;
                waitSep = false;
            }
            else // ждём tokenToken, после разделителя или в начале
            {
                if (!checkExactTokenType(tokenToken /* , "'display-options' directive: invalid option value" */ ))
                    return false; // а пришло хз что
                //evd.basicEvents.emplace_back(extractIdentifierName(m_pTokenInfo));
                handler();
                waitSep = true;
            }
        }

        return true;
    }



}; // class ParserBase2

//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba
// umba::tokenizer::


