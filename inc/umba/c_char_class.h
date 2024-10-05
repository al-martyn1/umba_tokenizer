#pragma once

#if defined(UMBA_TOKENIZER_TYPES_COMPACT)

    #if !defined(UMBA_CHAR_CLASS_UNDERLYING_COMPACT)
        #define UMBA_CHAR_CLASS_UNDERLYING_COMPACT
    #endif

#endif


#if !defined(UMBA_TOKENIZER_CHARCLASS)
    #if !defined(__cplusplus)

        #if defined(UMBA_CHAR_CLASS_UNDERLYING_COMPACT)
            #define UMBA_TOKENIZER_CHARCLASS  uint_least16_t
        #else
            #define UMBA_TOKENIZER_CHARCLASS  uint_fast32_t
        #endif

    #else

        #if defined(UMBA_CHAR_CLASS_UNDERLYING_COMPACT)
            #define UMBA_TOKENIZER_CHARCLASS  std::uint_least16_t
        #else
            #define UMBA_TOKENIZER_CHARCLASS  std::uint_fast32_t
        #endif

    #endif
#endif


#define UMBA_TOKENIZER_CHARCLASS_NONE                           0x0000u

#define UMBA_TOKENIZER_CHARCLASS_STRING_LITERAL_PREFIX          0x0001u
#define UMBA_TOKENIZER_CHARCLASS_NONPRINTABLE                   0x0002u
#define UMBA_TOKENIZER_CHARCLASS_LINEFEED                       0x0004u
#define UMBA_TOKENIZER_CHARCLASS_SPACE                          0x0008u
#define UMBA_TOKENIZER_CHARCLASS_OPEN                           0x0010u /* Флаг для парных символов */
#define UMBA_TOKENIZER_CHARCLASS_CLOSE                          0x0020u /* Флаг для парных символов */
#define UMBA_TOKENIZER_CHARCLASS_XDIGIT                         0x0040u
#define UMBA_TOKENIZER_CHARCLASS_OPCHAR                         0x0080u
#define UMBA_TOKENIZER_CHARCLASS_PUNCTUATION                    0x0100u
#define UMBA_TOKENIZER_CHARCLASS_DIGIT                          0x0200u
#define UMBA_TOKENIZER_CHARCLASS_ALPHA                          0x0400u
#define UMBA_TOKENIZER_CHARCLASS_UPPER                          0x0800u /* Флаг для символов верхнего регистра */
#define UMBA_TOKENIZER_CHARCLASS_IDENTIFIER                     0x1000u
#define UMBA_TOKENIZER_CHARCLASS_IDENTIFIER_FIRST               0x2000u
#define UMBA_TOKENIZER_CHARCLASS_SEMIALPHA                      0x4000u /* Для символов, которые никуда не вошли, такие как @ # $, буквоподобные символы */
#define UMBA_TOKENIZER_CHARCLASS_ESCAPE                         0x8000u /* Для символа '\', который везде используется как escape-символ */

// Obsolete
// #define UMBA_TOKENIZER_CHARCLASS_QUOT             0x0040u
// #define UMBA_TOKENIZER_CHARCLASS_TAB              0x0008u
// #define UMBA_TOKENIZER_CHARCLASS_BRACE            0x0040u /* Отдельно brace флаг не надо, наличие либо open, либо close - само по себе признак скобки */
// #define UMBA_TOKENIZER_CHARCLASS_BRACE

/*
   # https://ru.wikipedia.org/wiki/%D0%97%D0%BD%D0%B0%D0%BA_%D1%80%D0%B5%D1%88%D1%91%D1%82%D0%BA%D0%B8
   $ https://ru.wikipedia.org/wiki/%D0%A1%D0%B8%D0%BC%D0%B2%D0%BE%D0%BB_%D0%B4%D0%BE%D0%BB%D0%BB%D0%B0%D1%80%D0%B0
   @ https://ru.wikipedia.org/wiki/@
*/



// https://www.geeksforgeeks.org/inline-function-in-c/
// https://stackoverflow.com/questions/31108159/what-is-the-use-of-the-inline-keyword-in-c

#if !defined(UMBA_TOKENIZER_ENABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION)
    #if !defined(UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION)
        #define UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION
    #endif
#endif

#if !defined(UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION)
static inline
UMBA_TOKENIZER_CHARCLASS umbaTokenizerGetCharClass(char ch)
{
    static
    #include "c_char_class_table.h.inc"

    size_t idx = (size_t)(unsigned char)ch;
    if (idx>=0x80u)
        idx = 0x7Fu;

    return charClassesTable[idx];
}
#endif

