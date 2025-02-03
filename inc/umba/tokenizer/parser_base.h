#pragma once

#include "token_collection.h"
#include "enums.h"
//
#include "umba/the.h"

//
#include <memory>

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


