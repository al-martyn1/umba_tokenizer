/*! \file
    \brief Базовый парсер
 */

#pragma once

#include "umba/warnings/push_disable_rel_inc_contains_dbldot.h"
#include "../token_collection.h"
#include "../enums.h"
#include "umba/warnings/pop.h"
//
#include "umba/the.h"
#include "umba/format_message.h"

//
#include <memory>
#include <initializer_list>

//----------------------------------------------------------------------------
// umba::tokenizer::
namespace umba {
namespace tokenizer {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename TokenizerType>
class ParserBase
{

public: // types & ctors

    using tokenizer_type           = TokenizerType;
    using TokenCollectionType      = TokenCollection<TokenizerType>;
    using TokenCollectionItemType  = typename TokenCollectionType::TokenCollectionItemType;
    using TokenInfoType            = TokenCollectionItemType;
    using TokenPosType             = typename TokenCollectionType::token_pos_type;
    using token_parsed_data_type   = typename tokenizer_type::token_parsed_data_type;
    using file_id_type             = umba::TextPositionInfo::file_id_type;
    using string_type              = typename tokenizer_type::string_type; // Input chars string type
    using char_type                = typename string_type::value_type;


    UMBA_RULE_OF_FIVE_COPY_DELETE(ParserBase);
    UMBA_RULE_OF_FIVE_MOVE_DEFAULT(ParserBase);

    ParserBase() {}

    // ParserBase(ParserBase &&pb)
    // : m_tokens(std::exchange(pb.m_tokens, TokenCollectionType()))
    // {
    // }
    //  
    // ParserBase& operator=(ParserBase &&pb)
    // {
    //     m_tokens = std::exchange(pb.m_tokens, TokenCollectionType());
    //     return *this;
    // }

    explicit
    ParserBase(std::shared_ptr<TokenCollectionType> tc)
    : m_pTokens(tc)
    {
    }

    std::size_t getNumFetchedTokens() const       { return m_pTokens->getNumFetchedTokens(); }
    std::size_t getNumberOfTokensTotal() const    { return m_pTokens->getNumberOfTokensTotal(); }
    std::size_t getNumberOfTokenDataTotal() const { return m_pTokens->getNumberOfTokenDataTotal(); } 
    std::size_t getBytesOfTokensTotal() const     { return m_pTokens->getBytesOfTokensTotal(); } 
    std::size_t getBytesOfTokenDataTotal() const  { return m_pTokens->getBytesOfTokenDataTotal(); } 


public: // methods
//protected: // methods

    file_id_type getFileId() const
    {
        return m_pTokens->getFileId();
    }

    const token_parsed_data_type* getTokenParsedData(const TokenCollectionItemType *ptki) const
    {
        return m_pTokens->getTokenParsedData(ptki);
    }

    TextPositionInfo getTokenPositionInfo(const TokenCollectionItemType *ptki) const
    {
        return m_pTokens->getTokenPositionInfo(ptki);
    }

    const char_type* getTextPointer(const TokenCollectionItemType *ptki) const
    {
        return m_pTokens->getTextPointer(ptki);
    }


    void logSimpleMessage(const TokenCollectionItemType *pTokenInfo, const std::string &msgId, const std::string &msg) const
    {
        TextPositionInfo textPosInfo = getTokenPositionInfo(pTokenInfo);
        std::string erroneousLineText = m_pTokens->getTokenTextLine(textPosInfo);

        m_pTokens->getLog()->logErrorEvent( umba::tokenizer::log::ParserErrorEventType::customError
                                          , textPosInfo
                                          , pTokenInfo->tokenType
                                          , std::string() // unexpectedTokenValue  // erroneousValue
                                          , erroneousLineText // std::string() // erroneousLineText - пока не ищем
                                          , msgId, msg
                                          , 0, 0
                                          );
    }


    template<typename KindStringGetter>
    void logUnexpected( const TokenCollectionItemType *pTokenInfo
                      , umba::tokenizer::payload_type payloadExpected
                      , const std::string &prefixMsg
                      , KindStringGetter kindStringGetter
                      ) const
    {
        logUnexpected(pTokenInfo, {payloadExpected}, prefixMsg, kindStringGetter);
    }


