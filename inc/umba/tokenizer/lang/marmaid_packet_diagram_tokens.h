/*! \file
    \brief Определение токенов, специфичных для marmaid packet diagram
 */

#pragma once


//----------------------------------------------------------------------------
// 0x4000u
#define MARMAID_TOKEN_SET_DIRECTIVES                           UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST
#define MARMAID_TOKEN_SET_DIRECTIVES_FIRST                     UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_FIRST
#define MARMAID_TOKEN_SET_DIRECTIVES_LAST                      UMBA_TOKENIZER_TOKEN_KEYWORD_SET1_LAST

// 0x4400u
#define MARMAID_TOKEN_SET_TYPES                                UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST
#define MARMAID_TOKEN_SET_TYPES_FIRST                          UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_FIRST
#define MARMAID_TOKEN_SET_TYPES_LAST                           UMBA_TOKENIZER_TOKEN_KEYWORD_SET2_LAST

#define MARMAID_TOKEN_SET_ATTRS                                UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST
#define MARMAID_TOKEN_SET_ATTRS_FIRST                          UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_FIRST
#define MARMAID_TOKEN_SET_ATTRS_LAST                           UMBA_TOKENIZER_TOKEN_KEYWORD_SET3_LAST

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Флаги и размер типа кодируем прямо в идентификаторе токена

// Флаги примитивных типов
#define MARMAID_PRIM_TYPE_FLAG_UNSIGNED                        0x080u
#define MARMAID_PRIM_TYPE_FLAG_INTEGRAL                        0x040u
#define MARMAID_PRIM_TYPE_FLAG_CHAR                            0x020u

// 17921 = 0x4601

//------------------------------
// char у нас знаковый
#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_CHAR                 (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x001u|MARMAID_PRIM_TYPE_FLAG_CHAR)

#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT8                 (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x001u)
#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT16                (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x002u)
#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT32                (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x004u)
#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_INT64                (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x008u)

#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT8                (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x001u|MARMAID_PRIM_TYPE_FLAG_UNSIGNED)
#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT16               (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x002u|MARMAID_PRIM_TYPE_FLAG_UNSIGNED)
#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT32               (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x004u|MARMAID_PRIM_TYPE_FLAG_UNSIGNED)
#define MARMAID_PACKET_DIAGRAM_TOKEN_TYPE_UINT64               (MARMAID_TOKEN_SET_TYPES|MARMAID_PRIM_TYPE_FLAG_INTEGRAL|0x008u|MARMAID_PRIM_TYPE_FLAG_UNSIGNED)



#define MARMAID_TOKEN_SET_OPERATORS                            UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST
#define MARMAID_TOKEN_SET_OPERATORS_FIRST                      UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST
#define MARMAID_TOKEN_SET_OPERATORS_LAST                       ((UMBA_TOKENIZER_TOKEN_OPERATOR_USER_FIRST)+0x1FF)

// Двоеточие
#define MARMAID_TOKEN_OPERATOR_FOLLOW_DELIMITER               ((MARMAID_TOKEN_SET_OPERATORS_FIRST)+1)
#define MARMAID_TOKEN_OPERATOR_RANGE                          ((MARMAID_TOKEN_SET_OPERATORS_FIRST)+2)
#define MARMAID_TOKEN_OPERATOR_EXTRA                          ((MARMAID_TOKEN_SET_OPERATORS_FIRST)+3)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARMAID_TOKEN_DIRECTIVE_PACKET_BETA                    (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x001u)  /* packet-beta */
#define MARMAID_TOKEN_DIRECTIVE_TITLE                          (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x002u)  /* title  */
#define MARMAID_TOKEN_DIRECTIVE_NATIVE                         (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x003u)  /* native */
#define MARMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH                  (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x004u)  /* width  */
#define MARMAID_TOKEN_DIRECTIVE_ORG                            (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x005u)  /* width  */
// #define MARMAID_TOKEN_DIRECTIVE_                          (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x000u)  /*  */

