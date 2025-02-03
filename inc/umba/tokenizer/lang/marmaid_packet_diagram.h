#pragma once

#include "umba/umba.h"
//
#include "umba/tokenizer.h"
#include "umba/tokenizer/token_filters.h"
#include "umba/tokenizer/string_literal_parsing.h"
#include "umba/string_plus.h"

//
#include "marmaid_packet_diagram_tokens.h"

//
#include <unordered_map>

// umba::tokenizer::
namespace umba {
namespace tokenizer {


template< typename CharType                     //!< Input chars type
        , typename UserDataType        = void*  //!< Пользовательские данные
        , typename CharClassTableType  = std::array<umba::tokenizer::CharClass, 128> //!< Таблица классов символов
        , typename TrieVectorType      = std::vector<umba::tokenizer::TrieNode>      //!< Тип Trie
        , typename StringType          = std::basic_string<CharType>  //!< Тип строки, которая хранит входные символы
        , typename MessagesStringType  = std::string  //!< Тип строки, которая используется для сообщений (в том числе и от внешних суб-парсеров)
        , typename InputIteratorType   = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits = umba::iterator::TextIteratorTraits<InputIteratorType>
        , typename TokenizerType       = umba::tokenizer::Tokenizer< CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType >
        >
umba::tokenizer::TokenizerBuilder<CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits, TokenizerType>
makeTokenizerBuilderMarmaidPacketDiagram()
{
    using CppStringLiteralParser     = CppEscapedSimpleQuotedStringLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;
    using AngleBracketsLiteralParser = SimpleQuotedStringLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;

    using umba::string_plus::make_string;

    auto tokenizerBuilder = TokenizerBuilder<CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits, TokenizerType>()

                          .generateStandardCharClassTable()

                          .addBrackets(make_string<StringType>("[]"), UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS)

                          .addSingleLineComment(make_string<StringType>("%%"), UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST)

                          .addNumbersPrefix(make_string<StringType>("0b"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN)
                          .addNumbersPrefix(make_string<StringType>("0B"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0d"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC)
                          .addNumbersPrefix(make_string<StringType>("0D"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0") , UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT)

                          .addNumbersPrefix(make_string<StringType>("0x"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX)
                          .addNumbersPrefix(make_string<StringType>("0X"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX, true) // allowUseExistingToken


                          // .addSingleLineComment(make_string<StringType>("#"), UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST)

                          .addOperator(make_string<StringType>("-"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION                   )
                          .addOperator(make_string<StringType>(":"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE           )
                          //.addOperator( )

                          .template addStringLiteralParser<CppStringLiteralParser>( UMBA_TOKENIZER_TOKEN_STRING_LITERAL
                                                                                  , { make_string<StringType>("\"")   // UMBA_TOKENIZER_TOKEN_STRING_LITERAL itself
                                                                                    //, make_string<StringType>("\'")   // UMBA_TOKENIZER_TOKEN_CHAR_LITERAL (UMBA_TOKENIZER_TOKEN_STRING_LITERAL+1)
                                                                                    }
                                                                                  )
                          //.template addStringLiteralParser< AngleBracketsLiteralParser >(UMBA_TOKENIZER_TOKEN_ANGLE_BACKETS_STRING_LITERAL, { make_string<StringType>("<") } )

                          ;

    return tokenizerBuilder;

}


struct MarmaidPacketDiagramTokenizerConfigurator
{
    template<typename TokenizerType>
    TokenizerType operator()(TokenizerType tokenizer)
    {
        //using tokenizer_type = typename TokenizerBuilder::tokenizer_type;
        using IdentifierToKeywordConversionFilter = umba::tokenizer::filters::IdentifierToKeywordConversionFilter<TokenizerType>;
        using string_type    = typename TokenizerType::string_type;

        auto options = tokenizer.getOptions();
        options.unclassifiedCharsRaw = false;
        tokenizer.setOptions(options);

        // !!! Фильтры, установленные позже, отрабатывают раньше

        tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                                                  , std::unordered_map<string_type, umba::tokenizer::payload_type>
    
                                                                                    { {"char"            , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR    }
                                                                                    , {"int8"            , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8    }
                                                                                    , {"int16"           , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16   }
                                                                                    , {"int32"           , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32   }
                                                                                    , {"int64"           , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64   }
                                                                                    , {"uint8"           , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8   }
                                                                                    , {"uint16"          , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16  }
                                                                                    , {"uint32"          , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32  }
                                                                                    , {"uint64"          , MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64  }
    
                                                                                    //, {""          ,        }
                                                                                    }
                                                                                  );

        return tokenizer;
    }
};


template<typename TokenizerBuilder, typename TokenHandler>
//typename TokenizerBuilder::tokenizer_type makeTokenizerCpp(const TokenizerBuilder &builder, TokenHandler tokenHandler, bool suffixGluing=true, bool preprocessorFilter=true)
typename TokenizerBuilder::tokenizer_type makeTokenizerMarmaidPacketDiagram(TokenizerBuilder builder, TokenHandler tokenHandler, bool suffixGluing=true, bool preprocessorFilter=true)
{
    using TokenizerType = typename TokenizerBuilder::tokenizer_type;
    auto tokenizer = builder.makeTokenizer();
    tokenizer.tokenHandler = tokenHandler;

    return MarmaidPacketDiagramTokenizerConfigurator()(tokenizer);
}


} // namespace tokenizer
} // namespace umba

