# Вопрос

> У нас есть протокол SirfB

```
Start Sequence  Payload Length       Payload Message        Checksum             End Sequence
0xA0, 0xA2     Two-bytes (15-bits)  Up to 2**10-1 (<1023)  Two-bytes (15-bits)  0xB0, 0xB3
Characters preceded by "0x" denotes a hexadecimal value. 0xA0 equals 160.

The validation layer is of part of the transport, but operates independently. The byte
count refers to the payload byte length. The checksum is a sum on the payload

The payload data follows the payload length. It contains the number of bytes specified
by the payload length. The payload data may contain any 8-bit value.

Where multi-byte values are in the payload data, neither the alignment nor the byte order
are defined as part of the transport although SiRF payloads uses the big-endian order. 

The payload length is transmitted high order byte first followed by the low byte
High Byte   Low Byte
<= 0x7F     Any value

The checksum is transmitted high order byte first followed by the low byte. This is the so-called big-endian order.
High Byte   Low Byte
<= 0x7F     Any value

Let message be the array of bytes to be sent by the transport.
Let msgLen be the number of bytes in the message array to be transmitted.
  Index = first
  checkSum = 0
  while index < msgLen
    checkSum = checkSum + message[index]
    checkSum = checkSum AND (215-1).
    increment index

A0A20018 — Start Sequence and Payload Length
810201010001010105010101000100010001000100012580 — Payload
013AB0B3 — Message Checksum and End Sequence 

A0A2002B — Start Sequence and Payload Length
83036000105005010201010448656C6C6F00253264202532642025326420253264202531302E316C660000 — Payload
0867B0B3 — Message Checksum and End Sequence
```

> 
> У нас при описании и реализации ДКА есть четкое разграничение - сначала вычисляются условия перехода,
> затем производится переход - присваивается новое состояние и выполняются действия.
> 
> Но в некоторых случаях нам нужно выполнить действия, и только потом становится понятно, 
> в какое состояние делать переход.
> 
> В данном протоколе мы по преамбуле определяем начало сообщения, после вычитывания его 
> полностью по заданной длине проверяем Checksum и ждём завершение.
> Если у нас на каком-то этапе возникла ошибка, нам нельзя сбрасывать весь буфер, нам 
> надо попробовать найти в нем преамбулу. Преамбула может быть найдена, но может и отсутствовать.
> Тут возникает момент, когда следующее состояние зависит от действия.
> 
> Как быть?
> 
> Можно сделать фейковое состояние, в котором ждём фейковое событие, а это фейковое событие
> генерируется действием по сдвигу буфера. Но это как-то сложновато.
> 
> Второй вариант: делаем предикат - bufHasPreamble, который производит поиск преамбулы без сдвига
> буфера (возможно запоминая расположение преамбулы). В этом случае не надо вводить доп состояния 
> и доп события.
> 
> Второй случай. К нам приходит второй байт длины данных. Одним действием мы кладём его в буфер,
> другим действием формирем число с длиной, и тут нам надо проверить, не превышает ли длина 0x7FFF,
> и перейти в зависимости от этого. Можно сделать состояние READ_FIRST, и переходить не в READ, а туда.
> В READ_FIRST, по приходу байта, дёргать предикат isLengthOk и переходить в различные состояния в 
> зависимости от его значения.
> 
> Или мы создаём состояние WAIT_LL_E (Epsilon), которое обрабатывает только событие epsilon, а все 
> другие события отправляют безусловно в состояние fatal_stop.
> 
> Когда мы выходим из WAIT_LL, мы можем указать действие epsilon. Но куда мы переходим? Мы же ещё не 
> знаем, куда идти.
> 
> Или мы задаём epsilon как предикат в условиях перехода? Сам epsilon предикат удаляем из условия перехода,
> но отмечаем, что по данному условию переходим в epsilon состояние.
> 
> А теперь - epsilon состояние, WAIT_LL_E (Epsilon), например. В этом состоянии обрабатываем только 
> epsilon-событие.
> 
> Ну, хорошо. У нас есть epsilon состояние, WAIT_LL_E (Epsilon), и есть epsilon-событие.
> 
> На примере WAIT_LL, из WAIT_LL_E - тут мы можем перейти либо в какой-то гуд, либо в какой-то бэд, всё 
> определяется аргументом epsilon в предикате базового состояния WAIT_LL. Если в базовом состоянии в предикате 
> перехода есть epsilon(checksumOk), то мы в epsilon состоянии обрабатываем epsilon-событие с предикатом
> checksumOk - и всё вроде бы хорошечно. Но: у нас нет синтаксиса для предикатов с аргументами, в базовом состоянии
> мы не можем одним из условий в логической формуле сделать epsilon(checksumOk).
> 
> Как быть?


Отличный вопрос! Вы столкнулись с классической проблемой в проектировании детерминированных конечных автоматов (ДКА), когда следующее состояние зависит от результата выполнения действий.

Давайте рассмотрим практические решения этой проблемы:

## Решение 1: Введение промежуточных состояний (Рекомендуется)

Создаем специальные состояния для проверки условий:

```python
class ParserState:
    # Основные состояния
    WAIT_PREAMBLE = 0
    WAIT_LENGTH_HIGH = 1
    WAIT_LENGTH_LOW = 2
    WAIT_PAYLOAD = 3
    WAIT_CHECKSUM_HIGH = 4
    WAIT_CHECKSUM_LOW = 5
    WAIT_END = 6
    
    # Промежуточные состояния для проверок
    CHECK_LENGTH = 7      # Проверка корректности длины
    CHECK_CHECKSUM = 8    # Проверка контрольной суммы
    SEARCH_PREAMBLE = 9   # Поиск преамбулы в буфере
```

**Пример реализации:**

```python
def handle_state(current_state, byte):
    if current_state == ParserState.WAIT_LENGTH_LOW:
        # Действие: сохраняем младший байт длины
        self.length_low = byte
        self.total_length = (self.length_high << 8) | self.length_low
        
        # Переход в состояние проверки
        return ParserState.CHECK_LENGTH, self.epsilon_event
    
    elif current_state == ParserState.CHECK_LENGTH:
        # Предикат определяет следующее состояние
        if self.is_length_valid():
            return ParserState.WAIT_PAYLOAD, None
        else:
            # Действие: сброс или поиск преамбулы
            self.reset_buffer()
            return ParserState.SEARCH_PREAMBLE, None
    
    elif current_state == ParserState.SEARCH_PREAMBLE:
        # Действие: поиск преамбулы в буфере
        found_preamble = self.buffer_has_preamble()
        if found_preamble:
            return ParserState.WAIT_LENGTH_HIGH, None
        else:
            # Действие: сдвиг буфера
            self.shift_buffer()
            return ParserState.SEARCH_PREAMBLE, None
```

## Решение 2: Расширенная модель ДКА с условиями переходов

Модифицируем модель ДКА, чтобы разрешить предикаты в переходах:

```python
class Transition:
    def __init__(self, condition, next_state, action=None):
        self.condition = condition  # Функция-предикат
        self.next_state = next_state
        self.action = action  # Функция-действие

class StateMachine:
    def __init__(self):
        self.transitions = {
            ParserState.WAIT_LENGTH_LOW: [
                Transition(condition=lambda: True,  # Безусловный переход
                         next_state=ParserState.CHECK_LENGTH,
                         action=self.calculate_total_length)
            ],
            ParserState.CHECK_LENGTH: [
                Transition(condition=self.is_length_valid,
                         next_state=ParserState.WAIT_PAYLOAD),
                Transition(condition=lambda: not self.is_length_valid(),
                         next_state=ParserState.SEARCH_PREAMBLE,
                         action=self.reset_and_search)
            ]
        }
```

## Решение 3: Epsilon-переходы с аргументами (Более сложное)

Если очень хочется сохранить чистоту ДКА, можно использовать epsilon-события с данными:

```python
class EpsilonEvent:
    def __init__(self, type, data=None):
        self.type = type  # 'checksum_ok', 'length_invalid', etc.
        self.data = data

# В основном цикле обработки
if current_state == ParserState.WAIT_CHECKSUM_LOW:
    checksum_ok = self.verify_checksum()
    epsilon_event = EpsilonEvent('checksum_verified', checksum_ok)
    return ParserState.CHECK_CHECKSUM, epsilon_event

elif current_state == ParserState.CHECK_CHECKSUM:
    if event.type == 'checksum_verified' and event.data:
        return ParserState.WAIT_END, None
    else:
        return ParserState.SEARCH_PREAMBLE, None
```

## Практическая рекомендация

**Используйте Решение 1** (промежуточные состояния) потому что:

1. **Простота понимания** - логика ясна и легко отлаживается
2. **Соответствует практике** - многие промышленные парсеры так работают
3. **Расширяемость** - легко добавлять новые проверки
4. **Тестируемость** - каждое состояние и проверка могут быть протестированы отдельно

**Пример для вашего случая с поиском преамбулы:**

```python
def buffer_has_preamble(self):
    """Поиск преамбулы без изменения буфера"""
    for i in range(len(self.buffer) - 1):
        if self.buffer[i] == 0xA0 and self.buffer[i + 1] == 0xA2:
            self.buffer_start = i  # Запоминаем позицию
            return True
    return False

def handle_search_preamble(self):
    if self.buffer_has_preamble():
        # Нашли преамбулу - переходим к чтению длины
        self.buffer = self.buffer[self.buffer_start:]  # Сдвигаем к началу преамбулы
        return ParserState.WAIT_LENGTH_HIGH
    else:
        # Преамбулы нет - очищаем буфер
        self.buffer.clear()
        return ParserState.SEARCH_PREAMBLE
```

Этот подход сохраняет детерминированность и позволяет выполнять действия перед определением следующего состояния.


# Вопрос

> Распиши решения 2 и 3 максимально подробно. Подумай, нет ли ещё каких-либо вариантов

Рассмотрю решения 2 и 3 максимально подробно, а также предложу дополнительные варианты.

## Решение 2: Расширенная модель ДКА с условиями переходов

### Полная архитектура

```python
from typing import Callable, List, Optional, Any
from enum import Enum
import struct

class ParserState(Enum):
    WAIT_PREAMBLE = 0
    WAIT_LENGTH_HIGH = 1
    WAIT_LENGTH_LOW = 2
    WAIT_PAYLOAD = 3
    WAIT_CHECKSUM_HIGH = 4
    WAIT_CHECKSUM_LOW = 5
    WAIT_END = 6
    ERROR = 7

class Transition:
    """Расширенный класс перехода с условиями и действиями"""
    def __init__(self, 
                 condition: Callable[[Any], bool],
                 next_state: ParserState,
                 action: Optional[Callable[[Any], None]] = None,
                 description: str = ""):
        self.condition = condition
        self.next_state = next_state
        self.action = action
        self.description = description

class SirfBParser:
    def __init__(self):
        self.state = ParserState.WAIT_PREAMBLE
        self.buffer = bytearray()
        self.expected_payload_length = 0
        self.received_payload_length = 0
        self.calculated_checksum = 0
        self.received_checksum = 0
        self.message_complete_callback = None
        
        # Инициализация таблицы переходов
        self.transitions = self._build_transition_table()
    
    def _build_transition_table(self) -> dict:
        """Построение расширенной таблицы переходов"""
        return {
            ParserState.WAIT_PREAMBLE: [
                Transition(
                    condition=lambda byte: len(self.buffer) >= 2 and 
                                         self.buffer[-2] == 0xA0 and 
                                         self.buffer[-1] == 0xA2,
                    next_state=ParserState.WAIT_LENGTH_HIGH,
                    action=self._extract_preamble,
                    description="Найдена преамбула A0 A2"
                ),
                Transition(
                    condition=lambda byte: len(self.buffer) > 100,  # Защита от переполнения
                    next_state=ParserState.WAIT_PREAMBLE,
                    action=self._search_preamble_in_buffer,
                    description="Буфер переполнен - поиск преамбулы"
                )
            ],
            
            ParserState.WAIT_LENGTH_HIGH: [
                Transition(
                    condition=lambda byte: len(self.buffer) >= 2,  # Получили старший байт длины
                    next_state=ParserState.WAIT_LENGTH_LOW,
                    action=self._store_length_high,
                    description="Старший байт длины получен"
                )
            ],
            
            ParserState.WAIT_LENGTH_LOW: [
                Transition(
                    condition=lambda byte: len(self.buffer) >= 2,  # Получили младший байт длины
                    next_state=ParserState.CHECK_LENGTH,
                    action=self._store_length_low_and_calculate,
                    description="Младший байт длины получен"
                )
            ],
            
            ParserState.CHECK_LENGTH: [
                Transition(
                    condition=self._is_length_valid,
                    next_state=ParserState.WAIT_PAYLOAD,
                    action=self._prepare_for_payload,
                    description="Длина корректна"
                ),
                Transition(
                    condition=lambda byte: not self._is_length_valid(),
                    next_state=ParserState.WAIT_PREAMBLE,
                    action=self._handle_invalid_length,
                    description="Некорректная длина"
                )
            ],
            
            ParserState.WAIT_PAYLOAD: [
                Transition(
                    condition=self._is_payload_complete,
                    next_state=ParserState.WAIT_CHECKSUM_HIGH,
                    action=self._calculate_payload_checksum,
                    description="Полезная нагрузка полностью получена"
                ),
                Transition(
                    condition=lambda byte: not self._is_payload_complete(),
                    next_state=ParserState.WAIT_PAYLOAD,
                    action=None,  # Продолжаем накапливать payload
                    description="Продолжаем прием payload"
                )
            ],
            
            ParserState.WAIT_CHECKSUM_HIGH: [
                Transition(
                    condition=lambda byte: True,  # Безусловный переход
                    next_state=ParserState.WAIT_CHECKSUM_LOW,
                    action=self._store_checksum_high,
                    description="Старший байт контрольной суммы получен"
                )
            ],
            
            ParserState.WAIT_CHECKSUM_LOW: [
                Transition(
                    condition=lambda byte: True,  # Безусловный переход
                    next_state=ParserState.CHECK_CHECKSUM,
                    action=self._store_checksum_low,
                    description="Младший байт контрольной суммы получен"
                )
            ],
            
            ParserState.CHECK_CHECKSUM: [
                Transition(
                    condition=self._is_checksum_valid,
                    next_state=ParserState.WAIT_END,
                    action=None,
                    description="Контрольная сумма верна"
                ),
                Transition(
                    condition=lambda byte: not self._is_checksum_valid(),
                    next_state=ParserState.WAIT_PREAMBLE,
                    action=self._handle_invalid_checksum,
                    description="Неверная контрольная сумма"
                )
            ],
            
            ParserState.WAIT_END: [
                Transition(
                    condition=lambda byte: len(self.buffer) >= 2 and 
                                         self.buffer[-2] == 0xB0 and 
                                         self.buffer[-1] == 0xB3,
                    next_state=ParserState.WAIT_PREAMBLE,
                    action=self._handle_complete_message,
                    description="Сообщение полностью принято"
                ),
                Transition(
                    condition=lambda byte: len(self.buffer) > 0 and 
                                         (self.buffer[-1] != 0xB3 or 
                                          (len(self.buffer) >= 2 and 
                                           self.buffer[-2] != 0xB0)),
                    next_state=ParserState.WAIT_PREAMBLE,
                    action=self._handle_invalid_end_sequence,
                    description="Некорректная завершающая последовательность"
                )
            ]
        }
    
    def process_byte(self, byte: int):
        """Основной метод обработки входящих байтов"""
        # Добавляем байт в буфер
        self.buffer.append(byte)
        
        # Получаем возможные переходы для текущего состояния
        possible_transitions = self.transitions.get(self.state, [])
        
        for transition in possible_transitions:
            # Проверяем условие перехода
            if transition.condition(byte):
                # Выполняем действие, если оно определено
                if transition.action:
                    transition.action(byte)
                
                # Выполняем переход
                old_state = self.state
                self.state = transition.next_state
                
                print(f"Переход: {old_state.name} -> {self.state.name} "
                      f"({transition.description})")
                return
        
        # Если не найден подходящий переход, остаемся в текущем состоянии
        print(f"Нет подходящего перехода в состоянии {self.state.name} для байта 0x{byte:02X}")

    # Методы-действия
    def _extract_preamble(self, byte):
        """Извлечение преамбулы из буфера"""
        if len(self.buffer) >= 2:
            # Убеждаемся, что начинаем с преамбулы
            preamble_pos = len(self.buffer) - 2
            if (self.buffer[preamble_pos] == 0xA0 and 
                self.buffer[preamble_pos + 1] == 0xA2):
                # Оставляем только преамбулу в буфере
                self.buffer = self.buffer[preamble_pos:]
                print("Преамбула извлечена, буфер очищен")

    def _store_length_high(self, byte):
        """Сохранение старшего байта длины"""
        if len(self.buffer) >= 2:
            self.length_high = self.buffer[-1]  # Последний байт - старший байт длины
            print(f"Старший байт длины: 0x{self.length_high:02X}")

    def _store_length_low_and_calculate(self, byte):
        """Сохранение младшего байта длины и вычисление полной длины"""
        if len(self.buffer) >= 2:
            self.length_low = self.buffer[-1]  # Последний байт - младший байт длины
            self.expected_payload_length = (self.length_high << 8) | self.length_low
            print(f"Ожидаемая длина payload: {self.expected_payload_length}")

    def _is_length_valid(self, byte=None):
        """Проверка корректности длины"""
        return (self.expected_payload_length <= 1023 and 
                self.expected_payload_length >= 0)

    def _prepare_for_payload(self, byte):
        """Подготовка к приему полезной нагрузки"""
        self.received_payload_length = 0
        self.calculated_checksum = 0
        print(f"Начинаем прием payload длиной {self.expected_payload_length}")

    def _is_payload_complete(self, byte=None):
        """Проверка завершения приема payload"""
        # +2 потому что после payload сразу начинается checksum
        payload_end_pos = 4 + self.expected_payload_length  # preamble(2) + length(2) + payload
        return len(self.buffer) >= payload_end_pos

    def _calculate_payload_checksum(self, byte):
        """Вычисление контрольной суммы payload"""
        start_pos = 4  # После preamble и длины
        end_pos = 4 + self.expected_payload_length
        
        self.calculated_checksum = 0
        for i in range(start_pos, end_pos):
            self.calculated_checksum = (self.calculated_checksum + self.buffer[i]) & 0x7FFF
        
        print(f"Вычисленная контрольная сумма: 0x{self.calculated_checksum:04X}")

    def _store_checksum_high(self, byte):
        """Сохранение старшего байта контрольной суммы"""
        checksum_high_pos = 4 + self.expected_payload_length
        if len(self.buffer) > checksum_high_pos:
            self.received_checksum_high = self.buffer[checksum_high_pos]

    def _store_checksum_low(self, byte):
        """Сохранение младшего байта контрольной суммы"""
        checksum_low_pos = 4 + self.expected_payload_length + 1
        if len(self.buffer) > checksum_low_pos:
            self.received_checksum_low = self.buffer[checksum_low_pos]
            self.received_checksum = (self.received_checksum_high << 8) | self.received_checksum_low
            print(f"Полученная контрольная сумма: 0x{self.received_checksum:04X}")

    def _is_checksum_valid(self, byte=None):
        """Проверка контрольной суммы"""
        return self.calculated_checksum == self.received_checksum

    def _handle_complete_message(self, byte):
        """Обработка полностью принятого сообщения"""
        print("Сообщение полностью принято и проверено!")
        if self.message_complete_callback:
            self.message_complete_callback(self.buffer)
        self._reset_parser()

    def _reset_parser(self):
        """Сброс парсера"""
        self.buffer.clear()
        self.expected_payload_length = 0
        self.received_payload_length = 0
        self.calculated_checksum = 0
        self.received_checksum = 0

    def _search_preamble_in_buffer(self, byte):
        """Поиск преамбулы в буфере"""
        for i in range(len(self.buffer) - 1):
            if self.buffer[i] == 0xA0 and self.buffer[i + 1] == 0xA2:
                # Нашли преамбулу - обрезаем буфер
                self.buffer = self.buffer[i:]
                print(f"Найдена преамбула в позиции {i}")
                return
        
        # Преамбула не найдена - очищаем буфер
        self.buffer.clear()
        print("Преамбула не найдена - буфер очищен")

    def _handle_invalid_length(self, byte):
        print(f"Некорректная длина: {self.expected_payload_length}")
        self._search_preamble_in_buffer(byte)

    def _handle_invalid_checksum(self, byte):
        print("Неверная контрольная сумма")
        self._search_preamble_in_buffer(byte)

    def _handle_invalid_end_sequence(self, byte):
        print("Некорректная завершающая последовательность")
        self._search_preamble_in_buffer(byte)
```

### Преимущества Решения 2:
- **Четкое разделение** логики переходов и действий
- **Легкость расширения** - добавление новых состояний и переходов
- **Тестируемость** - каждое условие и действие можно тестировать отдельно
- **Читаемость** - таблица переходов дает общее представление о логике

## Решение 3: Epsilon-переходы с аргументами

### Полная архитектура с epsilon-событиями

```python
from typing import Callable, Optional, Any, List
from enum import Enum
from dataclasses import dataclass

@dataclass
class EpsilonEvent:
    """Событие для epsilon-переходов"""
    event_type: str
    data: Any = None
    source_state: Optional['ParserState'] = None

class EpsilonParserState(Enum):
    # Основные состояния
    WAIT_PREAMBLE = 0
    WAIT_LENGTH_HIGH = 1
    WAIT_LENGTH_LOW = 2
    WAIT_PAYLOAD = 3
    WAIT_CHECKSUM_HIGH = 4
    WAIT_CHECKSUM_LOW = 5
    WAIT_END = 6
    
    # Epsilon-состояния (для внутренних переходов)
    EPSILON_CHECK_LENGTH = 100
    EPSILON_CHECK_CHECKSUM = 101
    EPSILON_SEARCH_PREAMBLE = 102
    EPSILON_PROCESS_COMPLETE = 103

class EpsilonSirfBParser:
    def __init__(self):
        self.state = EpsilonParserState.WAIT_PREAMBLE
        self.buffer = bytearray()
        self.expected_payload_length = 0
        self.calculated_checksum = 0
        self.received_checksum = 0
        
        # Очередь epsilon-событий
        self.epsilon_queue = []
        
        # Таблицы переходов
        self.byte_transitions = self._build_byte_transitions()
        self.epsilon_transitions = self._build_epsilon_transitions()
    
    def _build_byte_transitions(self):
        """Таблица переходов по байтам"""
        return {
            EpsilonParserState.WAIT_PREAMBLE: [
                (self._is_preamble_detected, EpsilonParserState.WAIT_LENGTH_HIGH, 
                 self._on_preamble_detected)
            ],
            EpsilonParserState.WAIT_LENGTH_HIGH: [
                (self._always_true, EpsilonParserState.WAIT_LENGTH_LOW, 
                 self._store_length_high)
            ],
            EpsilonParserState.WAIT_LENGTH_LOW: [
                (self._always_true, EpsilonParserState.EPSILON_CHECK_LENGTH, 
                 self._store_length_low_and_trigger_check)
            ],
            EpsilonParserState.WAIT_PAYLOAD: [
                (self._is_payload_complete, EpsilonParserState.WAIT_CHECKSUM_HIGH, 
                 self._calculate_checksum_and_prepare)
            ],
            EpsilonParserState.WAIT_CHECKSUM_HIGH: [
                (self._always_true, EpsilonParserState.WAIT_CHECKSUM_LOW, 
                 self._store_checksum_high)
            ],
            EpsilonParserState.WAIT_CHECKSUM_LOW: [
                (self._always_true, EpsilonParserState.EPSILON_CHECK_CHECKSUM, 
                 self._store_checksum_low_and_trigger_check)
            ],
            EpsilonParserState.WAIT_END: [
                (self._is_end_sequence, EpsilonParserState.EPSILON_PROCESS_COMPLETE, 
                 self._trigger_message_complete),
                (self._is_invalid_end, EpsilonParserState.EPSILON_SEARCH_PREAMBLE, 
                 self._trigger_preamble_search)
            ]
        }
    
    def _build_epsilon_transitions(self):
        """Таблица переходов по epsilon-событиям"""
        return {
            EpsilonParserState.EPSILON_CHECK_LENGTH: [
                (self._is_length_valid_epsilon, EpsilonParserState.WAIT_PAYLOAD, 
                 self._on_length_valid),
                (self._is_length_invalid_epsilon, EpsilonParserState.EPSILON_SEARCH_PREAMBLE, 
                 self._on_length_invalid)
            ],
            EpsilonParserState.EPSILON_CHECK_CHECKSUM: [
                (self._is_checksum_valid_epsilon, EpsilonParserState.WAIT_END, 
                 self._on_checksum_valid),
                (self._is_checksum_invalid_epsilon, EpsilonParserState.EPSILON_SEARCH_PREAMBLE, 
                 self._on_checksum_invalid)
            ],
            EpsilonParserState.EPSILON_SEARCH_PREAMBLE: [
                (self._is_preamble_found_epsilon, EpsilonParserState.WAIT_LENGTH_HIGH, 
                 self._on_preamble_found),
                (self._is_preamble_not_found_epsilon, EpsilonParserState.WAIT_PREAMBLE, 
                 self._on_preamble_not_found)
            ],
            EpsilonParserState.EPSILON_PROCESS_COMPLETE: [
                (self._always_true_epsilon, EpsilonParserState.WAIT_PREAMBLE, 
                 self._on_message_processed)
            ]
        }
    
    def process_byte(self, byte: int):
        """Обработка входящего байта"""
        # Сначала обрабатываем epsilon-события, если они есть
        while self.epsilon_queue and self._is_epsilon_state(self.state):
            self._process_epsilon_event()
        
        # Обрабатываем байт
        self.buffer.append(byte)
        self._process_byte_transition(byte)
        
        # После обработки байта проверяем epsilon-события
        while self.epsilon_queue and self._is_epsilon_state(self.state):
            self._process_epsilon_event()
    
    def _process_byte_transition(self, byte: int):
        """Обработка перехода по байту"""
        transitions = self.byte_transitions.get(self.state, [])
        
        for condition, next_state, action in transitions:
            if condition(byte):
                if action:
                    action(byte)
                old_state = self.state
                self.state = next_state
                print(f"Байт-переход: {old_state.name} -> {self.state.name}")
                return
        
        # Если переход не найден, остаемся в текущем состоянии
        print(f"Нет байт-перехода в состоянии {self.state.name}")
    
    def _process_epsilon_event(self):
        """Обработка epsilon-события"""
        if not self.epsilon_queue:
            return
        
        event = self.epsilon_queue.pop(0)
        transitions = self.epsilon_transitions.get(self.state, [])
        
        for condition, next_state, action in transitions:
            if condition(event):
                if action:
                    action(event)
                old_state = self.state
                self.state = next_state
                print(f"Epsilon-переход: {old_state.name} -> {self.state.name} "
                      f"(событие: {event.event_type})")
                return
        
        print(f"Нет epsilon-перехода для события {event.event_type} в состоянии {self.state.name}")
    
    def _is_epsilon_state(self, state: EpsilonParserState) -> bool:
        """Проверка, является ли состояние epsilon-состоянием"""
        return state.value >= 100
    
    def _add_epsilon_event(self, event_type: str, data: Any = None):
        """Добавление epsilon-события в очередь"""
        event = EpsilonEvent(event_type, data, self.state)
        self.epsilon_queue.append(event)
        print(f"Добавлено epsilon-событие: {event_type}")
    
    # Условия для байт-переходов
    def _is_preamble_detected(self, byte) -> bool:
        return (len(self.buffer) >= 2 and 
                self.buffer[-2] == 0xA0 and 
                self.buffer[-1] == 0xA2)
    
    def _always_true(self, byte) -> bool:
        return True
    
    def _is_payload_complete(self, byte) -> bool:
        payload_start = 4  # После preamble(2) + length(2)
        return len(self.buffer) >= payload_start + self.expected_payload_length
    
    def _is_end_sequence(self, byte) -> bool:
        end_pos = 4 + self.expected_payload_length + 2  # После checksum
        return (len(self.buffer) >= end_pos and 
                self.buffer[end_pos-2] == 0xB0 and 
                self.buffer[end_pos-1] == 0xB3)
    
    def _is_invalid_end(self, byte) -> bool:
        end_pos = 4 + self.expected_payload_length + 2
        return (len(self.buffer) >= end_pos and 
                not (self.buffer[end_pos-2] == 0xB0 and 
                     self.buffer[end_pos-1] == 0xB3))
    
    # Условия для epsilon-переходов
    def _is_length_valid_epsilon(self, event) -> bool:
        return (event.event_type == "check_length" and 
                self.expected_payload_length <= 1023)
    
    def _is_length_invalid_epsilon(self, event) -> bool:
        return (event.event_type == "check_length" and 
                self.expected_payload_length > 1023)
    
    def _is_checksum_valid_epsilon(self, event) -> bool:
        return (event.event_type == "check_checksum" and 
                self.calculated_checksum == self.received_checksum)
    
    def _is_checksum_invalid_epsilon(self, event) -> bool:
        return (event.event_type == "check_checksum" and 
                self.calculated_checksum != self.received_checksum)
    
    def _is_preamble_found_epsilon(self, event) -> bool:
        return (event.event_type == "search_preamble" and 
                event.data is not None)  # data содержит позицию преамбулы
    
    def _is_preamble_not_found_epsilon(self, event) -> bool:
        return (event.event_type == "search_preamble" and 
                event.data is None)
    
    def _always_true_epsilon(self, event) -> bool:
        return True
    
    # Действия для байт-переходов
    def _on_preamble_detected(self, byte):
        self.buffer = self.buffer[-2:]  # Оставляем только преамбулу
    
    def _store_length_high(self, byte):
        self.length_high = self.buffer[-1]
    
    def _store_length_low_and_trigger_check(self, byte):
        self.length_low = self.buffer[-1]
        self.expected_payload_length = (self.length_high << 8) | self.length_low
        self._add_epsilon_event("check_length", self.expected_payload_length)
    
    def _calculate_checksum_and_prepare(self, byte):
        start_pos = 4
        end_pos = 4 + self.expected_payload_length
        self.calculated_checksum = 0
        for i in range(start_pos, end_pos):
            self.calculated_checksum = (self.calculated_checksum + self.buffer[i]) & 0x7FFF
    
    def _store_checksum_high(self, byte):
        checksum_pos = 4 + self.expected_payload_length
        self.received_checksum_high = self.buffer[checksum_pos]
    
    def _store_checksum_low_and_trigger_check(self, byte):
        checksum_pos = 4 + self.expected_payload_length + 1
        self.received_checksum_low = self.buffer[checksum_pos]
        self.received_checksum = (self.received_checksum_high << 8) | self.received_checksum_low
        self._add_epsilon_event("check_checksum", 
                               (self.calculated_checksum, self.received_checksum))
    
    def _trigger_message_complete(self, byte):
        self._add_epsilon_event("message_complete", self.buffer.copy())
    
    def _trigger_preamble_search(self, byte):
        self._add_epsilon_event("search_preamble")
    
    # Действия для epsilon-переходов
    def _on_length_valid(self, event):
        print(f"Длина корректна: {self.expected_payload_length}")
    
    def _on_length_invalid(self, event):
        print(f"Некорректная длина: {self.expected_payload_length}")
        self._search_preamble()
    
    def _on_checksum_valid(self, event):
        print("Контрольная сумма верна")
    
    def _on_checksum_invalid(self, event):
        calc, recv = event.data
        print(f"Неверная контрольная сумма: вычислено {calc}, получено {recv}")
        self._search_preamble()
    
    def _on_preamble_found(self, event):
        pos = event.data
        self.buffer = self.buffer[pos:]
        print(f"Преамбула найдена в позиции {pos}")
    
    def _on_preamble_not_found(self, event):
        self.buffer.clear()
        print("Преамбула не найдена - буфер очищен")
    
    def _on_message_processed(self, event):
        message_data = event.data
        print("Сообщение обработано успешно")
        self.buffer.clear()
        self.expected_payload_length = 0
        self.calculated_checksum = 0
        self.received_checksum = 0
    
    def _search_preamble(self):
        """Поиск преамбулы в буфере"""
        for i in range(len(self.buffer) - 1):
            if self.buffer[i] == 0xA0 and self.buffer[i + 1] == 0xA2:
                self._add_epsilon_event("search_preamble", i)
                return
        
        self._add_epsilon_event("search_preamble", None)
```

## Дополнительные варианты

### Вариант 4: Гибридный подход с стейт-машиной и стеком

```python
class StackableParser:
    def __init__(self):
        self.state_stack = []
        self.current_state = ParserState.WAIT_PREAMBLE
        # ... остальная инициализация
    
    def push_state(self, state: ParserState):
        """Сохраняем текущее состояние в стек"""
        self.state_stack.append(self.current_state)
        self.current_state = state
    
    def pop_state(self) -> ParserState:
        """Восстанавливаем состояние из стека"""
        if self.state_stack:
            self.current_state = self.state_stack.pop()
        return self.current_state
    
    def process_with_submachine(self, byte):
        """Обработка с использованием под-автомата"""
        if self.current_state == ParserState.CHECK_LENGTH:
            if self._is_length_valid():
                self.pop_state()  # Возвращаемся к основному состоянию
                return ParserState.WAIT_PAYLOAD
            else:
                self.push_state(ParserState.SEARCH_PREAMBLE)
                return self.process_with_submachine(byte)
```

### Вариант 5: Подход на основе корутин (generator-based)

```python
def parser_coroutine():
    """Парсер на основе корутин"""
    buffer = []
    
    while True:
        byte = yield
        buffer.append(byte)
        
        # WAIT_PREAMBLE
        while len(buffer) >= 2:
            if buffer[-2] == 0xA0 and buffer[-1] == 0xA2:
                buffer = buffer[-2:]  # Оставляем только преамбулу
                break
            else:
                buffer.pop(0)  # Удаляем старые байты
        else:
            continue
        
        # WAIT_LENGTH_HIGH
        if len(buffer) < 2:
            continue
        length_high = buffer[1]
        
        # WAIT_LENGTH_LOW  
        if len(buffer) < 3:
            continue
        length_low = buffer[2]
        
        total_length = (length_high << 8) | length_low
        # ... и так далее
```

