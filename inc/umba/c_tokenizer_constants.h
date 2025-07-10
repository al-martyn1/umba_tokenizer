#pragma once

/* Dummy/control tokens without any payload and iterators, only signals for something special */

// Control tokens flag
#define UMBA_TOKENIZER_TOKEN_CTRL_FLAG                                                0x8000u


// Control token, which finalizes input
#define UMBA_TOKENIZER_TOKEN_CTRL_FIN                                                 (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0000u) /* empty token, finalizes processing current text */
//#define UMBA_TOKENIZER_TOKEN_UNEXPECTED                                               (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0001u)

// Для сброса состояния фильтров
#define UMBA_TOKENIZER_TOKEN_CTRL_RST                                                 (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0001u)


#define UMBA_TOKENIZER_TOKEN_BASE_FIRST                                               0x0000u
#define UMBA_TOKENIZER_TOKEN_BASE_LAST                                                0x007Fu

#define UMBA_TOKENIZER_TOKEN_BASE_USER_FIRST                                          (UMBA_TOKENIZER_TOKEN_BASE_LAST+1)
#define UMBA_TOKENIZER_TOKEN_BASE_USER_LAST                                           0x00FFu     


#define UMBA_TOKENIZER_TOKEN_NUL                                                      0x0000u
#define UMBA_TOKENIZER_TOKEN_UNEXPECTED                                               0x0001u
#define UMBA_TOKENIZER_TOKEN_RAW_DATA                                                 0x0002u
#define UMBA_TOKENIZER_TOKEN_LINEFEED                                                 0x0003u
#define UMBA_TOKENIZER_TOKEN_SPACE                                                    0x0004u
#define UMBA_TOKENIZER_TOKEN_TAB                                                      0x0005u
#define UMBA_TOKENIZER_TOKEN_FORM_FEED                                                0x0006u /* Надо добавить поддержку FF */
#define UMBA_TOKENIZER_TOKEN_IDENTIFIER                                               0x0007u
#define UMBA_TOKENIZER_TOKEN_FQIDENT                                                  0x0008u /* Full qualified identifier (with NS separators) */
#define UMBA_TOKENIZER_TOKEN_FQIDENT_SCOPERES                                         0x0009u /* For internal usage Full qualified identifier with scope resolution/NS separators at end */
#define UMBA_TOKENIZER_TOKEN_FQIDENT_AOPEN                                            0x000Au /* For internal usage Full qualified identifier with '<' at end */
#define UMBA_TOKENIZER_TOKEN_FQIDENT_ACLOSE                                           0x000Bu /* For internal usage Full qualified identifier with '>' at end */
#define UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA                                            0x000Cu /* For internal usage Full qualified identifier with ',' at end */
//#define UMBA_TOKENIZER_TOKEN_FQIDENT_                                            0x000Cu 
#define UMBA_TOKENIZER_TOKEN_SEMIALPHA                                                0x000Du
#define UMBA_TOKENIZER_TOKEN_ESCAPE                                                   0x000Eu
#define UMBA_TOKENIZER_TOKEN_LINE_CONTINUATION                                        0x000Fu
#define UMBA_TOKENIZER_TOKEN_UNCLASSIFIED_CHAR                                        0x0010u


#define UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN                                       0x0021u
#define UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_CLOSE                                      0x0022u
#define UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS                                           UMBA_TOKENIZER_TOKEN_CURLY_BRACKET_OPEN

#define UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN                                       0x0031u
#define UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_CLOSE                                      0x0032u
#define UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS                                           UMBA_TOKENIZER_TOKEN_ROUND_BRACKET_OPEN

#define UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN                                       0x0041u
#define UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_CLOSE                                      0x0042u
#define UMBA_TOKENIZER_TOKEN_ANGLE_BRACKETS                                           UMBA_TOKENIZER_TOKEN_ANGLE_BRACKET_OPEN

#define UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN                                      0x0051u
#define UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_CLOSE                                     0x0052u
#define UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS                                          UMBA_TOKENIZER_TOKEN_SQUARE_BRACKET_OPEN

