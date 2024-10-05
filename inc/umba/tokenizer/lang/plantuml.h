#pragma once

#include "umba/umba.h"
//
#include "umba/tokenizer.h"
#include "umba/tokenizer/token_filters.h"
#include "umba/tokenizer/string_literal_parsing.h"
#include "umba/string_plus.h"



// umba::tokenizer::
namespace umba {
namespace tokenizer {


template< typename CharType            // Input chars type
        , typename UserDataType        = void*
        , typename CharClassTableType  = std::array<CharClass, 128>
        , typename TrieVectorType      = std::vector<TrieNode>
        , typename StringType          = std::basic_string<CharType>  // Тип строки, которая хранит входные символы
        , typename MessagesStringType  = std::string  // Тип строки, которая используется для сообщений (в том числе и от внешних суб-парсеров)
        , typename InputIteratorType   = umba::iterator::TextPositionCountingIterator<CharType>
        , typename InputIteratorTraits = umba::iterator::TextIteratorTraits<InputIteratorType>
        , typename TokenizerType       = Tokenizer< CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType >
        >
TokenizerBuilder<CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits, TokenizerType>
makeTokenizerBuilderPlantUml()
{
    using CppStringLiteralParser     = CppEscapedSimpleQuotedStringLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;
    //using AngleBracketsLiteralParser = SimpleQuotedStringLiteralParser<CharType, MessagesStringType, InputIteratorType, InputIteratorTraits>;

    using umba::string_plus::make_string;

    auto tokenizerBuilder = TokenizerBuilder<CharType, UserDataType, CharClassTableType, TrieVectorType, StringType, MessagesStringType, InputIteratorType, InputIteratorTraits, TokenizerType>()

                          .generateStandardCharClassTable()


                          .addNumbersPrefix(make_string<StringType>("0b"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN)
                          .addNumbersPrefix(make_string<StringType>("0B"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0d"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC)
                          .addNumbersPrefix(make_string<StringType>("0D"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC, true) // allowUseExistingToken

                          .addNumbersPrefix(make_string<StringType>("0") , UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT | UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT)

                          .addNumbersPrefix(make_string<StringType>("0x"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX)
                          .addNumbersPrefix(make_string<StringType>("0X"), UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX, true) // allowUseExistingToken


                          .addBrackets(make_string<StringType>("{}"), UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS )
                          .addBrackets(make_string<StringType>("()"), UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS )
                          .addBrackets(make_string<StringType>("<>"), UMBA_TOKENIZER_TOKEN_ANGLE_BRACKETS )
                          .addBrackets(make_string<StringType>("[]"), UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS)


                          .addSingleLineComment(make_string<StringType>("'"), UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST)
                          .setMultiLineComment(make_string<StringType>("/'"), make_string<StringType>("'/"))



                          .addOperator(make_string<StringType>("#"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_HASH                          )
                          .addOperator(make_string<StringType>("@"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_AT                            )
 
                          //.addOperator(make_string<StringType>("[*]"), UMBA_TOKENIZER_TOKEN_OPERATOR_PSEUDO_STATE                    )

                          .addOperator(make_string<StringType>("+"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION                      )
                          .addOperator(make_string<StringType>("-"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION                   )
                          .addOperator(make_string<StringType>("*"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION                )
                          .addOperator(make_string<StringType>("/"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION                      )

                          .addOperator(make_string<StringType>(">"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER                       )
                          .addOperator(make_string<StringType>("<"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_LESS                          )

                          .addOperator(make_string<StringType>("--" ), UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_HSPLIT               )
                          .addOperator(make_string<StringType>("||" ), UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_VSPLIT               )

                          .addOperator(make_string<StringType>("|"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR                    )

                          .addOperator(make_string<StringType>("<<" ), UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_STEREOTYPE_LEFT      )
                          .addOperator(make_string<StringType>(">>" ), UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_STEREOTYPE_RIGHT     )

                          .addOperator(make_string<StringType>(":"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_COLON                         )

                          .addOperator(make_string<StringType>("->"              ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )
                          .addOperator(make_string<StringType>("-->"             ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )
                          .addOperator(make_string<StringType>("--->"            ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )
                          .addOperator(make_string<StringType>("---->"           ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )
                          .addOperator(make_string<StringType>("----->"          ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )
                          .addOperator(make_string<StringType>("------>"         ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )
                          .addOperator(make_string<StringType>("------->"        ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )
                          .addOperator(make_string<StringType>("-------->"       ), UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION, true )

                          

                          #if 0
                          .addOperator(make_string<StringType>("%"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO                        )

                          .addOperator(make_string<StringType>("++" ), UMBA_TOKENIZER_TOKEN_OPERATOR_INCREMENT                     )
                          // .addOperator(make_string<StringType>("--" ), UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT                     )
                          .addOperator(make_string<StringType>("==" ), UMBA_TOKENIZER_TOKEN_OPERATOR_EQUAL                         )
                          .addOperator(make_string<StringType>("!=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_NOT_EQUAL                     )
                          .addOperator(make_string<StringType>(">=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER_EQUAL                 )
                          .addOperator(make_string<StringType>("<=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_LESS_EQUAL                    )
                          .addOperator(make_string<StringType>("<=>"), UMBA_TOKENIZER_TOKEN_OPERATOR_THREE_WAY_COMPARISON          )
                          .addOperator(make_string<StringType>("!"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT                   )
                          .addOperator(make_string<StringType>("&&" ), UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND                   )
                          //.addOperator(make_string<StringType>("||" ), UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR                    )
                          .addOperator(make_string<StringType>("~"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT                   )
                          .addOperator(make_string<StringType>("&"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND                   )
                          //.addOperator(make_string<StringType>("|"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR                    )
                          .addOperator(make_string<StringType>("^"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR                   )
                          // .addOperator(make_string<StringType>("<<" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT            )
                          // .addOperator(make_string<StringType>(">>" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT           )
                          .addOperator(make_string<StringType>("="  ), UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT                    )
                          .addOperator(make_string<StringType>("+=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION_ASSIGNMENT           )
                          .addOperator(make_string<StringType>("-=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION_ASSIGNMENT        )
                          .addOperator(make_string<StringType>("*=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION_ASSIGNMENT     )
                          .addOperator(make_string<StringType>("/=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION_ASSIGNMENT           )
                          .addOperator(make_string<StringType>("%=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO_ASSIGNMENT             )
                          .addOperator(make_string<StringType>("&=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND_ASSIGNMENT        )
                          .addOperator(make_string<StringType>("|=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR_ASSIGNMENT         )
                          .addOperator(make_string<StringType>("^=" ), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR_ASSIGNMENT        )
                          .addOperator(make_string<StringType>("<<="), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT_ASSIGNMENT )
                          .addOperator(make_string<StringType>(">>="), UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT_ASSIGNMENT)
                          .addOperator(make_string<StringType>(","  ), UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA)
                          .addOperator(make_string<StringType>("->" ), UMBA_TOKENIZER_TOKEN_OPERATOR_MEMBER_OF_POINTER             )
                          .addOperator(make_string<StringType>("->*"), UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_POINTER  )
                          .addOperator(make_string<StringType>(".*" ), UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_OBJECT   )
                          //.addOperator(make_string<StringType>(":"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE           )
                          .addOperator(make_string<StringType>("?"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_CONDITIONAL           )
                          .addOperator(make_string<StringType>("::" ), UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION              )
                          .addOperator(make_string<StringType>(";"  ), UMBA_TOKENIZER_TOKEN_OPERATOR_EXPRESSION_END                )
                          //.addOperator( )
                          #endif

                          .template addStringLiteralParser<CppStringLiteralParser>( UMBA_TOKENIZER_TOKEN_STRING_LITERAL
                                                                                  , { make_string<StringType>("\"")   // UMBA_TOKENIZER_TOKEN_STRING_LITERAL itself
                                                                                    // , make_string<StringType>("\'")   // UMBA_TOKENIZER_TOKEN_CHAR_LITERAL (UMBA_TOKENIZER_TOKEN_STRING_LITERAL+1)
                                                                                    }
                                                                                  )
                          //.template addStringLiteralParser< AngleBracketsLiteralParser >(UMBA_TOKENIZER_TOKEN_ANGLE_BACKETS_STRING_LITERAL, { make_string<StringType>("<") } )

                          ;

    return tokenizerBuilder;

}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace pluntuml {

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

} // namespace pluntuml

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
template<typename TokenizerBuilder, typename TokenHandler>
typename TokenizerBuilder::tokenizer_type makeTokenizerPlantUml(TokenizerBuilder builder, TokenHandler tokenHandler)
{
    using tokenizer_type = typename TokenizerBuilder::tokenizer_type;
    using string_type    = typename tokenizer_type::string_type;

    using SimpleSequenceComposingFilter       = umba::tokenizer::filters::SimpleSequenceComposingFilter<tokenizer_type>;
    using IdentifierToKeywordConversionFilter = umba::tokenizer::filters::IdentifierToKeywordConversionFilter<tokenizer_type>;
    using RawCharsCollectingFilter            = umba::tokenizer::filters::RawCharsCollectingFilter<tokenizer_type>;


    auto tokenizer = builder.makeTokenizer();
    tokenizer.tokenHandler = tokenHandler;


    // !!! Фильтры, установленные позже, отрабатывают раньше

    tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                                              , std::unordered_map<string_type, payload_type>
                                                                                { {"startuml"        , UMBA_TOKENIZER_TOKEN_PLANTUML_STARTUML     }
                                                                                , {"enduml"          , UMBA_TOKENIZER_TOKEN_PLANTUML_ENDUML       }
                                                                                , {"state"           , UMBA_TOKENIZER_TOKEN_PLANTUML_STATE        }
                                                                                , {"hide"            , UMBA_TOKENIZER_TOKEN_PLANTUML_HIDE         }
                                                                                , {"empty"           , UMBA_TOKENIZER_TOKEN_PLANTUML_EMPTY        }
                                                                                , {"description"     , UMBA_TOKENIZER_TOKEN_PLANTUML_DESCRIPTION  }
                                                                                , {"scale"           , UMBA_TOKENIZER_TOKEN_PLANTUML_SCALE        }
                                                                                , {"width"           , UMBA_TOKENIZER_TOKEN_PLANTUML_WIDTH        }
                                                                                , {"height"          , UMBA_TOKENIZER_TOKEN_PLANTUML_HEIGHT       }
                                                                                , {"as"              , UMBA_TOKENIZER_TOKEN_PLANTUML_AS           }
                                                                                , {"note"            , UMBA_TOKENIZER_TOKEN_PLANTUML_NOTE         }
                                                                                , {"on"              , UMBA_TOKENIZER_TOKEN_PLANTUML_ON           }
                                                                                , {"end"             , UMBA_TOKENIZER_TOKEN_PLANTUML_END          }
                                                                                , {"of"              , UMBA_TOKENIZER_TOKEN_PLANTUML_OF           }
                                                                                , {"up"              , UMBA_TOKENIZER_TOKEN_PLANTUML_UP           }
                                                                                , {"down"            , UMBA_TOKENIZER_TOKEN_PLANTUML_DOWN         }
                                                                                , {"left"            , UMBA_TOKENIZER_TOKEN_PLANTUML_LEFT         }
                                                                                , {"right"           , UMBA_TOKENIZER_TOKEN_PLANTUML_RIGHT        }
                                                                                , {"dashed"          , UMBA_TOKENIZER_TOKEN_PLANTUML_DASHED       }
                                                                                , {"dotted"          , UMBA_TOKENIZER_TOKEN_PLANTUML_DOTTED       }
                                                                                , {"bold"            , UMBA_TOKENIZER_TOKEN_PLANTUML_BOLD         }
                                                                                , {"italic"          , UMBA_TOKENIZER_TOKEN_PLANTUML_ITALIC       }
                                                                                , {"skinparam"       , UMBA_TOKENIZER_TOKEN_PLANTUML_SKINPARAM    }
                                                                                , {"linetype"        , UMBA_TOKENIZER_TOKEN_PLANTUML_LINETYPE     }
                                                                                , {"polyline"        , UMBA_TOKENIZER_TOKEN_PLANTUML_POLYLINE     }
                                                                                //, {""          ,        }

                                                                                }
                                                                              );

    // Пребразование generic stereotypes в конкретные
    tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE
                                                                              , std::unordered_map<string_type, payload_type>
                                                                                { {"fork"            , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_FORK            }
                                                                                , {"join"            , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_JOIN            }
                                                                                , {"choice"          , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_CHOICE          }
                                                                                , {"start"           , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_START           }
                                                                                , {"end"             , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_END             }
                                                                                , {"entryPoint"      , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_ENTRYPOINT      }
                                                                                , {"exitPoint"       , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_EXITPOINT       }
                                                                                , {"inputPin"        , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_INPUTPIN        }
                                                                                , {"outputPin"       , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_OUTPUTPIN       }
                                                                                , {"expansionInput"  , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_EXPANSIONINPUT  }
                                                                                , {"expansionOutput" , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_EXPANSIONOUTPUT }
                                                                                }
                                                                              );

    tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_TAG_START
                                                                              , std::unordered_map<string_type, payload_type>
                                                                                { {"style"           , UMBA_TOKENIZER_TOKEN_PLANTUML_STYLE_TAG_START            }
                                                                                //, {"join"            , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_JOIN            }
                                                                                }
                                                                              );
    
    tokenizer.template installTokenFilter<IdentifierToKeywordConversionFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_TAG_END
                                                                              , std::unordered_map<string_type, payload_type>
                                                                                { {"style"           , UMBA_TOKENIZER_TOKEN_PLANTUML_STYLE_TAG_END              }
                                                                                //, {"join"            , UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_JOIN            }
                                                                                }
                                                                              );
    

    // Композитим псевдосостояние - [*]
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_PSEUDO_STATE // заменяем последовательность на UMBA_TOKENIZER_TOKEN_PLANTUML_PSEUDO_STATE
                                                                        , 0u // Нагрузку берём по нулевому индексу
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN
                                                                                                   , UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION
                                                                                                   , UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE
                                                                                                   }
                                                                        );

    // Композитим историческое состояние - [H]
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_HISTORY_STATE
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN
                                                                                                   , UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                                                                   , UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE
                                                                                                   }
                                                                          // std::function<bool(std::size_t,TokenizerType&,bool,payload_type,iterator_type,iterator_type,token_parsed_data_type)>;
                                                                        , typename SimpleSequenceComposingFilter::extra_check_function_type(
                                                                              []( std::size_t idx, tokenizer_type &tokenizer, bool lineStartFlag, payload_type payloadToken
                                                                                , typename tokenizer_type::iterator_type b, typename tokenizer_type::iterator_type e, typename tokenizer_type::token_parsed_data_type parsedData
                                                                              ) -> bool
                                                                              {
                                                                                  if (idx!=1)
                                                                                      return true;
                                                                                  auto identifierData = std::get<typename tokenizer_type::IdentifierData>(parsedData);
                                                                                  string_type identifierStr = typename tokenizer_type::string_type(identifierData.data);
                                                                                  if (umba::string_plus::make_string<std::string>(identifierStr)=="H")
                                                                                      return true;
                                                                                  return false;
                                                                              }
                                                                          )
                                                                        );

    // Композитим историческое состояние - [H*]
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_HISTORYPLUS_STATE
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN
                                                                                                   , UMBA_TOKENIZER_TOKEN_IDENTIFIER
                                                                                                   , UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION
                                                                                                   , UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE
                                                                                                   }
                                                                          // std::function<bool(std::size_t,TokenizerType&,bool,payload_type,iterator_type,iterator_type,token_parsed_data_type)>;
                                                                        , typename SimpleSequenceComposingFilter::extra_check_function_type(
                                                                              []( std::size_t idx, tokenizer_type &tokenizer, bool lineStartFlag, payload_type payloadToken
                                                                                , typename tokenizer_type::iterator_type b, typename tokenizer_type::iterator_type e, typename tokenizer_type::token_parsed_data_type parsedData
                                                                              ) -> bool
                                                                              {
                                                                                  if (idx!=1)
                                                                                      return true;
                                                                                  auto identifierData = std::get<typename tokenizer_type::IdentifierData>(parsedData);
                                                                                  string_type identifierStr = typename tokenizer_type::string_type(identifierData.data);
                                                                                  if (umba::string_plus::make_string<std::string>(identifierStr)=="H")
                                                                                      return true;
                                                                                  return false;
                                                                              }
                                                                          )
                                                                        );

    // Композитим начало тэга
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_TAG_START
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_LESS, UMBA_TOKENIZER_TOKEN_IDENTIFIER, UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER }
                                                                        );
    // Композитим конец тэга
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_TAG_END
                                                                        , 2u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_LESS, UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION, UMBA_TOKENIZER_TOKEN_IDENTIFIER, UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER }
                                                                        );
    // Композитим стереотип <<STEREOTYPE>>
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_STEREOTYPE_LEFT, UMBA_TOKENIZER_TOKEN_IDENTIFIER, UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_STEREOTYPE_RIGHT }
                                                                        );


    // Композитим цвет
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_COLOR
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_HASH, UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC, UMBA_TOKENIZER_TOKEN_IDENTIFIER }
                                                                        );
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_COLOR
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_HASH, UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC }
                                                                        );
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_COLOR
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_HASH, UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_OCT, UMBA_TOKENIZER_TOKEN_IDENTIFIER }
                                                                        );
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_COLOR
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_HASH, UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_OCT }
                                                                        );
    tokenizer.template installTokenFilter<SimpleSequenceComposingFilter>( UMBA_TOKENIZER_TOKEN_PLANTUML_COLOR
                                                                        , 1u
                                                                        , std::vector<payload_type>{ UMBA_TOKENIZER_TOKEN_OPERATOR_HASH, UMBA_TOKENIZER_TOKEN_IDENTIFIER }
                                                                        );


    tokenizer.template installTokenFilter<RawCharsCollectingFilter>();


    return tokenizer;
}






} // namespace tokenizer
} // namespace umba