### Вариант 6: Декораторный подход

```python
def state_handler(state):
    """Декоратор для регистрации обработчиков состояний"""
    def decorator(func):
        func._handler_state = state
        return func
    return decorator

class DecoratorParser:
    def __init__(self):
        self.state = ParserState.WAIT_PREAMBLE
        self._handlers = self._discover_handlers()
    
    def _discover_handlers(self):
        """Автоматическое обнаружение обработчиков состояний"""
        handlers = {}
        for method_name in dir(self):
            method = getattr(self, method_name)
            if hasattr(method, '_handler_state'):
                handlers[method._handler_state] = method
        return handlers
    
    @state_handler(ParserState.WAIT_PREAMBLE)
    def handle_wait_preamble(self, byte):
        # Логика обработки
        if self._is_preamble_detected():
            return ParserState.WAIT_LENGTH_HIGH
        return ParserState.WAIT_PREAMBLE
    
    def process_byte(self, byte):
        handler = self._handlers.get(self.state)
        if handler:
            new_state = handler(byte)
            self.state = new_state
```

## Рекомендации

**Для промышленного использования** я рекомендую **Решение 2** (расширенная модель ДКА) потому что:
- Баланс между гибкостью и сложностью
- Хорошая тестируемость
- Понятная архитектура для новых разработчиков
- Легкость отладки

**Решение 3** (epsilon-переходы) хорошо подходит для:
- Академических целей
- Сложных протоколов с множеством внутренних состояний
- Систем, где важна формальная верификация

**Решение 1** (промежуточные состояния) - лучший выбор для:
- Простых и средних по сложности протоколов
- Систем с ограниченными ресурсами
- Проектов с жесткими требованиями к производительности


# Вопрос

> Можно расписать предложенные тобой решения не на языке программирования, а на человеческом языке?


Конечно! Распишу все решения на человеческом языке, без программирования.

## Решение 1: Промежуточные состояния (самое простое)

**Основная идея:** Создаем дополнительные "проверочные" состояния, которые только анализируют ситуацию и решают, куда идти дальше.

**Как работает:**
- У нас есть обычные состояния: "Ждем преамбулу", "Ждем длину", "Ждем данные" и т.д.
- Когда нужно принять решение (например, проверить корректность длины), мы переходим в специальное состояние "Проверка длины"
- В этом состоянии мы не ждем новых байтов, а только анализируем то, что уже получили
- На основе анализа выбираем следующее состояние: если длина правильная - идем ждать данные, если нет - идем искать преамбулу заново

**Аналогия из жизни:**
Представьте почтальона:
1. Получает письмо → состояние "Получил письмо"
2. Переходит в состояние "Проверяю адрес" (промежуточное)
3. Если адрес правильный → состояние "Несу письмо адресату"
4. Если адрес неверный → состояние "Ищу правильный адрес"

**Преимущества:** Просто понять, легко отлаживать, подходит для большинства случаев.

## Решение 2: Умные переходы с условиями

**Основная идея:** Каждый переход между состояниями снабжаем "умным условием", которое проверяет не только сам байт, но и контекст.

**Как работает:**
- Для каждого состояния прописываем набор возможных переходов с условиями
- Условия - это маленькие проверки: "длина корректна?", "контрольная сумма совпала?", "найдена ли преамбула в буфере?"
- Когда приходит байт, автомат проверяет все условия по порядку и выбирает подходящий переход
- Действия (например, расчет контрольной суммы) выполняются прямо во время перехода

**Аналогия из жизни:**
Умный светофор:
- Состояние "Красный свет"
- Условия перехода:
  - Если прошло 60 секунд И нет машин скорой помощи → перейти в "Зеленый свет"
  - Если прошло 60 секунд И есть машина скорой помощи → остаться в "Красном свете"
  - Если авария → перейти в "Мигающий желтый"

**Преимущества:** Гибкость, все правила переходов видны в одном месте.

## Решение 3: Epsilon-переходы (внутренние события)

**Основная идея:** Вводим специальные "внутренние события", которые генерируются самим автоматом и не требуют внешних данных.

**Как работает:**
- У нас есть два типа событий: внешние (приход байтов) и внутренние (epsilon-события)
- Epsilon-события генерируются действиями: "проверил длину", "рассчитал контрольную сумму", "нашел преамбулу"
- Специальные epsilon-состояния обрабатывают только эти внутренние события
- Автомат сам себе создает события и сам же на них реагирует

**Пример процесса:**
1. Получили младший байт длины → действие: "рассчитать полную длину и создать epsilon-событие 'проверить_длину'"
2. Переходим в состояние "Обработка проверки длины" (epsilon-состояние)
3. Здесь обрабатываем событие 'проверить_длину':
   - Если длина OK → переходим в "Ждем данные"
   - Если длина плохая → переходим в "Поиск преамбулы"

**Аналогия из жизни:**
Заводской конвейер:
- Рабочий получает деталь (внешнее событие)
- Выполняет действие: сверяет с чертежом (создает внутреннее событие "деталь проверена")
- Передает деталь контролеру (epsilon-состояние)
- Контролер решает: деталь годная → на сборку, брак → в утиль

**Преимущества:** Чистая архитектура, хорошо для сложных протоколов.

## Решение 4: Стек состояний (вложенные автоматы)

**Основная идея:** Автомат может "запоминать" текущее состояние и временно переключаться на решение подзадачи.

**Как работает:**
- Когда нужно принять решение (проверить длину, найти преамбулу), мы "ставим на паузу" текущее состояние
- Переходим в состояние-помощник, которое решает конкретную задачу
- После решения задачи возвращаемся к прерванному состоянию с готовым ответом

