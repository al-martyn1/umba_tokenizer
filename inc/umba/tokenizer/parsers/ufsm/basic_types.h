/*! \file
    \brief Базовые типы для парсера ufsm
 */

#pragma once

#include "basic_typedefs.h"

//
#include "../../../undef_min_max.h"


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*
  Предикаты могу быть внешними и составными.
  Составные предикаты - это логическое выражение (булева формула), состоящее из 
  предикатов, операций `&`и `|`, а также группирующих скобок.
  Предикаты, входящие в выражение, могут быть внешними и составными.
  После обработки это выражение раскрывается так, что содержит только внешние предикаты.

  Примеры описаний предикатов, внешних и составных:

    greenLightIsOn : external - "Checks the GREEN light is ON"; // valid-for {toggleGreen}
    isIndexBrace : external valid-for {evOperator} - "Оператор - индексная скобка";
    isAnyOpenBrace = isOpenBrace & pTrue; // Оставляем без описания

  После того, как в описание автомата добавляются определения событий и предикатов из используемых
  дефиниций и унаследованных автоматов, для всех пользовательских предикатов формируется список
  validForList.

 */

//NOTE: заметка по терминологии. Дефиниции (`definitions`) полностью аналогичны автоматам, 
//      но из дефиниций нельзя произвести генерацию автомата в диаграмму или код.
//      Автоматы можно переиспользовать (унаследовать) при помощи ключевого слова `inherits`,
//      дефиниции переиспользуются при помощи ключевого слова `uses`.

//NOTE: Нужно продумать, как происходит формирование списков действий, событий, предикатов и переходов
//      при наследовании `inherits` автоматов (и использовании `uses` дефиниций).
//      Пока есть идея, что непосредственно описанные в автомате сущности первичны, 
//      а то, что мы поттягиваем из ранее определенных автоматов и дефиниций добавляется (и может 
//      переопределять сущности автомата), тогда выражение `override` в `inherits`/`uses`
//      имеет смысл. 
//      Иначе же у нас сущности берутся из первой базы (но в принципе, `override`
//      имеет смысл для последущих `inherits`/`uses`), и надо как-то уметь их переопределять
//      в описании текущего автомата - для каждого раздела задавать override?
//      в принципе, тоже рабочий вариант. Как же быть?

/*
  Пример объявления автомата:

  state-machine TrafficLightPedestrian : 
  //inherits TrafficLightBase,
  uses TrafficLightCommands, 
       TrafficLightEventsActions override {actions, events, predicates, states, transitions} // возможные коллизии молча переписываются более поздними значениями
  - "Pedestrian Traffic Light (Red-Green)"
*/

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

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*
    Имени у перехода нет.
    У перехода есть список начальных состояний, из которых он может быть совершён.
    На базе этого списка и делаем имя. 

    После изначального разбора, при генерации запрошенного автомата, производится 
    дополнительная обработка переходов - разворачивание исходных состояний и разворачивание
    событий:
    1. для каждого состояния из списка исходных состояний перехода создаётся отдельный переход;
    2. для каждого события из списка событий для перехода (уже для перехода, у которого имеется 
      только одно исходное состояние) создаётся отдельный переход.

    Примеры описаний переходов:

      turnedOff : cmdStopTraffic   -> trafficStopped;
      turnedOff : * -> self; // cmdTurnOff,cmdUncontrolledMode и пр -> turnedOff
      turnedOff : *,!tmToggleGreen -> self; // cmdTurnOff,cmdUncontrolledMode и пр -> turnedOff
      stopNotice : tmToggleGreen ?  greenLightIsOn -> self : greenOff;
      stopNotice : tmToggleGreen ? !greenLightIsOn -> self : greenOn;
      *,!stopNotice : cmdTurnOff          -> turnedOff;
      * : cmdTurnOff          -> turnedOff;
      * : tmAlivePoll -> self : reportAlive;

      Общий синтаксис описания переходов таков:

      СПИСОК_СОСТОЯНИЙ : СПИСОК_СОБЫТИЙ [? УСЛОВИЕ] -> ЦЕЛЕВОЕ_СОСТОЯНИЕ [: ДЕЙСТВИЕ];

      СПИСОК_СОСТОЯНИЙ - одно или несколько состояний, разделенных запятыми. 
                         Символ `*` обозначает все состояния (кроме финальных).
                         Состояние может быть с отрицанием - это значит, что оно явно 
                         исключается из обработки, но если в списке нет состояния ANY (`*`),
                         то отрицание не имеет смысла.
                         Отрицание для состояния ANY (`*`) также не имеет смысла.

      СПИСОК_СОБЫТИЙ   - одно или несколько событий, разделенных запятыми. 
                         Символ `*` обозначает все события.
                         Событие может быть с отрицанием - это значит, что оно явно 
                         исключается из обработки, но если в списке нет события ANY (`*`),
                         то отрицание не имеет смысла.
                         Отрицание для события ANY (`*`) также не имеет смысла.

      УСЛОВИЕ          - опциональное условие перехода, логическое выражение.
                         При обработке условия производится:
                         1. подстановка всех пользовательских предикатов в выражение условия 
                         до тех пор, пока все предикаты в выражении не станут элементарными 
                         (внешними, external);
                         2. проверка всех предикатов на предмет допустимости использования
                         с обрабатываемым событием (valid-for).

 */

//------------------------------
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

//------------------------------
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

//------------------------------
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

//------------------------------
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


//----------------------------------------------------------------------------

/*
  Тут уже можно описать структуру TransitionDefinition, но у нас пока нет подходящего контейнера 

*/



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