#define UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN                                   0x0061u
#define UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_CLOSE                                  0x0062u
#define UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKETS                                       UMBA_TOKENIZER_TOKEN_DBLSQUARE_BRACKET_OPEN

// !!! При изменении базовых констант не забываем, что нельзя вылезать за UMBA_TOKENIZER_TOKEN_BASE_LAST (или надо поправить эту константу)

// На всякий случай считаем за идентификатор даже константы для внутреннего использования
#define UMBA_TOKENIZER_TOKEN_IDENTIFIER_FIRST                                         UMBA_TOKENIZER_TOKEN_IDENTIFIER
#define UMBA_TOKENIZER_TOKEN_IDENTIFIER_LAST                                          UMBA_TOKENIZER_TOKEN_FQIDENT_COMMA

#define UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_FALSE                                       0x0080u
#define UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_TRUE                                        0x0081u

#define UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_FIRST                                       UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_FALSE
#define UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_LAST                                        UMBA_TOKENIZER_TOKEN_BOOL_LITERAL_TRUE




// Кодирование числовых токенов
// Требуется
// 1) флаг плавучки - 1 бит
// 2) флаг miss digit - если префикс есть, а цифр после него нет - этот флаг позволяет трактовать префикс как валидное число
// 3) Четыре (младших) бита резервируем для пользователя
// 4) Три бита резервируем под основание системы счисления
// Итого: 1+1+4+3=9 бит, 0x01FF маска

#define UMBA_TOKENIZER_TOKEN_FLOAT_FLAG                                               0x0100u

// Числовой литерал, целый или плавучка
// Базовый номер токена, не флаг признака числового литерала!
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER                                          0x1000u

#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER                                             ((UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER)|(UMBA_TOKENIZER_TOKEN_FLOAT_FLAG))
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST                                     UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_LAST                                      0x11FFu

#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_FIRST                                    UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_LAST                                     ((UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER)-1)

#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_FIRST                                       UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER
#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_LAST                                        ((UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_FIRST)+(UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)-1)

// #define UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_FIRST                                (UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FIRST+1)
// #define UMBA_TOKENIZER_TOKEN_NUMBER_USER_LITERAL_LAST                                 0x11FFu


// Кодируем признаки числового литерала
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_FLAG_MISS_DIGIT                           0x0080u  /* После префикса может не быть ни одной цифры */
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_MASK                                 0x0070u  /* Маска для системы счисления */
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC                                  0x0000u
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN                                  0x0010u
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_QUAT                                 0x0020u  /* четвертичная quaternary number system */
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_OCT                                  0x0030u
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DUOD                                 0x0040u  /* двенадцатеричная duodecimal number system */
#define UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_HEX                                  0x0050u


// #define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER                                          0x1000u
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC                                      (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DEC )
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN                                      (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_BIN )
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_QUAT                                     (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_QUAT)
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT                                      (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_OCT )
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DUOD                                     (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_DUOD)
#define UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX                                      (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_NUMBER_LITERAL_BASE_HEX )

// #define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER                                             (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER|UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_DEC                                         (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DEC |UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_BIN                                         (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_BIN |UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_QUAT                                        (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_QUAT|UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_OCT                                         (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_OCT |UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_DUOD                                        (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_DUOD|UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)
#define UMBA_TOKENIZER_TOKEN_FLOAT_NUMBER_HEX                                         (UMBA_TOKENIZER_TOKEN_INTEGRAL_NUMBER_HEX |UMBA_TOKENIZER_TOKEN_FLOAT_FLAG)



// Переехали из диапазона операторов
// Или пока не переехали
#define UMBA_TOKENIZER_TOKEN_COMMENT_BASE                                             0x2000u /* 0x1200u */
#define UMBA_TOKENIZER_TOKEN_COMMENT_FIRST                                            UMBA_TOKENIZER_TOKEN_COMMENT_BASE
#define UMBA_TOKENIZER_TOKEN_COMMENT_LAST                                             0x207Fu /* 0x123Fu */

