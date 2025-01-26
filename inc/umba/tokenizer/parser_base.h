#pragma once

#include "token_collection.h"
#include "enums.h"
//
#include "umba/the.h"


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

    using TokenCollectionType      = TokenCollection<TokenizerType>;
    using TokenCollectionItemType  = typename TokenCollectionType::TokenCollectionItemType;
    using TokenInfoType            = TokenCollectionItemType;
    using TokenPosType             = typename TokenCollectionType::token_pos_type;


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
    ParserBase(TokenCollectionType tc)
    : m_tokens(std::move(tc))
    {
    }

    std::size_t getNumFetchedTokens() const       { return m_tokens.getNumFetchedTokens(); }
    std::size_t getNumberOfTokensTotal() const    { return m_tokens.getNumberOfTokensTotal(); }
    std::size_t getNumberOfTokenDataTotal() const { return m_tokens.getNumberOfTokenDataTotal(); } 
    std::size_t getBytesOfTokensTotal() const     { return m_tokens.getBytesOfTokensTotal(); } 
    std::size_t getBytesOfTokenDataTotal() const  { return m_tokens.getBytesOfTokenDataTotal(); } 


public: // methods
//protected: // methods

    const TokenInfoType* waitForSignificantToken(TokenPosType *pTokenPos=0, ParserWaitForTokenFlags waitFlags=ParserWaitForTokenFlags::stopOnAny)
    {
        const TokenInfoType* pTokenInfo = m_tokens.getToken(pTokenPos);

        const auto theFlags = TheFlags(waitFlags);

        while(pTokenInfo)
        {
            const auto tokenType = umba::TheValue(pTokenInfo->tokenType);

            if ( tokenType.oneOf< UMBA_TOKENIZER_TOKEN_SPACE, UMBA_TOKENIZER_TOKEN_TAB>()
              && !theFlags.oneOf<ParserWaitForTokenFlags::stopOnSpace>()
               )
            {
                pTokenInfo = m_tokens.getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            if ( tokenType.oneOf< UMBA_TOKENIZER_TOKEN_LINEFEED, UMBA_TOKENIZER_TOKEN_FORM_FEED>()
              && !theFlags.oneOf<ParserWaitForTokenFlags::stopOnLinefeed>()
               )
            {
                pTokenInfo = m_tokens.getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            if ( tokenType.inClosedRange< UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST, UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST >()
              && !theFlags.oneOf<ParserWaitForTokenFlags::stopOnSingleLineComment>()
               )
            {
                pTokenInfo = m_tokens.getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            if ( tokenType.inOpenRange< UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START, UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END >()
              && !theFlags.oneOf<ParserWaitForTokenFlags::stopOnMultiLineComment>()
               )
            {
                pTokenInfo = m_tokens.getToken(pTokenPos); // продолжаем поиск значащего токена
                continue;
            }

            // У нас не что-то пробельное и не комент, возвращаем то, что есть

            return pTokenInfo;
        
        }
        
        return pTokenInfo;

    }



protected: // fields

    TokenCollectionType   m_tokens;


}; // class ParserBase






//----------------------------------------------------------------------------

} // namespace tokenizer
} // namespace umba


