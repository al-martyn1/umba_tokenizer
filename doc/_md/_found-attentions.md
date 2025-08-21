---
Generator: Umba Brief Scanner
---

# inc/umba

- `[inc/umba/c_tokenizer_constants.h:76]`
  При изменении базовых констант не забываем, что нельзя вылезать за
  UMBA_TOKENIZER_TOKEN_BASE_LAST (или надо поправить эту константу)



# inc/umba/tokenizer

- `[inc/umba/tokenizer/string_literal_parsing.h:518]`
  В MSVC работает, в GCC надо через явное указание базы

- `[inc/umba/tokenizer/tokenizer_base.h:312]`
  Надо вспомнить, почему я сделал DataType##Holder, ведь изначально в variant'е
  были именно типы DataType Скорее всего, variant<DataType> был очень жирный, а
  также его копирование было дорогим. Но это не факт.

- `[inc/umba/tokenizer/tokenizer_base.h:764]`
  return'а не было, был забыт, а что и в каких случаях возращать - я уже не помню

- `[inc/umba/tokenizer/tokenizer_base.h:1221]`
  Тут надо преобразовать префикс в число, и поместить его в numberCurrentIntValue

- `[inc/umba/tokenizer/tokenizer_base.h:1238]`
  Не забыть передать numberCurrentIntValue

- `[inc/umba/tokenizer/tokenizer_base.h:1721]`
  Тут ещё надо проработать - а вдруг это 0 с каким-то суффиксом? по идее, всю
  цепочку итераторов надо бы хранить где-то, чтобы в данном случае сбросить её
  пользователю, а не выдавать ошибку

- `[inc/umba/tokenizer/tokenizer_base.h:1761]`
  тут надо уже начинать подсчитывать std::uint64_t numberCurrentIntValue

- `[inc/umba/tokenizer/tokenizer_base.h:1851]`
  Вот тут гавно какое-то получится, надо подумать

- `[inc/umba/tokenizer/tokenizer_base.h:1858]`
  Добавил

- `[inc/umba/tokenizer/tokenizer_base.h:1869]`
  Добавил

- `[inc/umba/tokenizer/tokenizer_base.h:1877]`
  unreachable code st = TokenizerInternalState::stReadNumberFloat;

- `[inc/umba/tokenizer/tokenizer_base.h:1891]`
  Вот тут гавно какое-то получится, надо подумать

- `[inc/umba/tokenizer/tokenizer_base.h:1925]`
  Вот тут гавно какое-то получится, надо подумать

- `[inc/umba/tokenizer/tokenizer_base.h:2336]`
  Пока IdentifierData задаем как вьюшку от итераторов, но вообще идентификатор
  надо бы сохранять в буфере, чтобы корректно обрабатывать linefeed escap'ы

- `[inc/umba/tokenizer/tokenizer_builder.h:217]`
  Чот какая-то херня непонятная сама ф-я addTokenToKnownSet выглядит так: void
  addTokenToKnownSet(payload_type tk, const string_type &tkStr, bool
  allowExisting=false) А тут мы string_type как-то очень странно конструируем.
  Вроде бы нам надо просто сделать строку из одного символа А тут мы вроде бы
  делаем строки размером bracketsPair[0] и bracketsPair[1] из символов
  pairBaseToken и pairBaseToken+1 Пока закоментим, и сделаем как надо
  addTokenToKnownSet(pairBaseToken  , string_type(bracketsPair[0], pairBaseToken 
  )); addTokenToKnownSet(pairBaseToken+1, string_type(bracketsPair[1],
  pairBaseToken+1));

- `[inc/umba/tokenizer/token_collection.h:217]`
  У нас итераторы довольно жирные (32/64 байта), там овердофига всего хранится
  допустим, токенов у нас в 4 раза меньше, чем текста - если много кода, то много
  мелких токенов - операторов и тп но идентификаторы обычно длиннее 4х символов
  если есть коментарии или строковые литералы - то они разбавляют плотность

- `[inc/umba/tokenizer/token_collection.h:487]`
  Надо наверное что-то придумать с итератором конца. Или не надо? ,
  m_tokenizer(tknConfigurator(initTokenizerHandlers(std::move(tkn))))



# inc/umba/tokenizer/filters

- `[inc/umba/tokenizer/filters/simple_suffix_gluing_filter.h:137]`
  Зачем я тут копию делаю?



# inc/umba/tokenizer/lexers

- `[inc/umba/tokenizer/lexers/cpp.h:147]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/mermaid_packet_diagram.h:103]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/plantuml.h:178]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/ufsm.h:113]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/usketch_tokenizer.h:193]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/usketch_tokenizer.h:224]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/usketch_tokenizer.h:271]`
  Фильтры, установленные позже, отрабатывают раньше



# inc/umba/tokenizer/parsers/mermaid

- `[inc/umba/tokenizer/parsers/mermaid/packet_diagram_parser.h:1461]`
  Тут надо делать селект - если токен не числовой внутри орга, то литеральный
  глобальный Пока только числовой делаем

- `[inc/umba/tokenizer/parsers/mermaid/packet_diagram_parser.h:1483]`
  Надо разобраться, почему диагностика не туда пырит

