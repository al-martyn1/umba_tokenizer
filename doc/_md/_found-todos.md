---
Generator: Umba Brief Scanner
---

# inc/umba/tokenizer

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:558]`
  Надо определится, что делать в tokenize в raw режиме. См там коментарии

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:1257]`
  У нас был встречен символ разделяющий целую и дробную часть плавающего числа,
  но без целой части. Пока просто съедаем его, но вообще этот символ может быть
  оператором, надо это обработать Например, это может быть паскалевский `End.`
  (End с точкой)

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:1296]`
  Надо уточнить, что за комент

- [ ] `[inc/umba/tokenizer/tokenizer_base.h:2070]`
  Разобраться с continuation

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:210]`
  Проверить, что pairBaseToken один из: UMBA_TOKENIZER_TOKEN_CURLY_BRACKETS
  UMBA_TOKENIZER_TOKEN_ROUND_BRACKETS UMBA_TOKENIZER_TOKEN_ANGLE_BRACKETS
  UMBA_TOKENIZER_TOKEN_SQUARE_BRACKETS или 0, для автоопределения, и сделать
  автоопределение

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:247]`
  Подумать, как сделать, чтобы числа можно было начинать с символов @ # $

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:248]`
  Проверить tokenId на вхождение в диапазон, или сделать автоопределение

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:265]`
  Проверить tokenId на вхождение в диапазон, или сделать автоопределение

- [ ] `[inc/umba/tokenizer/tokenizer_builder.h:309]`
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

- [ ] `[inc/umba/tokenizer/token_collection.h:293]`
  запретить все остальные конструкторы

- [ ] `[inc/umba/tokenizer/token_collection.h:294]`
  запретить копирование

- [ ] `[inc/umba/tokenizer/token_collection.h:681]`
  может не надо в getTokenImpl проверять доступность? Или пофик, одно условие,
  зато peekToken() упрощается

- [ ] `[inc/umba/tokenizer/token_collection.h:756]`
  Вот тут надо вычитать следующий токен из входного текста

- [ ] `[inc/umba/tokenizer/token_collection.h:803]`
  надо как-то просигналить, потому что снаружы мы генерим только ошибки
  синтаксического анализа а ошибки лексера генерируются им самим. Но раз лексер
  вернул true, то там ошибки не было, но, тем не менее, финальный токен не
  появился в массиве токенов - что-то пошло не так. unexpected-что?



# inc/umba/tokenizer/filters

- [ ] `[inc/umba/tokenizer/filters/cc_preprocessor_filter.h:42]`
  Вообще, по уму, надо бы сделать энум со значениями inDefine, inPragma, inError,
  inWarning, inCondition