    template<typename KindStringGetter>
    void logUnexpected( const TokenCollectionItemType *pTokenInfo
                      , std::initializer_list<umba::tokenizer::payload_type> payloadExpectedList
                      , const std::string &prefixMsg
                      , KindStringGetter kindStringGetter
                      ) const
    {
        //std::string expectedTokenKind   = kindStringGetter(payloadExpected);
        std::string unexpectedTokenKind = kindStringGetter(pTokenInfo->tokenType);
        std::string msgId = "unexpected-" + unexpectedTokenKind;

        auto formatMessage = FormatMessage<std::string>();
        formatMessage.arg("UnexpectedTokenKind", unexpectedTokenKind);

        std::string expectedFormatStr;
        std::size_t plIdx = 1;
        for(auto e : payloadExpectedList)
        {
            std::string curName = "ExpectedTokenKind" + std::to_string(plIdx);
            std::string curKind = kindStringGetter(e);

            if (expectedFormatStr.empty())
            {
                expectedFormatStr = "'$(" + curName + ")'";
            }
            else
            {
                if (plIdx!=payloadExpectedList.size())
                {
                    expectedFormatStr += ", '$(" + curName + ")'";
                }
                else
                {
                    expectedFormatStr += " or '$(" + curName + ")'";
                }
            }

            formatMessage.arg(curName, curKind);

            ++plIdx;
        }


        std::string msg = prefixMsg.empty()
                        ? "expected " + expectedFormatStr + ", but got '$(UnexpectedTokenKind)'" 
                        : prefixMsg + ". Expected " + expectedFormatStr + ", but got '$(UnexpectedTokenKind)'"
                        ;

        std::string unexpectedTokenValue;

        // Ключевые слова получаются из идентификаторов, просто обретают новый ID
        // Полезная нагрузка у них остаётся того же типа
        if ( pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER
          || (pTokenInfo->tokenType>=UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST && pTokenInfo->tokenType<=UMBA_TOKENIZER_TOKEN_KEYWORD_SET8_LAST)
           )
        {
            const token_parsed_data_type* pParsedData = getTokenParsedData(pTokenInfo);
            auto identifierData = std::get<typename tokenizer_type::IdentifierDataHolder>(*pParsedData);
            unexpectedTokenValue = identifierData.pData->value;
        }

        if (!unexpectedTokenValue.empty())
        {
            msg += " ('$(Value)')";
        }

        // msg += "";

        TextPositionInfo textPosInfo = getTokenPositionInfo(pTokenInfo);
        std::string erroneousLineText = m_pTokens->getTokenTextLine(textPosInfo);

        m_pTokens->getLog()->formatArgs = formatMessage.values();

        m_pTokens->getLog()->logErrorEvent( umba::tokenizer::log::ParserErrorEventType::customError
                                          , textPosInfo
                                          , pTokenInfo->tokenType
                                          , unexpectedTokenValue  // erroneousValue
                                          , erroneousLineText // std::string() // erroneousLineText - пока не ищем
                                          , msgId, msg
                                          , 0, 0
                                          );

    }

    void logMessage(const TokenCollectionItemType *pTokenInfo, const std::string &msgId, const std::string &msg, const typename umba::FormatMessage<std::string>::macros_map_type &args )
    {
        m_pTokens->getLog()->formatArgs = args;

        TextPositionInfo textPosInfo = getTokenPositionInfo(pTokenInfo);
        // std::string erroneousLineText = m_pTokens->getTokenTextLine(textPosInfo);
     
        m_pTokens->getLog()->logErrorEvent( umba::tokenizer::log::ParserErrorEventType::customError
                                          , textPosInfo
                                          , pTokenInfo->tokenType
                                          , std::string() // unexpectedTokenValue  // erroneousValue
                                          , std::string() // erroneousLineText // std::string() // erroneousLineText - пока не ищем
                                          , msgId, msg
                                          , 0, 0
                                          );
    }

    void logMessage(const TokenCollectionItemType *pTokenInfo, const std::string &msgId, const std::string &msg) const
    {
        logSimpleMessage(pTokenInfo, msgId, msg);
    }

    const TokenInfoType* getToken(TokenPosType *pTokenPos /* =0 */ )
    {
        return m_pTokens->getToken(pTokenPos);
    }

    const TokenInfoType* waitForSignificantToken(TokenPosType *pTokenPos=0, ParserWaitForTokenFlags waitFlags=ParserWaitForTokenFlags::stopOnAny)
    {
        const TokenInfoType* pTokenInfo = m_pTokens->getToken(pTokenPos);

        const auto theFlags = TheFlags(waitFlags);

        while(pTokenInfo)
        {
            const auto tokenType = umba::TheValue(pTokenInfo->tokenType);

            //if (tokenType==UMBA_TOKENIZER_TOKEN_CTRL_FIN)

            if ( tokenType.template oneOf< UMBA_TOKENIZER_TOKEN_SPACE, UMBA_TOKENIZER_TOKEN_TAB>()
              && !theFlags.template oneOf<ParserWaitForTokenFlags::stopOnSpace>()
               )
            {
                pTokenInfo = m_pTokens->getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            if ( tokenType.template oneOf< UMBA_TOKENIZER_TOKEN_LINEFEED, UMBA_TOKENIZER_TOKEN_FORM_FEED>()
              && !theFlags.template oneOf<ParserWaitForTokenFlags::stopOnLinefeed>()
               )
            {
                pTokenInfo = m_pTokens->getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            if ( tokenType.template inClosedRange< UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST, UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST >()
              && !theFlags.template oneOf<ParserWaitForTokenFlags::stopOnSingleLineComment>()
               )
            {
                pTokenInfo = m_pTokens->getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            if ( tokenType.template inOpenRange< UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START, UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END >()
              && !theFlags.template oneOf<ParserWaitForTokenFlags::stopOnMultiLineComment>()
               )
            {
                pTokenInfo = m_pTokens->getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            // У нас не что-то пробельное и не комент, возвращаем то, что есть

            return pTokenInfo;
        
        }
        
        return pTokenInfo;

    }

    tokenizer_type& getTokenizer()
    {
        return m_pTokens->getTokenizer();
    }

    void clearFetched()
    {
        m_pTokens->clearFetched();
    }

    void setRawModeAutoStop(TokenizerRawAutoStopMode newMode, const string_type &stopChars=string_type())
    {
        getTokenizer().setRawModeAutoStop(newMode, stopChars);
    }

    bool setRawMode(bool bRawMode)
    {
        if (bRawMode)
            clearFetched();
        return getTokenizer().setRawMode(bRawMode);
    }

        


protected: // fields

    std::shared_ptr<TokenCollectionType>  m_pTokens;


}; // class ParserBase






//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba


