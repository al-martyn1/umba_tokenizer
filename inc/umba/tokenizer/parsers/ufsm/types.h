/*! \file
    \brief Типы для парсера ufsm
 */

#pragma once

#include "enums.h"
// 
#include "marty_expressions/simple_bool_expressions.h"
#include "marty_expressions/tokenizer_helpers.h"


//
#include <algorithm>
#include <string>
#include <vector>



//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
using PositionInfo     =  /* umba::tokenizer:: */ FullTokenPosition;

//----------------------------------------------------------------------------
using LogicExpressionInputItem = marty::expressions::ExpressionInputItem< PositionInfo
                                                                        , umba::tokenizer::payload_type // OperatorTokenType
                                                                        , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                                        , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                                        , std::string // StringType
                                                                        >;
// Use marty::expressions::tokenizer_helpers::convertTokenizerEvent to convert tokenizer events to LogicExpressionInputItem

//----------------------------------------------------------------------------
using LogicExpressionItem = marty::expressions::ExpressionItem< PositionInfo
                                                              , umba::tokenizer::payload_type // OperatorTokenType
                                                              , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                              , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                              , std::string // StringType
                                                              >;

//----------------------------------------------------------------------------
using LogicExpression  = marty::expressions::ExpressionNode< PositionInfo
                                                           , umba::tokenizer::payload_type // OperatorTokenType
                                                           , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                           , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                           , std::string // StringType
                                                           >;

//----------------------------------------------------------------------------
using LogicExpressionOperatorTraits = marty::expressions::SimpleBoolExpressionOperatorTraits<umba::tokenizer::payload_type /* OperatorTokenType*/ >;

//----------------------------------------------------------------------------
using LogicExpressionEvaluator = marty::expressions::SimpleBoolExpressionEvaluator< LogicExpressionOperatorTraits
                                                                                  , PositionInfo
                                                                                  , umba::tokenizer::payload_type // OperatorTokenType
                                                                                  , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                                                  , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                                                  , std::string // StringType
                                                                                  >;
/*
  LogicExpressionEvaluator methods
    ExpressionNodeType performConstantAbsorption(const ExpressionNodeType &node) const
    template<typename ValueGetter>
    bool evaluate(const ExpressionNodeType &node, ValueGetter valueGetter) const
    template<typename ValueMap>
    bool evaluateMap(const ExpressionNodeType &node, const ValueMap &mVals) const
    std::set<StringType> getExpressionVars(ExpressionNodeType node) const
    TruthTable<StringType> makeTruthTable(const ExpressionNodeType &node) const
    TruthTable<StringType> makeTruthTable(const ExpressionNodeType &node, typename TruthTable<StringType>::PayloadType plt) const
    StringType toString( const ExpressionNodeType &node
                       , bool useNamedOps=false // Также вместо 0/1 выводит false/true
                       ) const
*/

//----------------------------------------------------------------------------
using LogicExpressionParser = marty::expressions::SimpleBoolExpressionParser< LogicExpressionOperatorTraits
                                                                            , PositionInfo
                                                                            , umba::tokenizer::payload_type // OperatorTokenType
                                                                            , MARTY_EXPRESSIONS_DEFAULT_INTEGER_LITERAL_VALUE_TYPE // IntegerType
                                                                            , MARTY_EXPRESSIONS_DEFAULT_FLOATING_POINT_LITERAL_VALUE_TYPE // FloatingPointType
                                                                            , std::string // StringType
                                                                            >;
/*
  LogicExpressionParser methods
    SimpleBoolExpressionParser(const OperatorTraitsType &opTraits) : m_opTraits(opTraits)
    ExpressionNodeType getExpression() const
    std::string getErrorMessage(Error err, bool addErrCode=true) const
    std::string getErrorMessage(bool addErrCode=true) const
    Error initialize()
    Error finalize()
    Error parse(const ExpressionInputItemType &tk_)

*/

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*
  События могу быть внешними и составными.
  Составные события - это список возможных событий, внешних, или также составных.
  После обработки этот список раскрывается так, что содержит только внешние события.
  Везде, где используется составное событие, оно в итоге будет обработано как список 
  отдельных внешних событий.

  Примеры описаний событий, внешних и составных, а также внешних генерируемых:

    cmdStopTraffic : external - "The RED light (stop) mode is on";
    tmToggleYellow : external generated;
    evIntegralLiteral = evBoolLiteral | evIntLiteral - "Пришел интегральный литерал";

 */

 //NOTE: составное событие, хотя и записывается в виде логического выражения - конъюнкции,
 //      является не конъюнкцией, а просто списком, в котором разделителем выступает символ '|'.

struct EventDefinition
{
    PositionInfo               positionInfo;
    std::string                name        ;
    std::string                description ;

    EventFlags                 flags = EventFlags::none; // none, external, generated
    std::vector<std::string>   basicEvents ; //!< Список событий, составляющих данное (если событие не external - не установлен такой флаг)


    std::string getCanonicalName() const { return name; }

}; // struct EventDefinition

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
/*
  Действия могу быть внешними и составными.
  Составные действия - это список возможных действий, внешних, или также составных.
  После обработки этот список раскрывается так, что содержит только внешние действия.
  Везде, где используется составное действие, оно в итоге будет обработано как список 
  отдельных внешних действий.

  Примеры описаний действий, внешних и составных:

    doSomething : external - "Что-то делаем";
    startToggleGreen  : external generates tmToggleGreen; // starts periodic timer for tmToggleGreen
    startToggleAll    : external generates { tmToggleRed, tmToggleYellow, tmToggleGreen};
    pushStateDoSomething = { pushState; doSomething; } - "Засунули состояние на стек и что-то сделали";
    popStateDoOther = { popState; doOther; } - "Восстановили состояние из стека и сделали что-то другое";

 */

struct ActionDefinition
{
    PositionInfo               positionInfo;
    std::string                name        ;
    std::string                description ;

    ActionFlags                flags = ActionFlags::none; // none, external, generates
    std::vector<std::string>   basicActions; //!< Список действий, составляющих данное (если действие не external - не установлен такой флаг)
    std::vector<std::string>   generates   ; //!< Список список событий, которые может генерировать данное действие


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

    void append( const TransitionSourceState &st) { stateList.push_back(); }

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
                 = true;
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
        for(const auto s : sorted)
        {
            if (!res.empty())
                res.append(1, ',');
            res.append(s);
        }

        return res;
    }

    // int compare(const TransitionEvent &other) const
    // bool operator< (const TransitionEvent &other) const { return compare(other)<0; }
    // bool isEqual   (const TransitionEvent &other) const { return isAny()==other.isAny() && isExcluded()==other.isExcluded() && name==other.name; }
    // bool operator==(const TransitionEvent &other) const { return isEqual(other); }
    // bool operator!=(const TransitionEvent &other) const { return !isEqual(other); }


}; // struct TransitionSourceStates


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