#define UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE_FIRST                                (UMBA_TOKENIZER_TOKEN_COMMENT_BASE+0x010u)  /*  //    */
#define UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE_LAST                                 (UMBA_TOKENIZER_TOKEN_COMMENT_BASE+0x01Fu)  /*        */
#define UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE                                      UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE_FIRST

#define UMBA_TOKENIZER_TOKEN_COMMENT_MULTI_LINE_START                                 (UMBA_TOKENIZER_TOKEN_COMMENT_BASE+0x021u)  /*        */
#define UMBA_TOKENIZER_TOKEN_COMMENT_MULTI_LINE_END                                   (UMBA_TOKENIZER_TOKEN_COMMENT_BASE+0x022u)  /*        */
#define UMBA_TOKENIZER_TOKEN_COMMENT_MULTI_LINE                                       UMBA_TOKENIZER_TOKEN_COMMENT_MULTI_LINE_START



// С какого хрена коменты я присунул в операторы?
// С какого-то присунул
// Оставляю для совместимости
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST                       UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE_FIRST
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST                        UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE_LAST 

#define UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT                             UMBA_TOKENIZER_TOKEN_COMMENT_SINGLE_LINE


#define UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_START                        UMBA_TOKENIZER_TOKEN_COMMENT_MULTI_LINE_START
#define UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT                              UMBA_TOKENIZER_TOKEN_COMMENT_MULTI_LINE
#define UMBA_TOKENIZER_TOKEN_OPERATOR_MULTI_LINE_COMMENT_END                          UMBA_TOKENIZER_TOKEN_COMMENT_MULTI_LINE_END

#define UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FLAG_AS_REGULAR_OPERATOR    0x040u   /*        */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST_AS_REGULAR_OPERATOR   (UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FIRST|UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FLAG_AS_REGULAR_OPERATOR)
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST_AS_REGULAR_OPERATOR    (UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_LAST |UMBA_TOKENIZER_TOKEN_OPERATOR_SINGLE_LINE_COMMENT_FLAG_AS_REGULAR_OPERATOR)




#define UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST                                           0x2000u
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LAST                                            0x2FFFu
                                                                                   // 0x0205u // Последний пока заданный в данном файле
#define UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST                                      0x2800u
#define UMBA_TOKENIZER_TOKEN_OPERATOR_USER_LAST                                       UMBA_TOKENIZER_TOKEN_OPERATOR_LAST

#define UMBA_TOKENIZER_TOKEN_USER_OPERATOR_FIRST                                      UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST
#define UMBA_TOKENIZER_TOKEN_USER_OPERATOR_LAST                                       UMBA_TOKENIZER_TOKEN_OPERATOR_USER_LAST 


#define UMBA_TOKENIZER_TOKEN_STRING_LITERAL_FIRST                                     0x3000u
#define UMBA_TOKENIZER_TOKEN_STRING_LITERAL_LAST                                      0x3FFFu

#define UMBA_TOKENIZER_TOKEN_STRING_LITERAL                                           0x3000u
#define UMBA_TOKENIZER_TOKEN_CHAR_LITERAL                                             0x3001u
#define UMBA_TOKENIZER_TOKEN_ANGLE_BACKETS_STRING_LITERAL                             0x3002u
#define UMBA_TOKENIZER_TOKEN_RAW_STRING_LITERAL                                       0x3003u

#define UMBA_TOKENIZER_TOKEN_STRING_USER_LITERAL_FIRST                                0x3010u
#define UMBA_TOKENIZER_TOKEN_STRING_USER_LITERAL_LAST                                 UMBA_TOKENIZER_TOKEN_LITERAL_LAST


// На каждый сет встроенных кейвордов - по 1024 значений хватит?
#define UMBA_TOKENIZER_TOKEN_KEYWORDS_FIRST                                           0x4000u
#define UMBA_TOKENIZER_TOKEN_KEYWORDS_LAST                                            0x5FFFu



