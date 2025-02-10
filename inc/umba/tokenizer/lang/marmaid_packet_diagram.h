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

// umba::tokenizer::marmaid::
namespace umba {
namespace tokenizer {
namespace marmaid {


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
makeTokenizerBuilderPacketDiagram()
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

                          .addOperator(make_string<StringType>("-"   ), MARMAID_TOKEN_OPERATOR_RANGE             )
                          .addOperator(make_string<StringType>(":"   ), MARMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER  )
                          .addOperator(make_string<StringType>("%%#!"), MARMAID_TOKEN_OPERATOR_EXTRA             ) // Как комент, но специальный. Игнорится оригинальным marmaid'ом, но обрабатывается нами
                          .addOperator(make_string<StringType>("+"   ), MARMAID_TOKEN_OPERATOR_PLUS              )
                          
                          
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


struct PacketDiagramTokenizerConfigurator
{
    template<typename TokenizerType>
    TokenizerType operator()(TokenizerType tokenizer)
    {
        //using tokenizer_type = typename TokenizerBuilder::tokenizer_type;
        using IdentifierToKeywordConversionFilter = umba::tokenizer::filters::IdentifierToKeywordConversionFilter<TokenizerType>;
        using KebabCaseComposingFilter = umba::tokenizer::filters::KebabCaseComposingFilter<TokenizerType>;
        
        using string_type    = typename TokenizerType::string_type;

        auto options = tokenizer.getOptions();
        options.allowUnclassifiedChars = false;
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

                                                                                    , {"packet-beta"     , MARMAID_TOKEN_DIRECTIVE_PACKET_BETA       }
                                                                                    , {"title"           , MARMAID_TOKEN_DIRECTIVE_TITLE             }
                                                                                    , {"native"          , MARMAID_TOKEN_DIRECTIVE_NATIVE            }
                                                                                    , {"display-width"   , MARMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH     }
                                                                                    , {"org"             , MARMAID_TOKEN_DIRECTIVE_ORG               }
                                                                                    

                                                                                    , {"byte-diagram"    , MARMAID_TOKEN_ATTR_BYTE_DIA               }
                                                                                    , {"bit-diagram"     , MARMAID_TOKEN_ATTR_BIT_DIA                }
                                                                                    , {"memory-diagram"  , MARMAID_TOKEN_ATTR_MEMORY_DIA             }
                                                                                    , {"memory-layout"   , MARMAID_TOKEN_ATTR_MEMORY_DIA             }
                                                                                    , {"memory"          , MARMAID_TOKEN_ATTR_MEMORY_DIA             }

                                                                                    , {"byte"            , MARMAID_TOKEN_ATTR_BYTE                   }
                                                                                    , {"bytes"           , MARMAID_TOKEN_ATTR_BYTE                   }
                                                                                    , {"bit"             , MARMAID_TOKEN_ATTR_BIT                    }
                                                                                    , {"bits"            , MARMAID_TOKEN_ATTR_BIT                    }
                                                                                    , {"little-endian"   , MARMAID_TOKEN_ATTR_LE                     }
                                                                                    , {"le"              , MARMAID_TOKEN_ATTR_LE                     }
                                                                                    , {"big-endian"      , MARMAID_TOKEN_ATTR_BE                     }
                                                                                    , {"be"              , MARMAID_TOKEN_ATTR_BE                     }
                                                                                    , {"middle-endian"   , MARMAID_TOKEN_ATTR_ME                     }
                                                                                    , {"me"              , MARMAID_TOKEN_ATTR_ME                     }
                                                                                    , {"little-endian-middle-endian", MARMAID_TOKEN_ATTR_LE_ME       }
                                                                                    , {"le-me"           , MARMAID_TOKEN_ATTR_LE_ME                  }
                                                                                    , {"big-endian-middle-endian", MARMAID_TOKEN_ATTR_BE_ME          }
                                                                                    , {"be-me"           , MARMAID_TOKEN_ATTR_BE_ME                  }
                                                                                    , {"crc"             , MARMAID_TOKEN_ATTR_CRC                    }
                                                                                    , {"seed"            , MARMAID_TOKEN_ATTR_SEED                   }
                                                                                    , {"poly"            , MARMAID_TOKEN_ATTR_POLY                   }
                                                                                    , {"auto"            , MARMAID_TOKEN_ATTR_AUTO                   }

                                                                                    //, {""           ,              }
                                                                                    //, {""           ,              }


                                                                                    //, {""          ,        }
                                                                                    }
                                                                                  , false // not case sensitive
                                                                                  );

        tokenizer.template installTokenFilter<KebabCaseComposingFilter>(MARMAID_TOKEN_OPERATOR_RANGE);
        return tokenizer;
    }
};


template<typename TokenizerBuilder, typename TokenHandler>
//typename TokenizerBuilder::tokenizer_type makeTokenizerCpp(const TokenizerBuilder &builder, TokenHandler tokenHandler, bool suffixGluing=true, bool preprocessorFilter=true)
typename TokenizerBuilder::tokenizer_type makeTokenizerPacketDiagram(TokenizerBuilder builder, TokenHandler tokenHandler, bool suffixGluing=true, bool preprocessorFilter=true)
{
    using TokenizerType = typename TokenizerBuilder::tokenizer_type;
    auto tokenizer = builder.makeTokenizer();
    tokenizer.tokenHandler = tokenHandler;

    return PacketDiagramTokenizerConfigurator()(tokenizer);
}

} // namespace marmaid
} // namespace tokenizer
} // namespace umba

