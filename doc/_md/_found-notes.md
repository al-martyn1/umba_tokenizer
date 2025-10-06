---
Generator: Umba Brief Scanner
---

# inc/umba/tokenizer

- `[inc/umba/tokenizer/tokenizer_base.h:882]`
  Надо ли semialpha проверять, не является ли она началом числового префикса?
  Наверное, не помешает

- `[inc/umba/tokenizer/tokenizer_base.h:1698]`
  У нас пока так: префикс числа начинается с любой цифры, потом могут следовать
  любые символы, после префикса - те символы, которые разрешены префиксом

- `[inc/umba/tokenizer/tokenizer_base.h:1780]`
  Да, сразу после префикса у нас не может быть разделителя разрядов

