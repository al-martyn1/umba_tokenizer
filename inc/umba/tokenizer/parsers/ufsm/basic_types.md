- [Базовые типы](#базовые-типы)
  - [struct EventDefinition](#struct-eventdefinition)
    - [Поля структуры](#поля-структуры)
    - [Флаги (EventFlags flags)](#флаги-eventflags-flags)
    - [Методы](#методы)
    - [Описание](#описание)
  - [struct ActionDefinition](#struct-actiondefinition)
    - [Поля структуры](#поля-структуры-1)
    - [Флаги (ActionFlags flags)](#флаги-actionflags-flags)
    - [Методы](#методы-1)
    - [Описание](#описание-1)
  - [struct PredicateDefinition](#struct-predicatedefinition)
    - [Поля структуры](#поля-структуры-2)
    - [Флаги (PredicateFlags flags)](#флаги-predicateflags-flags)
    - [Методы](#методы-2)
    - [Описание](#описание-2)
  - [Переходы](#переходы)
    - [struct TransitionSourceState](#struct-transitionsourcestate)
      - [Поля структуры](#поля-структуры-3)
      - [Флаги (TransitionSourceStateFlags flags)](#флаги-transitionsourcestateflags-flags)
      - [Методы](#методы-3)
      - [Примечание](#примечание)
    - [struct TransitionSourceStates](#struct-transitionsourcestates)
      - [Поля структуры](#поля-структуры-4)
      - [Методы](#методы-4)
      - [Примечание](#примечание-1)
    - [struct TransitionEvent](#struct-transitionevent)
      - [Поля структуры](#поля-структуры-5)
      - [Флаги (TransitionEventFlags flags)](#флаги-transitioneventflags-flags)
      - [Методы](#методы-5)
      - [Примечание](#примечание-2)
    - [struct TransitionEvents](#struct-transitionevents)
      - [Поля структуры](#поля-структуры-6)
      - [Методы](#методы-6)
      - [Примечание](#примечание-3)


# Базовые типы


## struct EventDefinition

Определение события, внешнего (входного) или составного (составленного из набора внешних событий).

```cpp
struct EventDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    EventFlags      flags = EventFlags::none; // none, external, generated

    /*! Список событий, составляющих данное (если событие не external
        - не установлен флаг EventFlags::external) */
    std::vector<std::string>   basicEvents ;


public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct EventDefinition
```

### Поля структуры


|Значение|Описание|
|:---|:---|
|**positionInfo**|информация о позиции выражения в исходных файлах.|
|**name**|имя (название) события.|
|**description**|описание события.|
|**flags**|флаги (опции) события (EventFlags).|
|**basicEvents**|вектор (список) событий, из которых состоит данное событие, если данное событие является составным.|


### Флаги (EventFlags flags)


|Значение|Описание|
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Дынный флаг используется для проверок и оптимизаций.|


### Методы


|Значение|Описание|
|:---|:---|
|**getCanonicalName**|формирует "каноническое" имя объекта. В данном случае, возвращает просто имя объекта, которое является его полем/членом.|


### Описание

События могу быть внешними и составными.

Составные события - это список возможных событий, внешних, или также составных.

После обработки этот список раскрывается так, что содержит только внешние события.

Везде, где используется составное событие, оно в итоге будет обработано как список
отдельных внешних событий.


**Примеры описаний событий, внешних и составных, а также внешних генерируемых:**
```
cmdStopTraffic : external - "The RED light (stop) mode is on";
tmToggleYellow : external generated;
evIntegralLiteral = evBoolLiteral | evIntLiteral - "Пришел интегральный литерал";
```

**Примечание**
Составное событие, хотя и записывается в виде логического выражения - дизъюнкции,
является не дизъюнкцией, а просто списком, в котором разделителем выступает символ '|'.


## struct ActionDefinition

Определение действий, выполняемых при переходах между состояниями.


```cpp
struct ActionDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    ActionFlags     flags = ActionFlags::none; // none, external, generates

    /*! Список действий, составляющих данное действие (если действие не
        external - не установлен флаг ActionFlags::external) */
    std::vector<std::string>   basicActions;

    /*! Список список событий, которые может генерировать данное действие
        (флаг ActionFlags::generates) */
    std::vector<std::string>   generates   ;


public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct ActionDefinition
```

### Поля структуры


|Значение|Описание|
|:---|:---|
|**positionInfo**|информация о позиции выражения в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия (ActionFlags).|
|**basicActions**|список действий, если данное действие является составным (не установлен флаг ActionFlags::external).|
|**generates**|список событий, которые может генерировать данное действие (флаг ActionFlags::generates). Используется или может использоваться для оптимизаций.|


### Флаги (ActionFlags flags)


|Значение|Описание|
|:---|:---|
|**ActionFlags::none**|флаги не заданы.|
|**ActionFlags::external**|действие - внешнее. Иначе - внутреннее, составное.|
|**ActionFlags::generates**|действие вызывает генерацию события (используется или может использоваться для оптимизаций).|


### Методы


|Значение|Описание|
|:---|:---|
|**getCanonicalName**|формирует "каноническое" имя объекта. В данном случае, возвращает просто имя объекта, которое является его полем/членом.|


### Описание

Действия могу быть внешними и составными.

Составные действия - это список возможных действий, внешних, или также составных.

После обработки этот список раскрывается так, что содержит только внешние действия.

  Везде, где используется составное действие, оно в итоге будет обработано как список
  отдельных внешних действий.


**Примеры описаний действий, внешних и составных:**
```
doSomething : external - "Что-то делаем";
startToggleGreen  : external generates tmToggleGreen; // starts periodic timer for tmToggleGreen
startToggleAll    : external generates { tmToggleRed, tmToggleYellow, tmToggleGreen};
pushStateDoSomething = { pushState; doSomething; } - "Засунули состояние на стек и что-то сделали";
popStateDoOther = { popState; doOther; } - "Восстановили состояние из стека и сделали что-то другое";
```


## struct PredicateDefinition

Определение предикатов. Предикат - это дополнительное условие при переходе по событию.

```cpp
struct PredicateDefinition
{
    PositionInfo    positionInfo;
    std::string     name        ;
    std::string     description ;

    // none, external, validFor
    PredicateFlags  flags = PredicateFlags::none;

    /*! Выражение для пользовательского (не external)  предиката */
    LogicExpression expression  ;

    /*! Список событий, для которых предикат может быть использован.
        Актуален только если установлен флаг PredicateFlags::validFor */
    std::vector<std::string>   validForList;

    std::string getCanonicalName() const { return name; }

}; // struct PredicateDefinition
```

### Поля структуры


|Значение|Описание|
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) предиката.|
|**description**|описание предиката.|
|**flags**|флаги (опции) предиката (PredicateFlags).|
|**expression**|логическое выражение для пользовательского (не внешнего) предиката.|
|**validForList**|список событий, для которых предикат может использоваться.|


### Флаги (PredicateFlags flags)


|Значение|Описание|
|:---|:---|
|**PredicateFlags::none**|флаги не заданы.|
|**PredicateFlags::external**|Преликат является внешним, если установлен данный флаг. В противном случае предикат является составным, и `expression` содержит валидное логическое выражение.|
|**PredicateFlags::validFor**|предикат содержит список событий в векторе `validForList`, для которых данный предикат может исползоваться. Используется для проверок и оптимизаций.|


### Методы


|Значение|Описание|
|:---|:---|
|**getCanonicalName**|формирует "каноническое" имя объекта. В данном случае, возвращает просто имя объекта, которое является его полем/членом.|


### Описание

Предикаты могу быть внешними и составными.

Составные предикаты - это логическое выражение (булева формула), состоящее из
предикатов, операций `&`и `|`, а также группирующих скобок.

Предикаты, входящие в выражение, могут быть внешними и составными.
После обработки это выражение раскрывается так, что содержит только внешние предикаты.

**Примеры описаний предикатов, внешних и составных:**
```
greenLightIsOn : external - "Checks the GREEN light is ON"; // valid-for {toggleGreen}
isIndexBrace : external valid-for {evOperator} - "Оператор - индексная скобка";
isAnyOpenBrace = isOpenBrace & pTrue; // Оставляем без описания
```

**Примечание**
После того, как в описание автомата добавляются определения событий и предикатов из используемых
дефиниций и унаследованных автоматов, для всех пользовательских предикатов формируется список
validForList.

**Примечание по терминологии**
Дефиниции (`definitions`) полностью аналогичны автоматам,
но из дефиниций нельзя произвести генерацию автомата в диаграмму или код.
Автоматы можно переиспользовать (унаследовать) при помощи ключевого слова `inherits`,
дефиниции переиспользуются при помощи ключевого слова `uses`.

**Примечание по реализации**
Нужно продумать, как происходит формирование списков действий, событий, предикатов и переходов
при наследовании `inherits` автоматов (и использовании `uses` дефиниций).
Пока есть идея, что непосредственно описанные в автомате сущности первичны,
а то, что мы поттягиваем из ранее определенных автоматов и дефиниций добавляется (и может
переопределять сущности автомата), тогда выражение `override` в `inherits`/`uses`
имеет смысл.
Иначе же у нас сущности берутся из первой базы (но в принципе, `override`
имеет смысл для последущих `inherits`/`uses`), и надо как-то уметь их переопределять
в описании текущего автомата - для каждого раздела задавать override?
в принципе, тоже рабочий вариант. Как же быть?

Пример объявления автомата:
```
state-machine TrafficLightPedestrian :
//inherits TrafficLightBase,
uses TrafficLightCommands,
   TrafficLightEventsActions
     // возможные коллизии молча переписываются более поздними значениями
     override {actions, events, predicates, states, transitions}
- "Pedestrian Traffic Light (Red-Green)"
```


## Переходы

Явно задаваемого имени у перехода нет.

У перехода есть список начальных состояний, из которых он может быть совершён.

Также, у перехода есть список событий, по которым производится переход, и, опционально,
список дополнительных условий (предикатов) перехода.

На базе этих списков для перехода формируется имя, по которому производится поиск.

После изначального разбора, при генерации запрошенного автомата, производится
дополнительная обработка переходов - разворачивание исходных состояний и разворачивание
событий:
1. для каждого состояния из списка исходных состояний перехода создаётся отдельный переход;
2. для каждого события из списка событий для перехода (уже для перехода, у которого имеется
  только одно исходное состояние) создаётся отдельный переход.

Примеры описаний переходов:
```
turnedOff : cmdStopTraffic   -> trafficStopped;
turnedOff : * -> self; // cmdTurnOff,cmdUncontrolledMode и пр -> turnedOff
turnedOff : *,!tmToggleGreen -> self; // cmdTurnOff,cmdUncontrolledMode и пр -> turnedOff
stopNotice : tmToggleGreen ?  greenLightIsOn -> self : greenOff;
stopNotice : tmToggleGreen ? !greenLightIsOn -> self : greenOn;
*,!stopNotice : cmdTurnOff          -> turnedOff;
* : cmdTurnOff          -> turnedOff;
* : tmAlivePoll -> self : reportAlive;
```

Общий синтаксис описания переходов таков:
```
СПИСОК_СОСТОЯНИЙ : СПИСОК_СОБЫТИЙ [? УСЛОВИЕ] -> ЦЕЛЕВОЕ_СОСТОЯНИЕ [: ДЕЙСТВИЕ];
```

- `СПИСОК_СОСТОЯНИЙ` - одно или несколько состояний, разделенных запятыми.
Символ `*` обозначает все состояния (кроме финальных).
Состояние может быть с отрицанием - это значит, что оно явно
исключается из обработки, но если в списке нет состояния ANY (`*`),
то отрицание не имеет смысла.
Отрицание для состояния `ANY` (`*`) также не имеет смысла.

- `СПИСОК_СОБЫТИЙ` - одно или несколько событий, разделенных запятыми.
Символ `*` обозначает все события.
Событие может быть с отрицанием - это значит, что оно явно
исключается из обработки, но если в списке нет события ANY (`*`),
то отрицание не имеет смысла.
Отрицание для события `ANY` (`*`) также не имеет смысла.

- `УСЛОВИЕ` - опциональное условие перехода, логическое выражение.
При обработке условия производится:
 1. подстановка всех пользовательских предикатов в выражение условия
   до тех пор, пока все предикаты в выражении не станут элементарными
   (внешними, `external`);
 2. проверка всех предикатов на предмет допустимости использования
 с обрабатываемым событием (`valid-for`).


### struct TransitionSourceState

Определение исходного состояния для перехода.

```cpp
struct TransitionSourceState
{
    PositionInfo    positionInfo;
    std::string     name; // state name, not used if `any` flag is set

    // none, any, excluded
    TransitionSourceStateFlags flags = TransitionSourceStateFlags::none;


    std::string getCanonicalName() const;

    int compare(const TransitionSourceState&other) const;
    bool isEqual   (const TransitionSourceState &other) const;

    bool operator< (const TransitionSourceState &other) const;
    bool operator==(const TransitionSourceState &other) const;
    bool operator!=(const TransitionSourceState &other) const;

    bool isAny() const;
    bool isExcluded() const;

}; // struct TransitionSourceState
```

#### Поля структуры


|Значение|Описание|
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) состояния.|
|**flags**|флаги (опции) состояния (TransitionSourceStateFlags).|


#### Флаги (TransitionSourceStateFlags flags)


|Значение|Описание|
|:---|:---|
|**TransitionSourceStateFlags::none**|флаги не заданы.|
|**TransitionSourceStateFlags::any**|признак, что исходное состояние имеет тип `ANY` - переход относится ко всем состояниям автомата.|
|**TransitionSourceStateFlags::excluded**|указывает, что состояние нужно исключить из списка переходов. Используется в списке исходных состояний совместно с `ANY` исходными состояниями.|


#### Методы


|Значение|Описание|
|:---|:---|
|**getCanonicalName**|формирует "каноническое" имя объекта, см "Примечание".|
|**compare**|производит сравнение, возвращает `-1`, `0` или `1`.|
|**isEqual**|производит проверку на равеноство, и работает эффективнее, чем `compare`.|
|**operator<**, **operator==**, **operator!=**|опраторы отношений, реализованы на базе `compare` и `isEqual`.|
|**isAny**|возвращает true, если состояние является `ANY`-состоянием.|
|**isExcluded**|возвращает true, если состояние исключается из списка состояний.|


#### Примечание

Имя состояния формируется следующим образом:
- если состояние является `ANY`, то имя равно `*`, иначе - используется заданное имя состояния.
- если состояние `excluded`, то перед именем добавляется признак исключения `!`.


### struct TransitionSourceStates

Определение списка исходных состояний для перехода.

```cpp
struct TransitionSourceStates
{
    std::vector<TransitionSourceState>  stateList;

    std::string getCanonicalName() const;

    int compare(const TransitionSourceStates &other) const;
    bool isEqual(const TransitionSourceStates &other) const;

    bool operator< (const TransitionSourceStates &other) const;
    bool operator==(const TransitionSourceStates &other) const;
    bool operator!=(const TransitionSourceStates &other) const;

    void append   ( const TransitionSourceState &st);
    void push_back( const TransitionSourceState &st);

    std::vector<TransitionSourceState> getSortedStates() const;

    // returns true if no multiple ANYs found
    bool checkForAny( bool *pHasNormalAny=0
                    , bool *pHasExcludedAny=0) const;

}; // struct TransitionSourceStates
```

#### Поля структуры


|Значение|Описание|
|:---|:---|
|**stateList**|список элементарных исходных состояний.|


#### Методы


|Значение|Описание|
|:---|:---|
|**getCanonicalName**|формирует "каноническое" имя объекта, см "Примечание".|
|**compare**|производит сравнение, возвращает `-1`, `0` или `1`.|
|**isEqual**|производит проверку на равеноство, и работает эффективнее, чем `compare`.|
|**operator<**, **operator==**, **operator!=**|опраторы отношений, реализованы на базе `compare` и `isEqual`.|
|**append**|добавляет элементарное состояние в список.|
|**push_back**|добавляет элементарное состояние в список.|
|**getSortedStates**|возвращает отсортированный список состояний для сравнения и генерации имени.|
|**checkForAny**|произодит проверку, есть ли в списке состояние `ANY`.|


#### Примечание

Имя состояния формируется из предварительно отсортированного списка элементарных событий, которые разделяются запятыми.


### struct TransitionEvent

Определение события для перехода.

```cpp
struct TransitionEvent
{
    PositionInfo    positionInfo;
    std::string     name        ; // event name, not used if `any`

    // none, any, excluded
    TransitionEventFlags  flags = TransitionEventFlags::none;


    std::string getCanonicalName() const;

    int compare(const TransitionEvent &other) const;
    bool isEqual   (const TransitionEvent &other) const;

    bool operator< (const TransitionEvent &other) const;
    bool operator==(const TransitionEvent &other) const;
    bool operator!=(const TransitionEvent &other) const;

    bool isAny() const;
    bool isExcluded() const;

}; // struct TransitionEvent
```

#### Поля структуры


|Значение|Описание|
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) действия.|
|**flags**|флаги (опции) события перехода (TransitionEventFlags).|


#### Флаги (TransitionEventFlags flags)


|Значение|Описание|
|:---|:---|
|**TransitionEventFlags::none**|флаги не заданы.|
|**TransitionEventFlags::any**|признак, что событие имеет тип `ANY` - переход относится ко всем состояниям.|
|**TransitionEventFlags::excluded**|указывает, что событие нужно исключить из списка событий. Используется в списке событий перехода совместно с `ANY` событиями.|


#### Методы


|Значение|Описание|
|:---|:---|
|**getCanonicalName**|формирует "каноническое" имя объекта, см "Примечание".|
|**compare**|производит сравнение, возвращает `-1`, `0` или `1`.|
|**isEqual**|производит проверку на равеноство, и работает эффективнее, чем `compare`.|
|**operator<**, **operator==**, **operator!=**|опраторы отношений, реализованы на базе `compare` и `isEqual`.|
|**isAny**|возвращает true, если состояние является `ANY`-состоянием.|
|**isExcluded**|возвращает true, если состояние исключается из списка состояний.|


#### Примечание

Имя события формируется следующим образом:
- если событие является `ANY`, то имя равно `*`, иначе - используется заданное имя событие.
- если событие `excluded`, то перед именем добавляется признак исключения `!`.


### struct TransitionEvents

Определение списка событий для перехода.

```cpp
struct TransitionEvents
{
    std::vector<TransitionEvent>  eventList;

    std::string getCanonicalName() const;

    int compare(const TransitionEvents &other) const;
    bool isEqual(const TransitionEvents &other) const;

    bool operator< (const TransitionEvents &other) const;
    bool operator==(const TransitionEvents &other) const;
    bool operator!=(const TransitionEvents &other) const;

    void append   ( const TransitionEvent &st);
    void push_back( const TransitionEvent &st);

    std::vector<TransitionEvent> getSortedEvents() const;

    // returns true if no multiple ANYs found
    bool checkForAny( bool *pHasNormalAny=0
                    , bool *pHasExcludedAny=0) const;

}; // struct TransitionEvents
```

#### Поля структуры


|Значение|Описание|
|:---|:---|
|**eventList**|список элементарных событий.|


#### Методы


|Значение|Описание|
|:---|:---|
|**getCanonicalName**|формирует "каноническое" имя объекта, см "Примечание".|
|**compare**|производит сравнение, возвращает `-1`, `0` или `1`.|
|**isEqual**|производит проверку на равеноство, и работает эффективнее, чем `compare`.|
|**operator<**, **operator==**, **operator!=**|опраторы отношений, реализованы на базе `compare` и `isEqual`.|
|**append**|добавляет элементарное состояние в список.|
|**push_back**|добавляет элементарное состояние в список.|
|**getSortedEvents**|возвращает отсортированный список событий для сравнения и генерации имени.|
|**checkForAny**|произодит проверку, есть ли в списке событие `ANY`.|


#### Примечание

Имя списка событий формируется из предварительно отсортированного списка элементарных событий, которые разделяются запятыми.


