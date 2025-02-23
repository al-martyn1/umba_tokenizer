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
#define MARMAID_TOKEN_OPERATOR_PLUS                           ((MARMAID_TOKEN_SET_OPERATORS_FIRST)+4)

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
#define MARMAID_TOKEN_DIRECTIVE_PACKET_BETA                    (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x001u)  /* packet-beta */
#define MARMAID_TOKEN_DIRECTIVE_TITLE                          (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x002u)  /* title  */
#define MARMAID_TOKEN_DIRECTIVE_NATIVE                         (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x003u)  /* native */
#define MARMAID_TOKEN_DIRECTIVE_DISPLAY_WIDTH                  (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x004u)  /* width  */
#define MARMAID_TOKEN_DIRECTIVE_ORG                            (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x005u)  /* width  */
// #define MARMAID_TOKEN_DIRECTIVE_                          (MARMAID_TOKEN_SET_DIRECTIVES_FIRST+0x000u)  /*  */

//----------------------------------------------------------------------------
#define MARMAID_TOKEN_ATTR_CHECKSUM                       (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0000u)  /*  */
#define MARMAID_TOKEN_ATTR_SIMPLE_SUM                     (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0001u)  /*  */
#define MARMAID_TOKEN_ATTR_SIMPLE_SUM_COMPLEMENT          (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0002u)  /*  */
#define MARMAID_TOKEN_ATTR_SIMPLE_SUM_INVERT              (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0003u)  /*  */
#define MARMAID_TOKEN_ATTR_SIMPLE_XOR                     (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0004u)  /*  */
#define MARMAID_TOKEN_ATTR_SIMPLE_XOR_COMPLEMENT          (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0005u)  /*  */
#define MARMAID_TOKEN_ATTR_SIMPLE_XOR_INVERT              (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0006u)  /*  */
#define MARMAID_TOKEN_ATTR_CRC                            (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0007u)  /* crc */
#define MARMAID_TOKEN_ATTR_SEED                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0008u)  /* seed */
#define MARMAID_TOKEN_ATTR_POLY                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0009u)  /* poly */




#define MARMAID_TOKEN_ATTR_AUTO                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0100u)  /* auto */
#define MARMAID_TOKEN_ATTR_BYTE_DIA                       (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0101u)  /* byte-diagram */
#define MARMAID_TOKEN_ATTR_BIT_DIA                        (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0102u)  /* bit-diagram */
#define MARMAID_TOKEN_ATTR_MEMORY_DIA                     (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0103u)  /* memory-diagram memory-layout memory */
#define MARMAID_TOKEN_ATTR_BYTE                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0104u)  /* byte */
#define MARMAID_TOKEN_ATTR_BIT                            (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0105u)  /* bit */
#define MARMAID_TOKEN_ATTR_LE                             (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0106u)  /* little-endian */
#define MARMAID_TOKEN_ATTR_BE                             (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0107u)  /* big-endian */
#define MARMAID_TOKEN_ATTR_ME                             (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0108u)  /* middle-endian */
#define MARMAID_TOKEN_ATTR_LE_ME                          (MARMAID_TOKEN_SET_ATTRS_FIRST+0x0109u)  /* little-endian-middle-endian */
#define MARMAID_TOKEN_ATTR_BE_ME                          (MARMAID_TOKEN_SET_ATTRS_FIRST+0x010Au)  /* big-endian-middle-endian */
#define MARMAID_TOKEN_ATTR_ASCII_Z                        (MARMAID_TOKEN_SET_ATTRS_FIRST+0x010Bu)  /* ascii-z */
#define MARMAID_TOKEN_ATTR_SEGMENT                        (MARMAID_TOKEN_SET_ATTRS_FIRST+0x010Cu)  /* segment */
#define MARMAID_TOKEN_ATTR_SEGMENT_SHIFT                  (MARMAID_TOKEN_SET_ATTRS_FIRST+0x010Du)  /* segment-shift */
#define MARMAID_TOKEN_ATTR_OFFSET                         (MARMAID_TOKEN_SET_ATTRS_FIRST+0x010Eu)  /* offset */
#define MARMAID_TOKEN_ATTR_DATA                           (MARMAID_TOKEN_SET_ATTRS_FIRST+0x010Fu)  /* data */





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

    %%#! native le 32 bits bit byte bytes - для байтовой диаграммы задаём размер и endianness по умолчанию

    data 32 bit address 32 bit
    Если у нас сегментная модель, то надо задать размер сегмента, смещение сегмента и размер офсета.
    Если у нас линейная модель, то нам не важна разрядность памяти.
    native le data 32 bit segment 16 bit segment-shift 1 byte offset 16 bit

    segmented-memory - атрибут архитектуры (native) - как назвать одним словом? segmented? Хотя, можно и двумя - segmented-memory тоже норм.
    Пока не реализовано.
    Если буду реализовывать, надо будет проверить все вычисления адресов - младшие 16 бит не переносяться в старшие, а заворачиваются.
    Чтение адреса в директивах org сделать сегментированное XXXX:XXXX

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

    А может, в директиве org сделать другой синтаксис?
    %%#! org 0x20 - это смещение от предыдущего org
    %%#! org .0x800200 - это абсолютный org? Типа, точка, всё абсолютно

    %%#! org auto "Some Entry" - автоматическое вычисление адреса
    OrgType orgAuto, orgAbs, orgRel

    Если мы выводим сишный код для структуры, то нам надо:

     1) нагенерить различные fill_ имена
     2) нагенерить org имена

    Если мы выводим всё по разным структурам,
    то fill'ы вставляются в главной структуре

    CRC range - как работает? Если числовой рендж, то внутри текущего org?
    Если имена/литералы, то глобально 

    checksum crc
    checksum simple-sum
    и тп

    Сделать литералы для crc.
    Сделать задание алгоритма
      Простые алгоритмы. Работают побайтно и выдают 8 битную контрольную сумму
        simple-sum            - сумма байт
        simple-sum-complement - сумма байт с дополнением до 2 (вычитаем из нуля)
        simple-sum-invert     - сумма байт с инверсией
        simple-xor            - xor побайтный
        simple-xor-complement - xor побайтный с дополнением до 2 (вычитаем из нуля)
        simple-xor-invert     - xor побайтный с инверсией

    Adler-32 - https://ru.wikipedia.org/wiki/Adler-32
    Fletcher - https://ru.wikipedia.org/wiki/%D0%9A%D0%BE%D0%BD%D1%82%D1%80%D0%BE%D0%BB%D1%8C%D0%BD%D0%B0%D1%8F_%D1%81%D1%83%D0%BC%D0%BC%D0%B0_%D0%A4%D0%BB%D0%B5%D1%82%D1%87%D0%B5%D1%80%D0%B0




*/



