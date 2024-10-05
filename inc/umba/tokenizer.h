#pragma once

#ifndef UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__
#define UMBA_TOKENIZER_H__DCAEEDE0_7624_4E47_9919_08460EF65A3B__


#if defined(UMBA_TOKENIZER_TYPES_COMPACT)
    #if !defined(UMBA_CHAR_CLASS_UNDERLYING_COMPACT)
        #define UMBA_CHAR_CLASS_UNDERLYING_COMPACT
    #endif
#endif

#if defined(UMBA_CHAR_CLASS_UNDERLYING_COMPACT)
    #if !defined(UMBA_TOKENIZER_TYPES_COMPACT)
        #define UMBA_TOKENIZER_TYPES_COMPACT
    #endif
#endif

#if !defined(__cplusplus)
    #include <stddef.h>
    #include <stdint.h>
#else
    #include <cstddef>
    #include <cstdint>
#endif

#include <iterator>


//----------------------------------------------------------------------------
#include "char_class.h"
//
#include "c_tokenizer.h"

//
#include "c_tokenizer_constants.h"
//
#include "iterator.h"
//
#include "text_iterator_traits.h"
//
#include "escape_string.h"

//
#include <map>
#include <deque>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <type_traits>

//
#include "assert.h"

//
#include "tokenizer/defs.h"
//
#include "tokenizer/utils.h"
//
#include "tokenizer/trie_builder.h"
//
#include "tokenizer/string_literal_parsing.h"
//
#include "tokenizer/tokenizer_options.h"
//
#include "tokenizer/tokenizer_base.h"
//
#include "tokenizer/tokenizer_function_handlers.h"
//
#include "tokenizer/tokenizer.h"
//
#include "tokenizer/tokenizer_builder.h"






/*
    1) Сделать папку tokenizer, там насоздавать файлов, и раскидать по ним содержимое данного файла
    2) Делаем TokenizerBuilder - туда всё присовываем
       Он параметризован по самые нехочухи
       Умеет генерить сорсы для инициализации токенизера
    3) Делаем Tokenizer. Он тоже параметризован по самые нехочухи.
       Его умеет делать TokenizerBuilder
       Также Tokenizer можно сделать стандалоном, присуть туда сгенерённые trie, и наслаждаться.
       Внимание надо обратить на trie литерал префиксов - там флаги содержат адреса литерал парсеров,
       когда создаётся токенизер без билдера - надо задать их явно
 */

//----------------------------------------------------------------------------
// boost::tokenizer (где-то в середине)
//  Перевод  - https://habr.com/ru/companies/piter/articles/650945/
//  Оригинал - https://www.j-labs.pl/blog-technologiczny/find-a-better-solution-with-boost/

//----------------------------------------------------------------------------
// Наткнулся тут
// #include "microcode_tokenizer.h"
// В каком-то проекте же делал, надо поискать, что там

//----------------------------------------------------------------------------


// umba::tokenizer::
namespace umba {
namespace tokenizer {



//----------------------------------------------------------------------------
// Tokenizer

// TrieBuilder
/*

Разбираем текст на токены.

Какие бывают токены.

Пробелы - это сами пробелы, табуляция, переводы строки и тп.

Операторы. Это все символы пунктуации, операторов, скобок, брейсов и тп в различных комбинациях. (или скобки, брейсы - не операторные символы?)

Литералы.

  Литералы числовые - тут всё понятно

  Литералы строковые.

      Примитивные варианты - бэктик, apos и quot - начинаются с определённого символа

      Сложные варианты - начинаются с последовательности символов

      Очень сложные варианты - начинаются с последовательности символов, часть из которых вариативная, и участвует в определении конца литерала


Что мы будем делать?

Пока не будем парится по сложным строковым литералам.
Также не будем поддерживать экспоненциальную форму чисел с плавающей точкой.


1) Делаем таблицу, из которой получаем класс символа
   Таблица - это 128 позиций на базоые ASCII-символы
   Делаем это не вручную.
   Таблица генерируется не константная, надо уметь менять её в рантайме - например,
   чтобы управлять поведением символов $/@ в зависимости от контекста - то ли они могутт быть в идентификаторе, то ли нет

   а) Имеем список операторов с названиями - "!==" -> "STRICT_NEQ"
      Все операторы разбираем посимвольно, для каждого символа ставим флаг CharClass::opchar

   б) Всё, что меньше пробела - флаг nonprintable, а также 0x7F
      \r, \n - linefeed
      \t     - tab
      \r, \n, \t, ' ' - space

   в) кавычка, двойная кавычка, бэктик - quot
   г) Для {}()<>[] - расставляем флаги brace, open, close
   д) ! ? , . ( ) - punctuation (что ещё?) - можно добавлять/менять в рантайме
   е) A-Za-z - alpha, identifier, identifier_first
   ж) 0-9    - digit, identifier
   з) '-'    - hyphen, opchar
   и) '_'    - underscore, identifier, identifier_first


2) Для каждого opchar приписываем ему индекс.

   Также у нас есть таблица операторов. Что там?

   Как мы обрабатываем операторы.
   У нас есть длина текущего оператора. Изначально она равна нулю.
   Получили opchar. Лезем в первую таблицу (по индексу 0).
   Видим там идентификатор оператора. Также видим флаг, что может быть продолжение.
   Тогда ждём следующего символа.
   Если нет продолжения, то сразу выплёвываем токен (если он там есть, иначе - ошибка).


   Зачем бы это делать? Уменьшить

  opchar - нельзя менять в рантайме



Операторы (всевозможные)

@
$
===
!==
<-
---

*/


} // namespace tokenizer
} // namespace umba


#endif /* UMBA_TOKENIZER__DCAEEDE0_7624_4E47_9919_08460EF65A3B__ */

