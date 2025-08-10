#pragma once

#include "umba/umba.h"
//
#include "umba/tokenizer.h"
#include "umba/tokenizer/token_filters.h"
#include "umba/tokenizer/string_literal_parsing.h"
#include "umba/string_plus.h"

//
#include "ufsm_tokens.h"

//
#include <unordered_map>

// umba::tokenizer::
namespace umba {
namespace tokenizer {
namespace ufsm {


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

                          //.addBrackets(make_string<StringType>("[]"), UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS)
                          .addBrackets(make_string<StringType>("{}"), UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS)
                          .addBrackets(make_string<StringType>("()"), UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS)


                          //.addSingleLineComment(make_string<StringType>("%%"), UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST)
                          .setMultiLineComment (make_string<StringType>("/*"), make_string<StringType>("*/"))
                          .addSingleLineComment(make_string<StringType>("//"), UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST)
                          .addSingleLineComment(make_string<StringType>("#" ), UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST+1)

                          #if 0
                          // Числа с префиксами не нужны
                          .addNumbersPrefix(make_string<StringType>("0b"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN)
                          .addNumbersPrefix(make_string<StringType>("0B"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0d"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC)
                          .addNumbersPrefix(make_string<StringType>("0D"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0") , UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT)

                          .addNumbersPrefix(make_string<StringType>("0x"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX)
                          .addNumbersPrefix(make_string<StringType>("0X"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX, true) // allowUseExistingToken
                          #endif

                          .addOperator(make_string<StringType>("-"   ), UFSM_TOKEN_OP_DESCR_FOLOWS)
                          .addOperator(make_string<StringType>("*"   ), UFSM_TOKEN_OP_ANY         )
                          .addOperator(make_string<StringType>("~"   ), UFSM_TOKEN_OP_NOT         )
                          .addOperator(make_string<StringType>("&"   ), UFSM_TOKEN_OP_AND         )
                          .addOperator(make_string<StringType>("|"   ), UFSM_TOKEN_OP_OR          )
                          .addOperator(make_string<StringType>("!"   ), UFSM_TOKEN_OP_NOT_ALTER   )
                          .addOperator(make_string<StringType>("&&"  ), UFSM_TOKEN_OP_AND_ALTER   )
                          .addOperator(make_string<StringType>("||"  ), UFSM_TOKEN_OP_OR_ALTER    )
                          .addOperator(make_string<StringType>("="   ), UFSM_TOKEN_OP_ASSIGN      )
                          .addOperator(make_string<StringType>(":"   ), UFSM_TOKEN_OP_COLON       )
                          .addOperator(make_string<StringType>(","   ), UFSM_TOKEN_OP_COMMA       )

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
        using SimpleReplaceFilter      = umba::tokenizer::filters::SimpleReplaceFilter<TokenizerType>;
        
        using string_type    = typename TokenizerType::string_type;

        auto options = tokenizer.getOptions();
        options.allowUnclassifiedChars = false;
        tokenizer.setOptions(options);

        // !!! Фильтры, установленные позже, отрабатывают раньше

        tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                                                  , std::unordered_map<string_type, umba::tokenizer::payload_type>
    
                                                                                    { {"namespace"       , UFSM_TOKEN_KWD_NAMESPACE    }
                                                                                    , {"fsm"             , UFSM_TOKEN_KWD_FSM          }
                                                                                    , {"state-machine"   , UFSM_TOKEN_KWD_FSM          }
                                                                                    , {"events"          , UFSM_TOKEN_KWD_EVENTS       }
                                                                                    , {"actions"         , UFSM_TOKEN_KWD_ACTIONS      }
                                                                                    , {"states"          , UFSM_TOKEN_KWD_STATES       }
                                                                                    , {"predicates"      , UFSM_TOKEN_KWD_PREDICATES   }
                                                                                    , {"transitions"     , UFSM_TOKEN_KWD_TRANSITIONS  }
                                                                                    , {"external"        , UFSM_TOKEN_KWD_EXTERNAL     }
                                                                                    , {"initial"         , UFSM_TOKEN_KWD_INITIAL      }
                                                                                    , {"final"           , UFSM_TOKEN_KWD_FINAL        }
                                                                                    //, {"valid"           , UFSM_TOKEN_KWD_VALID        }
                                                                                    //, {"for"             , UFSM_TOKEN_KWD_FOR          }
                                                                                    , {"valid-for"       , UFSM_TOKEN_KWD_VALID_FOR    }
                                                                                    , {"enter"           , UFSM_TOKEN_KWD_ENTER        }
                                                                                    , {"leave"           , UFSM_TOKEN_KWD_LEAVE        }

                                                                                    , {"self-enter"      , UFSM_TOKEN_KWD_SELF_ENTER   }
                                                                                    , {"self-leave"      , UFSM_TOKEN_KWD_SELF_LEAVE   }
                                                                                    , {"self-e"          , UFSM_TOKEN_KWD_SELF_ENTER   }
                                                                                    , {"self-l"          , UFSM_TOKEN_KWD_SELF_LEAVE   }
                                                                                    , {"s-enter"         , UFSM_TOKEN_KWD_SELF_ENTER   }
                                                                                    , {"s-leave"         , UFSM_TOKEN_KWD_SELF_LEAVE   }

                                                                                    , {"inherits"        , UFSM_TOKEN_KWD_INHERITS     }

                                                                                    // , {""                , UFSM_TOKEN_KWD_CALL         }
                                                                                    // , {""                , UFSM_TOKEN_KWD_BEFORE       }
                                                                                    //, {""                , UFSM_TOKEN_KWD_AFTER        }

                                                                                    , {"call-before"     , UFSM_TOKEN_KWD_CALL_BEFORE  }
                                                                                    , {"call-after"      , UFSM_TOKEN_KWD_CALL_AFTER   }

                                                                                    //, {""                ,       }

                                                                                    //, {""          ,        }
                                                                                    }
                                                                                  , true // case sensitive
                                                                                  );

        tokenizer.template installTokenFilter<KebabCaseComposingFilter>(UFSM_TOKEN_OP_DESCR_FOLOWS);
        tokenizer.template installTokenFilter<SimpleReplaceFilter>(UFSM_TOKEN_OP_NOT_ALTER, UFSM_TOKEN_OP_NOT);
        tokenizer.template installTokenFilter<SimpleReplaceFilter>(UFSM_TOKEN_OP_AND_ALTER, UFSM_TOKEN_OP_AND);
        tokenizer.template installTokenFilter<SimpleReplaceFilter>(UFSM_TOKEN_OP_OR_ALTER , UFSM_TOKEN_OP_OR );

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

} // namespace ufsm
} // namespace tokenizer
} // namespace umba