#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST                                       0x4000u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_LAST                                        0x43FFu

#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST                                       0x4400u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_LAST                                        0x47FFu

#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST                                       0x4800u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_LAST                                        0x4BFFu

#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET4_FIRST                                       0x4C00u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET4_LAST                                        0x4FFFu

#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET5_FIRST                                       0x5000u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET5_LAST                                        0x53FFu

#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET6_FIRST                                       0x5400u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET6_LAST                                        0x57FFu

#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET7_FIRST                                       0x5800u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET7_LAST                                        0x5BFFu

#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET8_FIRST                                       0x5C00u
#define UMBA_TOKENIZER_TOKEN_KEYWORD_SET8_LAST                                        0x5FFFu


// #define UMBA_TOKENIZER_TOKEN_CTRL_PP_DEFINE                                           (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0003u) /* empty token, that "define" PP directive detected. Non-paired token */
// #define UMBA_TOKENIZER_TOKEN_CTRL_PP_INCLUDE                                          (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0004u) /* empty token, that "include" PP directive detected. Non-paired token */




// #define UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST_GENERIC                            (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x030u)

// C++
// https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B

#define UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION                                 (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x130u)  /*  +    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_ONE_MORE_REPETITION                      UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION        /*  +  regex mark alias */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION                              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x131u)  /*  -    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION                           (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x132u)  /*  *    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_ANY_NUM_REPETITIONS                      UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION  /*  *   regex mark alias */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION                                 (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x133u)  /*  /    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO                                   (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x134u)  /*  %    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_INCREMENT                                (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x135u)  /*  ++   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT                                (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x136u)  /*  --   */

#define UMBA_TOKENIZER_TOKEN_OPERATOR_EQUAL                                    (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x140u)   /*  ==   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_NOT_EQUAL                                (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x141u)   /*  !=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER                                  (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x142u)   /*  >    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LESS                                     (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x143u)   /*  <    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_GREATER_EQUAL                            (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x144u)   /*  >=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LESS_EQUAL                               (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x145u)   /*  <=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_THREE_WAY_COMPARISON                     (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x146u)   /*  <=>  */

#define UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_NOT                              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x150u)   /*  !    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_AND                              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x151u)   /*  &&   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR                               (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x152u)   /*  ||   */

#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_NOT                              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x160u)   /*  ~    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND                              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x161u)   /*  &    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR                               (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x162u)   /*  |    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR                              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x163u)   /*  ^    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT                       (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x164u)   /*  <<   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT                      (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x165u)   /*  >>   */

#define UMBA_TOKENIZER_TOKEN_OPERATOR_BNF_ALTER                                UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR


#define UMBA_TOKENIZER_TOKEN_OPERATOR_ASSIGNMENT                               (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x170u)   /*  =    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_ADDITION_ASSIGNMENT                      (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x171u)   /*  +=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SUBTRACTION_ASSIGNMENT                   (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x172u)   /*  -=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_MULTIPLICATION_ASSIGNMENT                (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x173u)   /*  *=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_DIVISION_ASSIGNMENT                      (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x174u)   /*  /=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_MODULO_ASSIGNMENT                        (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x175u)   /*  %=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_AND_ASSIGNMENT                   (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x176u)   /*  &=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_OR_ASSIGNMENT                    (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x177u)   /*  |=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_XOR_ASSIGNMENT                   (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x178u)   /*  ^=   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT_ASSIGNMENT            (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x179u)   /*  <<=  */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT_ASSIGNMENT           (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x17Au)   /*  >>=  */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PASCAL_ASSIGNMENT                        (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x17Bu)   /*  :=   */
// https://stackoverflow.com/questions/7294217/scala-what-do-and-operator-do
// Also used in BNF notation
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SCALA_CONCAT_ASSIGNMENT                  (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x17Cu)   /*  ::=   */


// https://en.cppreference.com/w/cpp/language/operator_member_access
#define UMBA_TOKENIZER_TOKEN_OPERATOR_STRUCTURE_DEREFERENCE                    (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x180u)   /*  ->   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_MEMBER_OF_POINTER                        (UMBA_TOKENIZER_TOKEN_OPERATOR_STRUCTURE_DEREFERENCE)
#define UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_POINTER             (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x181u)   /*  ->*  */ /* Member of object   selected by pointer-to-member b of object pointed to by a 'a->*b' */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_POINTER_TO_MEMBER_OF_OBJECT              (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x182u)   /*  .*   */ /* Member of object a selected by pointer-to-member b 'a.*b' */

#define UMBA_TOKENIZER_TOKEN_OPERATOR_DOT                                      (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x183u)   /*  .    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_COMMA                                    (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x184u)   /*  ,    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_COLON                                    (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x185u)   /*  :    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_SEMICOLON                                (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x186u)   /*  ;    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_QMARK                                    (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x187u)   /*  ?    */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_CC_TERNARY                               UMBA_TOKENIZER_TOKEN_OPERATOR_QMARK
#define UMBA_TOKENIZER_TOKEN_OPERATOR_ZERO_OR_ONE_REPETITIONS                  UMBA_TOKENIZER_TOKEN_OPERATOR_QMARK            /*  regex mark alias */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_CONDITIONAL                      UMBA_TOKENIZER_TOKEN_OPERATOR_QMARK
#define UMBA_TOKENIZER_TOKEN_OPERATOR_TERNARY_ALTERNATIVE                      UMBA_TOKENIZER_TOKEN_OPERATOR_COLON
#define UMBA_TOKENIZER_TOKEN_OPERATOR_EXPRESSION_END                           UMBA_TOKENIZER_TOKEN_OPERATOR_SEMICOLON

#define UMBA_TOKENIZER_TOKEN_OPERATOR_SCOPE_RESOLUTION                         (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x190u)   /*  ::   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_VA_ARGS                                  (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x191u)   /*  ...  */


//  "new slot" operator '<-' - http://squirrel-lang.org/doc/squirrel3.html#tableconstructor
//  => C# - operator lambda  - https://learn.microsoft.com/ru-ru/dotnet/csharp/language-reference/operators/lambda-operator
//     JS - https://ru.stackoverflow.com/questions/528707/%D0%9E%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D0%BE%D1%80-%D0%B2-javascript
//          https://developer.mozilla.org/ru/docs/Web/JavaScript/Reference/Functions/Arrow_functions

#define UMBA_TOKENIZER_TOKEN_OPERATOR_NEW_SLOT                                 (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1A0u)   /*  <-  */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_LAMBDA                                   (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1A1u)   /*  =>  */


// ===  JS Strict_equality -  https://developer.mozilla.org/ru/docs/Web/JavaScript/Reference/Operators/Strict_equality
#define UMBA_TOKENIZER_TOKEN_OPERATOR_STRICT_EQUAL                             (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1A2u)   /*  ===   */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_STRICT_NOT_EQUAL                         (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1A3u)   /*  !==   */

#define UMBA_TOKENIZER_TOKEN_OPERATOR_AT                                       (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1A4u)   /*  @     */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_HASH                                     (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1A5u)   /*  #     */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_USD                                      (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1A6u)   /*  $     */


// C/C++ preprocessor
#define UMBA_TOKENIZER_TOKEN_OPERATOR_CC_PP_CONCAT                             (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1E0u)   /*  ##  */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_CC_PP_STRINGIFY                          (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x1E1u)   /*  #  */


// Generic
#define UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION                               (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x200u)   /*  -> --> --> etc */
#define UMBA_TOKENIZER_TOKEN_OPERATOR_FOLLOW                                   UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION


// PlantUML
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_DIRECTED             (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x201u)
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_DIRECTED_UP          (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x202u)
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_DIRECTED_DOWN        (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x203u)
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_DIRECTED_LEFT        (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x204u)
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_DIRECTED_RIGHT       (UMBA_TOKENIZER_TOKEN_OPERATOR_FIRST+0x205u)

#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_FIRST                UMBA_TOKENIZER_TOKEN_OPERATOR_TRANSITION
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_LAST                 UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_TRANSITION_DIRECTED_RIGHT

#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_HSPLIT                          UMBA_TOKENIZER_TOKEN_OPERATOR_DECREMENT
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_VSPLIT                          UMBA_TOKENIZER_TOKEN_OPERATOR_LOGICAL_OR

// <<STEREOTYPE>>
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_STEREOTYPE_LEFT                 UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_LEFT
#define UMBA_TOKENIZER_TOKEN_OPERATOR_PLANTUML_STEREOTYPE_RIGHT                UMBA_TOKENIZER_TOKEN_OPERATOR_BITWISE_SHIFT_RIGHT


// C/C++ препроцессор относится ко второму набору ключевых слов
#define UMBA_TOKENIZER_TOKEN_CC_PP_BASE                                        UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST

#define UMBA_TOKENIZER_TOKEN_CC_PP_INCLUDE                                     (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0008u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_DEFINE                                      (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0009u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_UNDEF                                       (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x000Au)

#define UMBA_TOKENIZER_TOKEN_CC_PP_LINE                                        (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0010u)

#define UMBA_TOKENIZER_TOKEN_CC_PP_ERROR                                       (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0020u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_WARNING                                     (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0021u)

#define UMBA_TOKENIZER_TOKEN_CC_PP_PRAGMA                                      (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0030u)

#define UMBA_TOKENIZER_TOKEN_CC_PP_IF                                          (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0040u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_ELIF                                        (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0041u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_ELSE                                        (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0042u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_ENDIF                                       (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0043u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_IFDEF                                       (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0044u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_IFNDEF                                      (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0045u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_ELIFDEF                                     (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0046u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_ELIFNDEF                                    (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0047u)

#define UMBA_TOKENIZER_TOKEN_CC_PP_DEFINED                                     (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0050u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_HAS_INCLUE                                  (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0051u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_HAS_CPP_ATTRIBUTE                           (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0052u)

#define UMBA_TOKENIZER_TOKEN_CC_PP_EXPORT                                      (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0060u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_IMPORT                                      (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0061u)
#define UMBA_TOKENIZER_TOKEN_CC_PP_MODULE                                      (UMBA_TOKENIZER_TOKEN_CC_PP_BASE|0x0062u)


// #define UMBA_TOKENIZER_TOKEN_CTRL_FLAG                                                0x8000u

#define UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_START                                  (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0010u)                            /* empty token, signals that C/C++ preprocessor mode starts */
#define UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_END                                    (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|0x0011u)                            /* empty token, signals that C/C++ preprocessor mode ends */
#define UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_DEFINE                                 (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|UMBA_TOKENIZER_TOKEN_CC_PP_DEFINE)  /* empty token, that "define" PP directive detected. Non-paired token */
#define UMBA_TOKENIZER_TOKEN_CTRL_CC_PP_INCLUDE                                (UMBA_TOKENIZER_TOKEN_CTRL_FLAG|UMBA_TOKENIZER_TOKEN_CC_PP_INCLUDE) /* empty token, that "include" PP directive detected. Non-paired token */



// PlantUML

#define UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST                    UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST
#define UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_LAST                     (UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST-1)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STATE                                    (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x000u)
// [*] begin/end pseudo states
#define UMBA_TOKENIZER_TOKEN_PLANTUML_PSEUDO_STATE                             (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x001u)   /* [*] */
#define UMBA_TOKENIZER_TOKEN_PLANTUML_HISTORY_STATE                            (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x002u)   /* [H] */
#define UMBA_TOKENIZER_TOKEN_PLANTUML_HISTORYPLUS_STATE                        (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x003u)   /* [H*] */

