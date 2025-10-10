---
Generator: Umba Brief Scanner
---

# inc/umba/tokenizer

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:579]`
  Надо определится, что делать в tokenize в raw режиме. См там коментарии

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:1280]`
  У нас был встречен символ разделяющий целую и дробную часть плавающего числа,
  но без целой части. Пока просто съедаем его, но вообще этот символ может быть
  оператором, надо это обработать Например, это может быть паскалевский `End.`
  (End с точкой)

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:1319]`
  Надо уточнить, что за комент

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:2093]`
  Разобраться с continuation

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:216]`
  Проверить, что pairBaseToken один из: UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS
  UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS UMBA_TOKENIZER_TOKEN_ANGLE_BRACKETS
  UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS или 0, для автоопределения, и сделать
  автоопределение

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:253]`
  Подумать, как сделать, чтобы числа можно было начинать с символов @ # $

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:254]`
  Проверить tokenId на вхождение в диапазон, или сделать автоопределение

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:271]`
  Проверить tokenId на вхождение в диапазон, или сделать автоопределение

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:315]`
  Проверить tokenId на вхождение в диапазон, или сделать автоопределение

- [ ] `[inc/umba/tokenizer/tokenizer_log.h:163]`
  Тут надо бы сделать итерацию по символам текста, а не по char'ам

- [ ] `[inc/umba/tokenizer/token_collection.h:155]`
  Ещё забыт вертикальный TAB

- [ ] `[inc/umba/tokenizer/token_collection.h:156]`
  Символы меньше пробела - сейчас по идее, просто пропускаются, наверное надо
  сделать опцию, чтобы они вызывали ошибку

- [ ] `[inc/umba/tokenizer/token_collection.h:268]`
  Надо переделать на unordered_map, чтобы можно было удалять элементы

- [ ] `[inc/umba/tokenizer/token_collection.h:298]`
  запретить все остальные конструкторы

- [ ] `[inc/umba/tokenizer/token_collection.h:299]`
  запретить копирование

- [ ] `[inc/umba/tokenizer/token_collection.h:764]`
  может не надо в getTokenImpl проверять доступность? Или пофик, одно условие,
  зато peekToken() упрощается

- [ ] `[inc/umba/tokenizer/token_collection.h:839]`
  Вот тут надо вычитать следующий токен из входного текста

- [ ] `[inc/umba/tokenizer/token_collection.h:886]`
  надо как-то просигналить, потому что снаружы мы генерим только ошибки
  синтаксического анализа а ошибки лексера генерируются им самим. Но раз лексер
  вернул true, то там ошибки не было, но, тем не менее, финальный токен не
  появился в массиве токенов - что-то пошло не так. unexpected-что?



# inc/umba/tokenizer/filters

- [ ] `[inc/umba/tokenizer/filters/cc_preprocessor_filter.h:42]`
  Вообще, по уму, надо бы сделать энум со значениями inDefine, inPragma, inError,
  inWarning, inCondition



# inc/umba/tokenizer/parsers/ufsm

- [ ] `[inc/umba/tokenizer/parsers/ufsm/http.ufsm:30]`
  Нужно реализовать разбор таких строк Примеры: isAsciiPrnChar  =
  "\a![\x00-\x20]" - все символы базовой ASCII (7 бит), исключая диапазон от нуля
  до пробела isAnyPrnChar    = "\*![\x00-\x20]" - все символы (8 бит), исключая
  диапазон от нуля до пробела isDigit         = "0-9" isDigit         = "\d"
  isHexDigit      = "\da-fA-f" isAlphaUpper    = "A-Z"; isAlphaLower    = "a-z";
  isAlpha = isAlphaUpper | isAlphaLower;



# inc/umba/tokenizer/parsers/ufsm/samples

- [ ] `[inc/umba/tokenizer/parsers/ufsm/samples/http.ufsm:32]`
  Нужно реализовать разбор таких строк Примеры: isAsciiPrnChar  =
  "\a![\x00-\x20]" - все символы базовой ASCII (7 бит), исключая диапазон от нуля
  до пробела isAnyPrnChar    = "\*![\x00-\x20]" - все символы (8 бит), исключая
  диапазон от нуля до пробела isDigit         = "0-9" isDigit         = "\d"
  isHexDigit      = "\da-fA-f" isAlphaUpper    = "A-Z"; isAlphaLower    = "a-z";
  isAlpha = isAlphaUpper | isAlphaLower;