//----------------------------------------------------------------------------
#define MARMAID_TOKEN_ATTR_BYTE_DIA                       (MARMAID_TOKEN_SET_ATTRS_FIRST+0x001u)  /* byte-diagram */
#define MARMAID_TOKEN_ATTR_BIT_DIA                        (MARMAID_TOKEN_SET_ATTRS_FIRST+0x002u)  /* bit-diagram */
#define MARMAID_TOKEN_ATTR_MEMORY_DIA                     (MARMAID_TOKEN_SET_ATTRS_FIRST+0x003u)  /* memory-diagram memory-layout memory */
#define MARMAID_TOKEN_ATTR_BYTE                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x004u)  /* byte */
#define MARMAID_TOKEN_ATTR_BIT                            (MARMAID_TOKEN_SET_ATTRS_FIRST+0x005u)  /* bit */
#define MARMAID_TOKEN_ATTR_LE                             (MARMAID_TOKEN_SET_ATTRS_FIRST+0x006u)  /* little-endian */
#define MARMAID_TOKEN_ATTR_BE                             (MARMAID_TOKEN_SET_ATTRS_FIRST+0x007u)  /* big-endian */
#define MARMAID_TOKEN_ATTR_ME                             (MARMAID_TOKEN_SET_ATTRS_FIRST+0x008u)  /* middle-endian */
#define MARMAID_TOKEN_ATTR_LE_ME                          (MARMAID_TOKEN_SET_ATTRS_FIRST+0x009u)  /* little-endian-middle-endian */
#define MARMAID_TOKEN_ATTR_BE_ME                          (MARMAID_TOKEN_SET_ATTRS_FIRST+0x00Au)  /* big-endian-middle-endian */
#define MARMAID_TOKEN_ATTR_CRC                            (MARMAID_TOKEN_SET_ATTRS_FIRST+0x00Bu)  /* crc */
#define MARMAID_TOKEN_ATTR_SEED                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x00Cu)  /* seed */
#define MARMAID_TOKEN_ATTR_POLY                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x00Du)  /* poly */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */
// #define MARMAID_TOKEN_ATTR_                               (MARMAID_TOKEN_SET_ATTRS_FIRST+0x000u)  /*  */


/*
    Атрибуты будем вычитывать после оператора 

    packet-beta %%#! byte-diagram, byte, bit-diagram, bit. Может, ещё register? Тогда описание у нас от младших к старшим, 

    %%#! native le 32 bits bit byte bytes - для байтовой диаграммы задаём размер и endiannes по умолчанию

    Атрибуты порядка байт: little-endian, big-endian, little-endian-big-endian, big-endian-little-endian, le, be, le-be, be-le
    могут быть как в директиве native, так и в атрибутах поля.
    le-be, be-le - явно прописывает разделить поле на две половинки, порядок байт в каждой их них как в первой части, ... запутался
    А если так: middle-endian, me и little-endian-middle-endian, big-endian-middle-endian, le-me, be-me - 
    middle-endian - работает относительно нативного порядка байт, если указано в директиве native
    le-me, be-me - говорит нам, что 

    %%#! width 16/32/64 сколько бит/байт умещается в одной строке

    Директива org XXXX - задаёт текущий базовый адрес. Может встречаться более одного раза.
    Надо проверять, если XXX попадает куда-то в уже заполненное пространство, то это ошибка.
    Если пропуск/gap от предыдущего - предупреждение

    %%#! org 0x80000 "Hardware Info"

    Каждый org - стартует отдельную структуру. Если в описании только один org - то генерим одну структуру,
    если несколько - то генерим для каждого org свою структуру. Если нет имени - предупреждаем, имя делаем 
    из тайтла всего файла, добавляя порядковый номер org'а.

    После каждого org'а. нумерация байт идёт с нуля (если данные размечаются числовыми диапазонами, а не типами).

    Надо уметь делать constexpr указатель на всю структуру.

    %%#! org +0x20 - задаём org относительно предыдущего org'а

    По умолчанию, если базовый адрес не задан, добавляем org +0
    Или, если базовый адрес не задан, задаём org 0?
    А если базовый адрес задан, то задаём его в виде элемента с директивой org?

    При добавлении директивы org, если у нас gap с предыдущими данными, выводим не только варнинг, 
    но и дополнительный fill_ элемент, он присутствует в сишной структуре, но игнорируется при генерации
    HEX'а.

    Если мы выводим сишный код для структуры, то нам надо:

     1) нагенерить различные fill_ имена
     2) нагенерить org имена

    Если мы выводим всё по разным структурам,
    то fill'ы вставляются в главной структуре


*/



