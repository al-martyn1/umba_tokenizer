#pragma once

#include "token_collection.h"
#include "enums.h"
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
                expectedFormatStr = "$(" + curName + ")";
            }
            else
            {
                if (plIdx!=payloadExpectedList.size())
                {
                    expectedFormatStr += ", $(" + curName + ")";
                }
                else
                {
                    expectedFormatStr += " or $(" + curName + ")";
                }
            }

            formatMessage.arg(curName, curKind);

            ++plIdx;
        }


        std::string msg = prefixMsg.empty()
                        ? "expected " + expectedFormatStr + ", but got $(UnexpectedTokenKind)" 
                        : prefixMsg + ". Expected " + expectedFormatStr + ", but got $(UnexpectedTokenKind)"
                        ;

        std::string unexpectedTokenValue;

        if (pTokenInfo->tokenType==UMBA_TOKENIZER_TOKEN_IDENTIFIER)
        {
            const token_parsed_data_type* pParsedData = getTokenParsedData(pTokenInfo);
            auto identifierData = std::get<typename tokenizer_type::IdentifierDataHolder>(*pParsedData);
            unexpectedTokenValue = identifierData.pData->value;
        }

        if (!unexpectedTokenValue.empty())
        {
            msg += " ('$(Value)')";
        }

        msg += ".";

        TextPositionInfo textPosInfo = getTokenPositionInfo(pTokenInfo);

        const auto &text = m_pTokens->getText();

        auto posIt = umba::iterator::TextPositionCountingIterator<char>(text.data(), text.size(), textPosInfo.fileId, textPosInfo.lineOffset+textPosInfo.symbolOffset);
        std::string erroneousLineText = umba::iterator::makeString(posIt.getLineStartIterator(), posIt.getLineEndIterator());

        m_pTokens->getLog()->formatArgs = formatMessage.values();

        m_pTokens->getLog()->logErrorEvent( umba::tokenizer::log::ParserErrorEventType::customError
                                        , textPosInfo
                                        , pTokenInfo->tokenType
                                        , unexpectedTokenValue  // erroneousValue
                                        , erroneousLineText // std::string() // erroneousLineText - пока не ищем
                                        , msgId, msg
                                        , 0, 0
                                        );

        // if (it!=itEnd)
        // {
        //     erroneousValue    = umba::iterator::makeString(it,itEnd);
        //     erroneousLineText = umba::iterator::makeString(it.getLineStartIterator(), it.getLineEndIterator());
        // }


        //TextPositionInfo getTokenPositionInfo(const TokenCollectionItemType *ptki) const

    }

    //LOG_ERR << msg << ". Expected " << getTokenIdStr(tkExpected1) << " or " << getTokenIdStr(tkExpected2) << ", but got " << getTokenIdStr(tkReached) << "\n";
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



    const TokenInfoType* waitForSignificantToken(TokenPosType *pTokenPos=0, ParserWaitForTokenFlags waitFlags=ParserWaitForTokenFlags::stopOnAny)
    {
        const TokenInfoType* pTokenInfo = m_pTokens->getToken(pTokenPos);

        const auto theFlags = TheFlags(waitFlags);

        while(pTokenInfo)
        {
            const auto tokenType = umba::TheValue(pTokenInfo->tokenType);

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



protected: // fields

    std::shared_ptr<TokenCollectionType>  m_pTokens;


}; // class ParserBase






//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba


