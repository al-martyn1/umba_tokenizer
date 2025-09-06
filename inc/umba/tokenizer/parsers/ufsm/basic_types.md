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
    - [Флаги (EventFlags flags)](#флаги-eventflags-flags-1)
    - [Методы](#методы-2)
    - [Описание](#описание-2)
  - [struct TransitionSourceState](#struct-transitionsourcestate)
    - [Поля структуры](#поля-структуры-3)
    - [Флаги (EventFlags flags)](#флаги-eventflags-flags-2)
    - [Методы](#методы-3)
    - [Описание](#описание-3)
  - [struct TransitionEvent](#struct-transitionevent)
    - [Поля структуры](#поля-структуры-4)
    - [Флаги (EventFlags flags)](#флаги-eventflags-flags-3)
    - [Методы](#методы-4)
    - [Описание](#описание-4)
  - [struct TransitionSourceStates](#struct-transitionsourcestates)
    - [Поля структуры](#поля-структуры-5)
    - [Флаги (EventFlags flags)](#флаги-eventflags-flags-4)
    - [Методы](#методы-5)
    - [Описание](#описание-5)
  - [struct TransitionSourceStates](#struct-transitionsourcestates-1)
    - [Поля структуры](#поля-структуры-6)
    - [Флаги (EventFlags flags)](#флаги-eventflags-flags-5)
    - [Методы](#методы-6)
    - [Описание](#описание-6)
  - [struct TransitionEvents](#struct-transitionevents)
    - [Поля структуры](#поля-структуры-7)
    - [Флаги (EventFlags flags)](#флаги-eventflags-flags-6)
    - [Методы](#методы-7)
    - [Описание](#описание-7)


# Базовые типы


## struct EventDefinition

Определение события, внешнего (входного) или составного (составленного и набора внешних событий).

```cpp
struct EventDefinition
{
    PositionInfo               positionInfo;
    std::string                name        ;
    std::string                description ;

    EventFlags                 flags = EventFlags::none; // none, external, generated

    //!< Список событий, составляющих данное (если событие не external - не установлен флаг EventFlags::external)
    std::vector<std::string>   basicEvents ;


public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct EventDefinition
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции выражения в исходных файлах.|
|**name**|имя (название) события.|
|**description**|описание события.|
|**flags**|флаги (опции) события (EventFlags).|
|**basicEvents**|вектор (список) событий, из которых состоит данное событие, если данное событие является составным.|


### Флаги (EventFlags flags)


|||
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Дынный флаг используется для проверок и оптимизаций.|


### Методы


|||
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
    PositionInfo               positionInfo;
    std::string                name        ;
    std::string                description ;

    ActionFlags                flags = ActionFlags::none; // none, external, generates

    //! Список действий, составляющих данное действие (если действие не external - не установлен флаг ActionFlags::external)
    std::vector<std::string>   basicActions;

    //! Список список событий, которые может генерировать данное действие (флаг ActionFlags::generates)
    std::vector<std::string>   generates   ;


public: // methods

    std::string getCanonicalName() const { return name; }

}; // struct ActionDefinition
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции выражения в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия (ActionFlags).|
|**basicActions**|список действий, если данное действие является составным (не установлен флаг ActionFlags::external).|
|**generates**|список событий, которые может генерировать данное действие (флаг ActionFlags::generates). Используется или может использоваться для оптимизаций.|


### Флаги (ActionFlags flags)


|||
|:---|:---|
|**ActionFlags::none**|флаги не заданы.|
|**ActionFlags::external**|действие - внешнее. Иначе - внутреннее, составное.|
|**ActionFlags::generates**|действие вызывает генерацию события (используется или может использоваться для оптимизаций).|


### Методы


|||
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

Краткое описание

```cpp
struct PredicateDefinition
{
    PositionInfo               positionInfo;
    std::string                name        ;
    std::string                description ;

    PredicateFlags             flags = PredicateFlags::none; // none, external, validFor
    LogicExpression            expression  ; //!< Выражение для пользовательского (не external)  предиката
    std::vector<std::string>   validForList; //!< Список событий, для которых предикат может быть использован. Актуален только если установлен флаг PredicateFlags::validFor


    std::string getCanonicalName() const { return name; }

}; // struct PredicateDefinition
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия. ActionFlags                flags = ActionFlags::none; // none, external, generates|


### Флаги (EventFlags flags)


|||
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Используется для проверок и оптимизаций.|


### Методы


|||
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


## struct TransitionSourceState

Краткое описание

```cpp
struct TransitionSourceState
{
    PositionInfo               positionInfo;
    std::string                name        ; // state name, not used if `any` flag is set

    TransitionSourceStateFlags flags = TransitionSourceStateFlags::none; // none, any, excluded


    bool isAny() const      { return (flags & TransitionSourceStateFlags::any)!=0; }
    bool isExcluded() const { return (flags & TransitionSourceStateFlags::excluded)!=0; }

    std::string getCanonicalName() const
    {
        std::string res = isAny() ? std::string("*") : name;
        return std::string(isExcluded() ? "!" : "") + res;
    }

    int compare(const TransitionSourceState&other) const
    {
        if (isAny() || other.isAny())
        {
            // Есть один any

            if (isAny() && other.isAny()) // Оба - any?
            {
                if (isExcluded() && other.isExcluded())
                    return 0; // Оба - !*

                if (!isExcluded() && !other.isExcluded())
                    return 0; // Оба - *

                return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше
            }
            else // Один - any, другой - не any. Any всегда меньше, вне зависимости от отрицания
            {
                return isAny() ? -1 : 1;
            }
        }
        else
        {
            if (isExcluded() != other.isExcluded()) // Один с отрицанием, другой - без
                return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше

            return name.compare(other.name);
        }
    }

    bool operator< (const TransitionSourceState &other) const { return compare(other)<0; }
    bool isEqual   (const TransitionSourceState &other) const { return isAny()==other.isAny() && isExcluded()==other.isExcluded() && name==other.name; }
    bool operator==(const TransitionSourceState &other) const { return isEqual(other); }
    bool operator!=(const TransitionSourceState &other) const { return !isEqual(other); }

}; // struct TransitionSourceState
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия. ActionFlags                flags = ActionFlags::none; // none, external, generates|


### Флаги (EventFlags flags)


|||
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Используется для проверок и оптимизаций.|


### Методы


|||
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


## struct TransitionEvent

Краткое описание

```cpp
struct TransitionEvent
{
    PositionInfo               positionInfo;
    std::string                name        ; // event name, not used if `any` flag is set

    TransitionEventFlags       flags = TransitionEventFlags::none;


    bool isAny() const      { return (flags & TransitionEventFlags::any)!=0; }
    bool isExcluded() const { return (flags & TransitionEventFlags::excluded)!=0; }

    std::string getCanonicalName() const
    {
        std::string res = isAny() ? std::string("*") : name;
        return std::string(isExcluded() ? "!" : "") + res;
    }

    int compare(const TransitionEvent &other) const
    {
        if (isAny() || other.isAny())
        {
            // Есть один any

            if (isAny() && other.isAny()) // Оба - any?
            {
                if (isExcluded() && other.isExcluded())
                    return 0; // Оба - !*

                if (!isExcluded() && !other.isExcluded())
                    return 0; // Оба - *

                return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше
            }
            else // Один - any, другой - не any. Any всегда меньше, вне зависимости от отрицания
            {
                return isAny() ? -1 : 1;
            }
        }
        else
        {
            if (isExcluded() != other.isExcluded()) // Один с отрицанием, другой - без
                return isExcluded() ? -1 : 1; // С отрицанием - всегда меньше

            return name.compare(other.name);
        }
    }

    bool operator< (const TransitionEvent &other) const { return compare(other)<0; }
    bool isEqual   (const TransitionEvent &other) const { return isAny()==other.isAny() && isExcluded()==other.isExcluded() && name==other.name; }
    bool operator==(const TransitionEvent &other) const { return isEqual(other); }
    bool operator!=(const TransitionEvent &other) const { return !isEqual(other); }

}; // struct TransitionEvent
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия. ActionFlags                flags = ActionFlags::none; // none, external, generates|


### Флаги (EventFlags flags)


|||
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Используется для проверок и оптимизаций.|


### Методы


|||
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


## struct TransitionSourceStates

Краткое описание

```cpp
struct TransitionSourceStates
{
    std::vector<TransitionSourceState>  stateList;

    void append   ( const TransitionSourceState &st) { stateList.push_back(st); }
    void push_back( const TransitionSourceState &st) { stateList.push_back(st); }

    // returns true if no multiple ANYs found
    bool checkForAny(bool *pHasNormalAny=0, bool *pHasExcludedAny=0) const
    {
        bool hasNormalAny   = false;
        bool hasExcludedAny = false;

        std::size_t anyCount = 0;

        for(const auto &s : stateList)
        {
            if (!s.isAny())
                continue;

            ++anyCount;

            if (!s.isExcluded())
                hasNormalAny   = true;
            else
                hasExcludedAny = true;
        }

        if (pHasNormalAny)
           *pHasNormalAny = hasNormalAny;

        if (pHasExcludedAny)
           *pHasExcludedAny = hasExcludedAny;

        return anyCount <= 1;
    }

    std::vector<TransitionSourceState> getSortedStates() const  { auto res = stateList; std::sort(res.begin(), res.end()); return res; };

    std::string getCanonicalName() const
    {
        auto sorted = getSortedStates();
        std::string res;
        for(const auto &s : sorted)
        {
            if (!res.empty())
                res.append(1, ',');
            res.append(s.getCanonicalName());
        }

        return res;
    }

    bool isEqual(const TransitionSourceStates &other) const
    {
        if (stateList.size()!=other.stateList.size())
            return false;

        auto s1 = getSortedStates();
        auto s2 = other.getSortedStates();

        for(std::size_t i=0u; i!=s1.size(); ++i)
        {
            if (!s1[i].isEqual(s2[i]))
                return false;
        }

        return true;
    }

    int compare(const TransitionSourceStates &other) const
    {
        auto s1 = getSortedStates();
        auto s2 = other.getSortedStates();

        std::size_t sz = std::min(s1.size(), s2.size());

        for(std::size_t i=0u; i!=sz; ++i)
        {
            if (s1[i].isEqual(s2[i]))
                continue;
            if (s1[i].compare(s2[i])<0)
                return -1;
            else
                return  1;
        }

        // общую часть прошли
        if (s1.size()==s2.size())
            return 0; // При равном размере вектора равны

        return s1.size()<s2.size() ? -1 : 1; // Более короткая строка меньше
    }

     bool operator< (const TransitionSourceStates &other) const { return compare(other)<0; }
     bool operator==(const TransitionSourceStates &other) const { return isEqual(other); }
     bool operator!=(const TransitionSourceStates &other) const { return !isEqual(other); }

}; // struct TransitionSourceStates
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия. ActionFlags                flags = ActionFlags::none; // none, external, generates|


### Флаги (EventFlags flags)


|||
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Используется для проверок и оптимизаций.|


### Методы


|||
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


## struct TransitionSourceStates

Краткое описание

```cpp
struct TransitionSourceStates
{
    std::vector<TransitionSourceState>  stateList;

    void append   ( const TransitionSourceState &st) { stateList.push_back(st); }
    void push_back( const TransitionSourceState &st) { stateList.push_back(st); }

    // returns true if no multiple ANYs found
    bool checkForAny(bool *pHasNormalAny=0, bool *pHasExcludedAny=0) const
    {
        bool hasNormalAny   = false;
        bool hasExcludedAny = false;

        std::size_t anyCount = 0;

        for(const auto &s : stateList)
        {
            if (!s.isAny())
                continue;

            ++anyCount;

            if (!s.isExcluded())
                hasNormalAny   = true;
            else
                hasExcludedAny = true;
        }

        if (pHasNormalAny)
           *pHasNormalAny = hasNormalAny;

        if (pHasExcludedAny)
           *pHasExcludedAny = hasExcludedAny;

        return anyCount <= 1;
    }

    std::vector<TransitionSourceState> getSortedStates() const  { auto res = stateList; std::sort(res.begin(), res.end()); return res; };

    std::string getCanonicalName() const
    {
        auto sorted = getSortedStates();
        std::string res;
        for(const auto &s : sorted)
        {
            if (!res.empty())
                res.append(1, ',');
            res.append(s.getCanonicalName());
        }

        return res;
    }

    bool isEqual(const TransitionSourceStates &other) const
    {
        if (stateList.size()!=other.stateList.size())
            return false;

        auto s1 = getSortedStates();
        auto s2 = other.getSortedStates();

        for(std::size_t i=0u; i!=s1.size(); ++i)
        {
            if (!s1[i].isEqual(s2[i]))
                return false;
        }

        return true;
    }

    int compare(const TransitionSourceStates &other) const
    {
        auto s1 = getSortedStates();
        auto s2 = other.getSortedStates();

        std::size_t sz = std::min(s1.size(), s2.size());

        for(std::size_t i=0u; i!=sz; ++i)
        {
            if (s1[i].isEqual(s2[i]))
                continue;
            if (s1[i].compare(s2[i])<0)
                return -1;
            else
                return  1;
        }

        // общую часть прошли
        if (s1.size()==s2.size())
            return 0; // При равном размере вектора равны

        return s1.size()<s2.size() ? -1 : 1; // Более короткая строка меньше
    }

     bool operator< (const TransitionSourceStates &other) const { return compare(other)<0; }
     bool operator==(const TransitionSourceStates &other) const { return isEqual(other); }
     bool operator!=(const TransitionSourceStates &other) const { return !isEqual(other); }

}; // struct TransitionSourceStates
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия. ActionFlags                flags = ActionFlags::none; // none, external, generates|


### Флаги (EventFlags flags)


|||
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Используется для проверок и оптимизаций.|


### Методы


|||
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


## struct TransitionEvents

Краткое описание

```cpp
struct TransitionEvents
{
    std::vector<TransitionEvent>  eventList;

    void append   ( const TransitionEvent &st) { eventList.push_back(st); }
    void push_back( const TransitionEvent &st) { eventList.push_back(st); }

    // returns true if no multiple ANYs found
    bool checkForAny(bool *pHasNormalAny=0, bool *pHasExcludedAny=0) const
    {
        bool hasNormalAny   = false;
        bool hasExcludedAny = false;

        std::size_t anyCount = 0;

        for(const auto &s : eventList)
        {
            if (!s.isAny())
                continue;

            ++anyCount;

            if (!s.isExcluded())
                hasNormalAny   = true;
            else
                hasExcludedAny = true;
        }

        if (pHasNormalAny)
           *pHasNormalAny = hasNormalAny;

        if (pHasExcludedAny)
           *pHasExcludedAny = hasExcludedAny;

        return anyCount <= 1;
    }

    std::vector<TransitionEvent> getSortedEvents() const  { auto res = eventList; std::sort(res.begin(), res.end()); return res; };

    std::string getCanonicalName() const
    {
        auto sorted = getSortedEvents();
        std::string res;
        for(const auto &s : sorted)
        {
            if (!res.empty())
                res.append(1, ',');
            res.append(s.getCanonicalName());
        }

        return res;
    }

    bool isEqual(const TransitionEvents &other) const
    {
        if (eventList.size()!=other.eventList.size())
            return false;

        auto s1 = getSortedEvents();
        auto s2 = other.getSortedEvents();

        for(std::size_t i=0u; i!=s1.size(); ++i)
        {
            if (!s1[i].isEqual(s2[i]))
                return false;
        }

        return true;
    }

    int compare(const TransitionEvents &other) const
    {
        auto s1 = getSortedEvents();
        auto s2 = other.getSortedEvents();

        std::size_t sz = std::min(s1.size(), s2.size());

        for(std::size_t i=0u; i!=sz; ++i)
        {
            if (s1[i].isEqual(s2[i]))
                continue;
            if (s1[i].compare(s2[i])<0)
                return -1;
            else
                return  1;
        }

        // общую часть прошли
        if (s1.size()==s2.size())
            return 0; // При равном размере вектора равны

        return s1.size()<s2.size() ? -1 : 1; // Более короткая строка меньше
    }

     bool operator< (const TransitionEvents &other) const { return compare(other)<0; }
     bool operator==(const TransitionEvents &other) const { return isEqual(other); }
     bool operator!=(const TransitionEvents &other) const { return !isEqual(other); }

}; // struct TransitionEvents
```

### Поля структуры


|||
|:---|:---|
|**positionInfo**|информация о позиции в исходных файлах.|
|**name**|имя (название) действия.|
|**description**|описание действия.|
|**flags**|флаги (опции) действия. ActionFlags                flags = ActionFlags::none; // none, external, generates|


### Флаги (EventFlags flags)


|||
|:---|:---|
|**EventFlags::none**|флаги не заданы.|
|**EventFlags::external**|Событие является внешним, если установлен данный флаг. В противном случае событие является составным, и вектор `basicEvents` содержит имена базовых событий (базовых для данного, они, в свою очередь, также могут быть составными).|
|**EventFlags::generated**|событие является генерируемым (внешними действиями/actions). Используется для проверок и оптимизаций.|


### Методы


|||
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


