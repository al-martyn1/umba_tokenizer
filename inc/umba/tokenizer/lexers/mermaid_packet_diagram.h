/*! \file
    \brief Лексер (сканер) диаграмм mermaid packet diagram
 */
#pragma once

#include "umba/umba.h"
//
#include "umba/tokenizer.h"
#include "umba/tokenizer/token_filters.h"
#include "umba/tokenizer/string_literal_parsing.h"
#include "umba/string_plus.h"

//
#include "mermaid_packet_diagram_tokens.h"

//
#include <unordered_map>

// umba::tokenizer::mermaid::
namespace umba {
namespace tokenizer {
namespace mermaid {
namespace packet_diagram {


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
makeTokenizerBuilder()
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

                          .addOperator(make_string<StringType>("-"   ), MERMAID_TOKEN_OPERATOR_RANGE             )
                          //.addOperator(make_string<StringType>(":"   ), MERMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER  )
                          //.addOperator(make_string<StringType>("%%#!"), MERMAID_TOKEN_OPERATOR_EXTRA             ) // Как комент, но специальный. Игнорится оригинальным mermaid'ом, но обрабатывается нами
                          .addOperator(make_string<StringType>("+"   ), MERMAID_TOKEN_OPERATOR_PLUS              )
                          
                          
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


struct TokenizerConfigurator
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
    
                                                                                    { {"char"                      , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR    }
                                                                                    , {"int8"                      , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8    }
                                                                                    , {"int16"                     , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16   }
                                                                                    , {"int32"                     , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32   }
                                                                                    , {"int64"                     , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64   }
                                                                                    , {"uint8"                     , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8   }
                                                                                    , {"uint16"                    , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16  }
                                                                                    , {"uint32"                    , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32  }
                                                                                    , {"uint64"                    , MERMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64  }

                                                                                    , {"packet-beta"                , MERMAID_TOKEN_DIRECTIVE_PACKET_BETA      }
                                                                                    , {"title"                      , MERMAID_TOKEN_DIRECTIVE_TITLE            }
                                                                                    , {"native"                     , MERMAID_TOKEN_DIRECTIVE_NATIVE           }
                                                                                    , {"display-width"              , MERMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH    }
                                                                                    , {"display-options"            , MERMAID_TOKEN_DIRECTIVE_DISPLAY_OPTIONS  }
                                                                                    , {"org"                        , MERMAID_TOKEN_DIRECTIVE_ORG              }

                                                                                    , {"byte-diagram"               , MERMAID_TOKEN_ATTR_BYTE_DIA              }
                                                                                    , {"bit-diagram"                , MERMAID_TOKEN_ATTR_BIT_DIA               }
                                                                                    , {"memory-diagram"             , MERMAID_TOKEN_ATTR_MEMORY_DIA            }
                                                                                    , {"memory-layout"              , MERMAID_TOKEN_ATTR_MEMORY_DIA            }
                                                                                    , {"memory"                     , MERMAID_TOKEN_ATTR_MEMORY_DIA            }

                                                                                    , {"ascii-z"                    , MERMAID_TOKEN_ATTR_ASCII_Z               }
                                                                                    , {"byte"                       , MERMAID_TOKEN_ATTR_BYTE                  }
                                                                                    , {"bytes"                      , MERMAID_TOKEN_ATTR_BYTE                  }
                                                                                    , {"bit"                        , MERMAID_TOKEN_ATTR_BIT                   }
                                                                                    , {"bits"                       , MERMAID_TOKEN_ATTR_BIT                   }
                                                                                    , {"little-endian"              , MERMAID_TOKEN_ATTR_LE                    }
                                                                                    , {"le"                         , MERMAID_TOKEN_ATTR_LE                    }
                                                                                    , {"big-endian"                 , MERMAID_TOKEN_ATTR_BE                    }
                                                                                    , {"be"                         , MERMAID_TOKEN_ATTR_BE                    }
                                                                                    , {"middle-endian"              , MERMAID_TOKEN_ATTR_ME                    }
                                                                                    , {"me"                         , MERMAID_TOKEN_ATTR_ME                    }
                                                                                    , {"little-endian-middle-endian", MERMAID_TOKEN_ATTR_LE_ME                 }
                                                                                    , {"le-me"                      , MERMAID_TOKEN_ATTR_LE_ME                 }
                                                                                    , {"big-endian-middle-endian"   , MERMAID_TOKEN_ATTR_BE_ME                 }
                                                                                    , {"be-me"                      , MERMAID_TOKEN_ATTR_BE_ME                 }
                                                                                    , {"checksum"                   , MERMAID_TOKEN_ATTR_CHECKSUM              }
                                                                                    , {"simple-sum"                 , MERMAID_TOKEN_ATTR_SIMPLE_SUM            }
                                                                                    , {"simple-sum-complement"      , MERMAID_TOKEN_ATTR_SIMPLE_SUM_COMPLEMENT }
                                                                                    , {"simple-sum-invert"          , MERMAID_TOKEN_ATTR_SIMPLE_SUM            }
                                                                                    , {"simple-xor"                 , MERMAID_TOKEN_ATTR_SIMPLE_XOR            }
                                                                                    , {"simple-xor-complement"      , MERMAID_TOKEN_ATTR_SIMPLE_XOR_COMPLEMENT }
                                                                                    , {"simple-xor-invert"          , MERMAID_TOKEN_ATTR_SIMPLE_XOR_INVERT     }
                                                                                    , {"crc"                        , MERMAID_TOKEN_ATTR_CRC                   }
                                                                                    , {"seed"                       , MERMAID_TOKEN_ATTR_SEED                  }
                                                                                    , {"poly"                       , MERMAID_TOKEN_ATTR_POLY                  }
                                                                                    , {"auto"                       , MERMAID_TOKEN_ATTR_AUTO                  }
                                                                                    , {"segment"                    , MERMAID_TOKEN_ATTR_SEGMENT               }
                                                                                    , {"segment-shift"              , MERMAID_TOKEN_ATTR_SEGMENT_SHIFT         }
                                                                                    , {"offset"                     , MERMAID_TOKEN_ATTR_OFFSET                }
                                                                                    , {"data"                       , MERMAID_TOKEN_ATTR_DATA                  }
                                                                                    , {"block"                      , MERMAID_TOKEN_ATTR_BLOCK                 }

                                                                                    //, {""           ,              }
                                                                                    //, {""           ,              }


                                                                                    //, {""          ,        }
                                                                                    }
                                                                                  , false // not case sensitive
                                                                                  );

        tokenizer.template installTokenFilter<KebabCaseComposingFilter>(MERMAID_TOKEN_OPERATOR_RANGE);
        return tokenizer;
    }
};


template<typename TokenizerBuilder, typename TokenHandler>
//typename TokenizerBuilder::tokenizer_type makeTokenizerCpp(const TokenizerBuilder &builder, TokenHandler tokenHandler, bool suffixGluing=true, bool preprocessorFilter=true)
typename TokenizerBuilder::tokenizer_type makeTokenizer(TokenizerBuilder builder, TokenHandler tokenHandler /* , bool suffixGluing=true, bool preprocessorFilter=true */ )
{
    using TokenizerType = typename TokenizerBuilder::tokenizer_type;
    auto tokenizer = builder.makeTokenizer();
    tokenizer.tokenHandler = tokenHandler;

    return TokenizerConfigurator()(tokenizer);
}

} // namespace packet_diagram
} // namespace mermaid
} // namespace tokenizer
} // namespace umba

