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

- `[inc/umba/tokenizer/tokenizer_base.h:327]`
  Надо вспомнить, почему я сделал DataType##Holder, ведь изначально в variant'е
  были именно типы DataType Скорее всего, variant<DataType> был очень жирный, а
  также его копирование было дорогим. Но это не факт.

- `[inc/umba/tokenizer/tokenizer_base.h:785]`
  return'а не было, был забыт, а что и в каких случаях возращать - я уже не помню

- `[inc/umba/tokenizer/tokenizer_base.h:1244]`
  Тут надо преобразовать префикс в число, и поместить его в numberCurrentIntValue

- `[inc/umba/tokenizer/tokenizer_base.h:1261]`
  Не забыть передать numberCurrentIntValue

- `[inc/umba/tokenizer/tokenizer_base.h:1744]`
  Тут ещё надо проработать - а вдруг это 0 с каким-то суффиксом? по идее, всю
  цепочку итераторов надо бы хранить где-то, чтобы в данном случае сбросить её
  пользователю, а не выдавать ошибку

- `[inc/umba/tokenizer/tokenizer_base.h:1784]`
  тут надо уже начинать подсчитывать std::uint64_t numberCurrentIntValue

- `[inc/umba/tokenizer/tokenizer_base.h:1874]`
  Вот тут гавно какое-то получится, надо подумать

- `[inc/umba/tokenizer/tokenizer_base.h:1881]`
  Добавил

- `[inc/umba/tokenizer/tokenizer_base.h:1892]`
  Добавил

- `[inc/umba/tokenizer/tokenizer_base.h:1900]`
  unreachable code st = TokenizerInternalState::stReadNumberFloat;

- `[inc/umba/tokenizer/tokenizer_base.h:1914]`
  Вот тут гавно какое-то получится, надо подумать

- `[inc/umba/tokenizer/tokenizer_base.h:1948]`
  Вот тут гавно какое-то получится, надо подумать

- `[inc/umba/tokenizer/tokenizer_base.h:2383]`
  Пока IdentifierData задаем как вьюшку от итераторов, но вообще идентификатор
  надо бы сохранять в буфере, чтобы корректно обрабатывать linefeed escap'ы

- `[inc/umba/tokenizer/tokenizer_builder.h:223]`
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

- `[inc/umba/tokenizer/token_collection.h:504]`
  Надо наверное что-то придумать с итератором конца. Или не надо? ,
  m_tokenizer(tknConfigurator(initTokenizerHandlers(std::move(tkn))))



# inc/umba/tokenizer/filters

- `[inc/umba/tokenizer/filters/simple_suffix_gluing_filter.h:138]`
  Зачем я тут копию делаю?



# inc/umba/tokenizer/lexers

- `[inc/umba/tokenizer/lexers/cpp.h:147]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/mermaid_packet_diagram.h:106]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/plantuml.h:178]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/ufsm.h:121]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/usketch_tokenizer.h:193]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/usketch_tokenizer.h:224]`
  Фильтры, установленные позже, отрабатывают раньше

- `[inc/umba/tokenizer/lexers/usketch_tokenizer.h:271]`
  Фильтры, установленные позже, отрабатывают раньше



# inc/umba/tokenizer/parsers/mermaid

- `[inc/umba/tokenizer/parsers/mermaid/packet_diagram_parser.h:1468]`
  Тут надо делать селект - если токен не числовой внутри орга, то литеральный
  глобальный Пока только числовой делаем

- `[inc/umba/tokenizer/parsers/mermaid/packet_diagram_parser.h:1490]`
  Надо разобраться, почему диагностика не туда пырит



# inc/umba/tokenizer/parsers/ufsm

- `[inc/umba/tokenizer/parsers/ufsm/parser.h:44]`
  В definitions нельзя добавлять переходы. Сейчас это не проверяется

- `[inc/umba/tokenizer/parsers/ufsm/parser.h:46]`
  Целевое состояние перехода может быть self - надо обработать Тут ещё такой
  нюанс. Если используется self - то действия состояния (state actions) -
  используются действия self-enter/self-leave. Если целевым состоянием перехода
  задано именованное состояние, то, даже если целевое состояние совпадает с
  исходным, то используются действия enter/leave.

- `[inc/umba/tokenizer/parsers/ufsm/parser.h:52]`
  Если у нас список исходных состояний, или там есть ANY-состояние, а в качестве
  целевого состояния задан self - то всё нормально, сложно-составной переход
  будет разложен на элементарные, с одним исх. состоянием, и с одним событием, и
  там self будет понятно куда переходит.

- `[inc/umba/tokenizer/parsers/ufsm/parser.h:1334]`
  Нужно проверить наличие этого флага, если уже установлен, то это ошибка Пока
  просто устанавливаем без проверки