#define UMBA_TOKENIZER_TOKEN_PLANTUML_HIDE                                     (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x010u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_EMPTY                                    (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x011u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_DESCRIPTION                              (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x012u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_SCALE                                    (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x013u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_WIDTH                                    (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x014u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_HEIGHT                                   (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x015u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_AS                                       (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x016u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_NOTE                                     (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x017u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_ON                                       (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x018u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_END                                      (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x019u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_OF                                       (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x01Au)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_UP                                       (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x01Bu)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_DOWN                                     (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x01Cu)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_LEFT                                     (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x01Du)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_RIGHT                                    (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x01Eu)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_DASHED                                   (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x01Fu)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_DOTTED                                   (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x020u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_BOLD                                     (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x021u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_ITALIC                                   (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x022u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_SKINPARAM                                (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x023u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_LINETYPE                                 (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x024u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_POLYLINE                                 (UMBA_TOKENIZER_TOKEN_PLANTUML_NORMAL_KEYWORDS_FIRST+0x025u)


#define UMBA_TOKENIZER_TOKEN_PLANTUML_UML_FRAME_KEYWORDS_FIRST                 UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST
#define UMBA_TOKENIZER_TOKEN_PLANTUML_UML_FRAME_KEYWORDS_LAST                  (UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST-1)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STARTUML                                 (UMBA_TOKENIZER_TOKEN_PLANTUML_UML_FRAME_KEYWORDS_FIRST+0x000u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_ENDUML                                   (UMBA_TOKENIZER_TOKEN_PLANTUML_UML_FRAME_KEYWORDS_FIRST+0x001u)


#define UMBA_TOKENIZER_TOKEN_PLANTUML_TAG_KEYWORDS_FIRST                       UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST
#define UMBA_TOKENIZER_TOKEN_PLANTUML_TAG_KEYWORDS_LAST                        (UMBA_TOKENIZER_TOKEN_KEYWORD_SET4_FIRST-1)

#define UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_TAG_START                        (UMBA_TOKENIZER_TOKEN_PLANTUML_TAG_KEYWORDS_FIRST+0x000u)   /* <tag> */
#define UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_TAG_END                          (UMBA_TOKENIZER_TOKEN_PLANTUML_TAG_KEYWORDS_FIRST+0x001u)   /* </tag> */
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STYLE_TAG_START                          (UMBA_TOKENIZER_TOKEN_PLANTUML_TAG_KEYWORDS_FIRST+0x002u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STYLE_TAG_END                            (UMBA_TOKENIZER_TOKEN_PLANTUML_TAG_KEYWORDS_FIRST+0x003u)


#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_FIRST                         UMBA_TOKENIZER_TOKEN_KEYWORD_SET4_FIRST
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_LAST                          (UMBA_TOKENIZER_TOKEN_KEYWORD_SET5_FIRST-1)

// <<STEREOTYPE>>

#define UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE                       (UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_FIRST+0x000u)   /* <<STEREOTYPE>> */

#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_START                         (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x001u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_END                           (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x002u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_FORK                          (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x003u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_JOIN                          (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x004u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_CHOICE                        (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x005u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_ENTRYPOINT                    (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x006u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_EXITPOINT                     (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x007u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_INPUTPIN                      (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x008u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_OUTPUTPIN                     (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x009u)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_EXPANSIONINPUT                (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x00Au)
#define UMBA_TOKENIZER_TOKEN_PLANTUML_STEREOTYPE_EXPANSIONOUTPUT               (UMBA_TOKENIZER_TOKEN_PLANTUML_GENERIC_STEREOTYPE+0x00Bu)



#define UMBA_TOKENIZER_TOKEN_PLANTUML_UNCLASSIFIED_FIRST                       UMBA_TOKENIZER_TOKEN_KEYWORD_SET8_FIRST
#define UMBA_TOKENIZER_TOKEN_PLANTUML_UNCLASSIFIED_LAST                        UMBA_TOKENIZER_TOKEN_KEYWORD_SET8_LAST

#define UMBA_TOKENIZER_TOKEN_PLANTUML_COLOR                                    UMBA_TOKENIZER_TOKEN_PLANTUML_UNCLASSIFIED_FIRST


// Start Tag <name>
// End Tag </name>
// Stereotype <<name>>