**Пример:**
1. Состояние "Ждем младший байт длины" → получили байт
2. Сохраняем текущее состояние в стек ("Ждем младший байт длины" - пауза)
3. Переходим в "Проверка длины" (под-автомат)
4. После проверки извлекаем состояние из стека и продолжаем с учетом результата

**Аналогия из жизни:**
Разговор по телефону:
- Вы говорите с коллегой (основное состояние)
- Вам приходит СМС (внешнее событие)
- Вы говорите: "Извини, секундочку" (сохраняем состояние разговора)
- Читаете СМС (переход в под-состояние)
- Возвращаетесь к разговору: "Так, о чем мы?" (восстанавливаем состояние)

## Решение 5: Корутины (последовательные шаги)

**Основная идея:** Представляем парсер как последовательность шагов, которые выполняются друг за другом.

**Как работает:**
- Парсер - это цепочка последовательных действий
- Каждый шаг ждет, когда у него будет достаточно данных для работы
- Если данных не хватает - просто ждет следующих байтов
- Нет явных состояний - есть позиция в алгоритме

**Аналогия из жизни:**
Приготовление рецепта:
1. Жду, когда будут все ингредиенты
2. Когда есть - смешиваю их
3. Жду, когда духовка нагреется
4. Когда нагрелась - ставлю выпекаться
5. И т.д.

## Решение 6: Декораторы (автоматическая регистрация)

**Основная идея:** Каждое состояние объявляется как отдельная функция с пометкой "я обрабатываю такое-то состояние".

**Как работает:**
- Для каждого состояния пишется отдельная функция-обработчик
- Каждая функция помечается специальной меткой (@state_handler)
- Автомат сам находит все обработчики и знает, какую функцию вызывать для каждого состояния

**Аналогия из жизни:**
Больница с отделениями:
- Каждая табличка "Травматология", "Кардиология" etc. - это метка
- Пациента (байт) направляют в нужное отделение автоматически
- В каждом отделении свои правила обработки

## Какой вариант выбрать?

**Для вашего случая с протоколом SiRF:**

**Начинающим** → Решение 1 (промежуточные состояния)
- Самый простой для понимания
- Легко отлаживать
- Хорошо документируется

**Для промышленного использования** → Решение 2 (умные переходы)
- Баланс простоты и гибкости
- Легко добавлять новые проверки
- Хорошая структура

**Для сложных протоколов** → Решение 3 (epsilon-переходы)
- Чистая архитектура
- Хорошо масштабируется
- Подходит для формальной верификации

**Конкретно для проблемы поиска преамбулы** в вашем протоколе лучше всего подходят Решения 1 или 2, поскольку они наиболее наглядно показывают логику "нашел преамбулу - обрабатываю сообщение, не нашел - ищу дальше".


# Вопрос

> В решении 1 мы переходим в состояние "проверка длины" и находимся в нём, до каких пор? 
> "В этом состоянии мы не ждем новых байтов, а только анализируем то, что уже получили" - просто так автомат не работает, он только реагирует на обытия, и сам по себе ничего анализировать не может и не будет..


Вы абсолютно правы! Я допустил концептуальную ошибку. В классическом ДКА состояния действительно только **реагируют на события**, а не выполняют самостоятельный анализ. Давайте переосмыслим решение правильно.

## Исправленное Решение 1: Событийные промежуточные состояния

**Основная идея:** Промежуточные состояния обрабатывают специальные "внутренние события", которые генерируются при накоплении достаточных данных.

### Как это действительно работает:

**1. События - движущая сила:**
- Автомат всегда ждет событий (внешних байтов или внутренних сигналов)
- Без события - никаких действий не происходит

**2. Промежуточные состояния ждут специальных событий:**
- Состояние "Проверка длины" не "анализирует само по себе"
- Оно ждет событие "данные_для_проверки_готовы"
- Это событие генерируется, когда получены оба байта длины

**3. Процесс для вашего протокола:**

```
Состояние: ЖДЕМ_СТАРШИЙ_БАЙТ_ДЛИНЫ
Событие: пришел байт 0x00
Действие: сохранить как старший байт длины
Переход: остаемся в ЖДЕМ_СТАРШИЙ_БАЙТ_ДЛИНЫ

Событие: пришел байт 0x18 (младший байт длины)  
Действие: сохранить как младший байт длины
         сгенерировать внутреннее событие "ДЛИНА_ПОЛНОСТЬЮ_ПОЛУЧЕНА"
Переход: перейти в состояние ПРОВЕРКА_ДЛИНЫ

---

Состояние: ПРОВЕРКА_ДЛИНЫ
(Здесь автомат НЕ активен - он ждет события)

Событие: "ДЛИНА_ПОЛНОСТЬЮ_ПОЛУЧЕНА" (внутреннее событие)
Действие: вычислить полную длину = (старший_байт << 8) | младший_байт
         проверить длину на корректность
Переход: 
  - Если длина корректна: перейти в ЖДЕМ_ПОЛЕЗНУЮ_НАГРУЗКУ
  - Если длина некорректна: перейти в ПОИСК_ПРЕАМБУЛЫ
```

**4. Для поиска преамбулы:**

```
Состояние: ПОИСК_ПРЕАМБУЛЫ
Событие: "НУЖНО_НАЙТИ_ПРЕАМБУЛУ" (внутреннее событие)
Действие: просканировать буфер на наличие последовательности 0xA0, 0xA2
Переход:
  - Если преамбула найдена: 
        сдвинуть буфер к началу преамбулы
        перейти в ЖДЕМ_СТАРШИЙ_БАЙТ_ДЛИНЫ
  - Если преамбула не найдена:
        очистить буфер
        перейти в ЖДЕМ_ПРЕАМБУЛУ
```

### Откуда берутся внутренние события?

**Вариант А: Генерация действиями**
- Когда действие завершает критическую фазу (получили оба байта длины), оно генерирует внутреннее событие
- Автомат немедленно обрабатывает это событие в текущем такте

**Вариант Б: Epsilon-переходы**
- После выполнения действия автоматически происходит "эпсилон-переход" в промежуточное состояние
- В промежуточном состоянии выполняется проверка и выбирается следующее состояние

**Вариант В: Расширенная модель ДКА**
- Разрешаем состояниям иметь "действия при входе"
- При переходе в состояние "Проверка длины" сразу выполняется проверка
- На основе результата немедленно выбирается следующее состояние

### Практическая реализация:

```python
# Псевдокод
class Parser:
    def handle_event(self, event):
        if self.state == "WAIT_LENGTH_LOW":
            if event.type == "BYTE_RECEIVED":
                self.store_length_low(event.byte)
                # Генерируем внутреннее событие
                self.internal_events.append("LENGTH_COMPLETE")
                self.state = "CHECK_LENGTH"
                
        elif self.state == "CHECK_LENGTH":
            # Обрабатываем внутренние события в приоритете
            for internal_event in self.internal_events:
                if internal_event == "LENGTH_COMPLETE":
                    if self.is_length_valid():
                        self.state = "WAIT_PAYLOAD"
                    else:
                        self.internal_events.append("SEARCH_PREAMBLE")
                        self.state = "SEARCH_PREAMBLE"
```

### Ключевое понимание:

1. **Состояния пассивны** - они только ждут события
2. **События активны** - они заставляют автомат действовать  
3. **Внутренние события** - мост между необходимостью анализа и событийной моделью
4. **Один байт может вызывать цепочку обработки** через внутренние события

Так мы сохраняем чистоту событийной модели, но получаем возможность выполнять сложные проверки и принимать решения на основе накопленных данных.


# Вопрос

> Событийные промежуточные состояния - я эти состояния и соответсвующие им события назвал эпсилон-состояниями и эпсилон событиями.
> Получается, что вариант А и Б - идентичны.
> 
> Вариант В: Расширенная модель ДКА: действия при входе в состояние у меня можно описать, у меня есть действия при входе, действия при выходе, и действия при входе и выходе из состояния в само себя.
> Не очень понятно, за счёт чего инициируется переход в следующее состояние? Возможно, состоянию стоит добавить какой-то атрибут "transition", который говорит, что состояние переходное,
> и надо сразу проверить условия выхода из него и перейти по одной из веток?


Отлично, вы правильно уловили суть! Давайте разберём ваши вопросы подробно.

## Эпсилон-состояния и события

Да, вы абсолютно правы - то, что я назвал "событийными промежуточными состояниями", по сути и есть **эпсилон-состояния** и **эпсилон-события**. Это стандартная терминология в теории автоматов для обработки внутренних переходов без внешних входных символов.

**Варианты А и Б действительно идентичны** - просто разная терминология для одного подхода.

## Расширенная модель ДКА с действиями при входе

Вы затронули очень интересный момент! Давайте разберём вариант В подробнее.

### Концепция переходных состояний

Идея с атрибутом `transition` или "переходное состояние" - это именно то, что нужно! 

**Определение переходного состояния:**
- Состояние, которое автоматически инициирует проверку условий перехода сразу после входа
- Не ждет внешних событий
- Выполняет действие при входе, затем немедленно оценивает условия перехода

### Как это работает:

```
Состояние: ПРОВЕРКА_ДЛИНЫ [transition=true]
Действие при входе:
    total_length = (length_high << 8) | length_low
    valid = (total_length <= MAX_LENGTH)

Условия перехода:
    valid == true  → ЖДЕМ_ПОЛЕЗНУЮ_НАГРУЗКУ
    valid == false → ПОИСК_ПРЕАМБУЛЫ [transition=true]
```

**Процесс обработки:**
1. Автомат входит в состояние `ПРОВЕРКА_ДЛИНЫ`
2. **Немедленно** выполняет действие при входе (вычисляет и проверяет длину)
3. **Немедленно** проверяет условия перехода
4. **Немедленно** переходит в следующее состояние
5. Если следующее состояние тоже переходное - повторяет процесс

### Реализация в коде:

```python
class TransitionalState(State):
    def __init__(self, name, on_entry_action, transition_conditions):
        self.is_transitional = True  # Ваш атрибут transition!
        self.on_entry_action = on_entry_action
        self.transition_conditions = transition_conditions  # [(condition, next_state)]
    
    def enter(self):
        # Выполняем действие при входе
        context = self.on_entry_action()
        
        # Немедленно проверяем условия перехода
        for condition, next_state in self.transition_conditions:
            if condition(context):
                return next_state  # Немедленный переход
        
        # Если ни одно условие не выполнено - ошибка
        return ERROR_STATE

# Пример использования
CHECK_LENGTH = TransitionalState(
    name="CHECK_LENGTH",
    on_entry_action=lambda: {
        'total_length': (length_high << 8) | length_low,
        'is_valid': (length_high << 8 | length_low) <= MAX_LENGTH
    },
    transition_conditions=[
        (lambda ctx: ctx['is_valid'], WAIT_PAYLOAD),
        (lambda ctx: not ctx['is_valid'], SEARCH_PREAMBLE)
    ]
)
```

### Цепочка переходных состояний

Это особенно мощно, когда у нас несколько проверок подряд:

```
# Один внешний байт может запустить цепочку:
ЖДЕМ_МЛАДШИЙ_БАЙТ → [получили байт] → 
ПРОВЕРКА_ДЛИНЫ [transition] → 
ПОДГОТОВКА_БУФЕРА [transition] → 
ПРОВЕРКА_ДОСТАТОЧНОСТИ_ПАМЯТИ [transition] → 
ЖДЕМ_ПОЛЕЗНУЮ_НАГРУЗКУ [steady]
```

### Преимущества этого подхода:

1. **Естественность** - очень интуитивно понятен
2. **Компактность** - объединяет действие и решение в одном месте
3. **Производительность** - минимум накладных расходов
4. **Читаемость** - логика "получили данные → проверили → решили" ясна

### Потенциальные проблемы:

1. **Бесконечные циклы** - если переходные состояния циклически ссылаются друг на друга
2. **Сложность отладки** - один внешний байт может вызвать длинную цепочку переходов
3. **Нарушение классической модели** - не все frameworks ДКА поддерживают это

### Защита от проблем:

```python
def process_byte(self, byte):
    current_state = self.state
    transition_depth = 0
    max_transition_depth = 10  # Защита от бесконечных циклов
    
    while current_state.is_transitional and transition_depth < max_transition_depth:
        next_state = current_state.enter()
        current_state.on_exit()  # Действие при выходе
        current_state = next_state
        transition_depth += 1
    
    if transition_depth >= max_transition_depth:
        # Защита сработала - переходим в состояние ошибки
        current_state = ERROR_STATE
    
    self.state = current_state
```

## Сравнение подходов

| Подход | События | Сложность | Производительность | Естественность |
|--------|---------|-----------|-------------------|----------------|
| Эпсилон-события | Внутренние + внешние | Средняя | Ниже | Ниже |
| Переходные состояния | Только внешние | Низкая | Выше | Выше |

## Рекомендация

Для вашего парсера SiRF я рекомендую **подход с переходными состояниями**, потому что:

1. **Протокол достаточно простой** - нет сложных вложенных автоматов
2. **Производительность важна** - обработка GPS данных часто в реальном времени
3. **Логика проверок линейна** - получили данные → проверили → пошли дальше
4. **Легко тестировать** - каждая проверка изолирована в своем состоянии

Этот подход максимально близок к тому, как мы мысленно представляем процесс разбора протокола, и при этом остается в рамках расширенной, но все же понятной модели ДКА.

