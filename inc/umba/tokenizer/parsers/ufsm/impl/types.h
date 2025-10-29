/*! \file
    \brief Типы для парсера ufsm - реализация функций
 */

#pragma once

//
#include <string>
//
#include <sstream>
//
#include <iostream>

#include <unordered_set>


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
TypeValueInfo makeTypeValueInfo(const StateMachineDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("state machine") };
}

inline
TypeValueInfo makeTypeValueInfo(const NamespaceDefinition &d)
{
    return TypeValueInfo{ d.positionInfo, d.getCanonicalName(), std::string("namespace") };
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
std::string getNamespaceEntryName(const NamespaceEntry &e)
{
    return std::visit( [](const auto &a)
                       {
                           return a.getCanonicalName();
                       }
                     , e
                     );
}

//------------------------------
inline
NamespaceEntryKind getNamespaceEntryKind(const NamespaceEntry &e)
{
    return std::visit( [](const auto &a) -> NamespaceEntryKind
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, NamespaceDefinition    >) return NamespaceEntryKind::nsDefinition ;
                           if constexpr (std::is_same_v <ArgType, StateMachineDefinition >) return NamespaceEntryKind::fsmDefinition;
                       }
                     , e
                     );
}

//------------------------------
inline
PositionInfo getNamespaceEntryPositionInfo(const NamespaceEntry &e)
{
    return std::visit( [](const auto &a) -> PositionInfo
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, NamespaceDefinition    >) return a.positionInfo;
                           if constexpr (std::is_same_v <ArgType, StateMachineDefinition >) return a.positionInfo;
                       }
                     , e
                     );
}

//    StateMachineFlags flags = StateMachineFlags::none; // none, stateMachine


//----------------------------------------------------------------------------
inline
StateMachineFlags getNamespaceEntryStateMachineFlags(const NamespaceEntry &e)
{
    return std::visit( [](const auto &a) -> StateMachineFlags
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, NamespaceDefinition    >) return StateMachineFlags::invalid;
                           if constexpr (std::is_same_v <ArgType, StateMachineDefinition >)
                           {
                               return a.flags;
                           }
                       }
                     , e
                     );
}

inline
std::string getNamespaceEntryKindString(NamespaceEntryKind kind, StateMachineFlags flags)
{
    std::string kindStr = "unknown/invalid";
    switch(kind)
    {
        case NamespaceEntryKind::invalid      : break;
        case NamespaceEntryKind::none         : kindStr = "none"; break;
        case NamespaceEntryKind::nsDefinition : kindStr = "namespace"; break;
        case NamespaceEntryKind::fsmDefinition:
             if ((flags&StateMachineFlags::stateMachine)==0)
                 kindStr = "definitions";
             else
                 kindStr = "state machine";
             break;
        default: {}
    }

    return kindStr;
}

inline
std::string getNamespaceEntryKindString(NamespaceEntryKind kind)
{
    return getNamespaceEntryKindString(kind, StateMachineFlags::stateMachine);
}

inline
std::string getNamespaceEntryKindString(const NamespaceEntry &e)
{
    return getNamespaceEntryKindString(getNamespaceEntryKind(e), getNamespaceEntryStateMachineFlags(e));
}

inline
TypeValueInfo makeTypeValueInfo(const NamespaceEntry &d)
{
    return TypeValueInfo{ getNamespaceEntryPositionInfo(d), getNamespaceEntryName(d), getNamespaceEntryKindString(d) };
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
void StateMachineDefinition::prioritySortTransitions()
{
    std::vector<TransitionDefinition> ptr = getPrioritySortedTransitions();
    transitions.clear();

    for( auto &&t: ptr)
       addDefinition(t);
}

//----------------------------------------------------------------------------
inline
std::vector<TransitionDefinition> StateMachineDefinition::getTransitionsVector() const
{
    std::vector<TransitionDefinition> res; res.reserve(transitions.size());
    for(const auto &kv : transitions)
        res.emplace_back(kv.second);

    return res;
}

//----------------------------------------------------------------------------
inline
std::vector<TransitionDefinition> StateMachineDefinition::getPrioritySortedTransitions() const
{
    std::vector<TransitionDefinition> sorted = getTransitionsVector();

    auto less = [](const TransitionDefinition &td1, const TransitionDefinition &td2)
    {
        return td1.getWeightForCompareForPrioritySort() < td2.getWeightForCompareForPrioritySort();
    };

    std::stable_sort(sorted.begin(), sorted.end(), less);

    return sorted;
}

//----------------------------------------------------------------------------
inline
std::set<std::string> StateMachineDefinition::getSourceStateNamesSet(bool skipFinal) const
{
    std::set<std::string> s;
    for(const auto &kv : states)
    {
        if (((kv.second.flags&(StateFlags::final|StateFlags::error))!=0) && skipFinal)
            continue; // пропускаем финальные и ошибочные состояния, но только если задано опцией

        s.insert(kv.first);
    }

    return s;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> StateMachineDefinition::getSourceStateNamesList(bool skipFinal) const
{
    std::vector<std::string> v; v.reserve(states.size());
    for(const auto &kv : states)
    {
        if (((kv.second.flags&(StateFlags::final|StateFlags::error))!=0) && skipFinal)
            continue; // пропускаем финальные и ошибочные состояния, но только если задано опцией

        v.push_back(kv.first);
    }

    return v;
}

//----------------------------------------------------------------------------
inline
std::set<std::string> StateMachineDefinition::getEventNamesSet() const
{
    std::set<std::string> s;
    for(const auto &kv : events)
        s.insert(kv.first);

    return s;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> StateMachineDefinition::getEventNamesList() const
{
    std::vector<std::string> v; v.reserve(events.size());
    for(const auto &kv : events)
        v.push_back(kv.first);

    return v;
}

//----------------------------------------------------------------------------
inline
bool StateMachineDefinition::hasSamePrerequisitesTransition(const TransitionDefinition &tOther) const
{
    for(const auto &kvt : transitions)
    {
        if (kvt.second.isEqualPrerequisites(tOther))
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
inline
std::size_t StateMachineDefinition::assignStateIds(std::size_t startId)
{
    for(auto &kv : states)
    {
        kv.second.id = ++startId;
    }

    return startId;
}

//----------------------------------------------------------------------------
inline
std::size_t StateMachineDefinition::assignTransitionIds(std::size_t startId)
{
    for(auto &kv : transitions)
    {
        kv.second.id = ++startId;
    }

    return startId;
}

//----------------------------------------------------------------------------
inline
std::size_t StateMachineDefinition::assignIds(std::size_t startId)
{
    return assignTransitionIds(assignStateIds(startId));
}

//----------------------------------------------------------------------------
inline
bool StateMachineDefinition::expandTransitions()
{
    std::vector<std::string> eventNames       = getEventNamesList();
    std::vector<std::string> sourceStateNames = getSourceStateNamesList( true /* skipFinal */ );

    std::vector<TransitionDefinition> sortedTransitions = getPrioritySortedTransitions();

    transitions.clear();

    // !!! Данная версия expandTransitions нужна для реализации автомата
    // Для рисования графа нам не нужно раскрывать events - граф будет сильно замусорен,
    // но нам нужно раскрывать sourceStates - так как рисовать события нужно для каждой вершины.
    // При этом нам нужно как-то помечать раскрытые по sourceStates переходы каким-то ID для того,
    // чтобы проверить, присутствует ли что-либо с таким ID в полностью раскрытом графе.
    // Если не присутствует, то такой нераскрытый по events переход не отображается.

    for(const auto &srtd : sortedTransitions)
    {
        auto expanded = srtd.expandEventsAndSourceStates(eventNames, sourceStateNames);

        for(const auto &expd : expanded)
        {
            if (hasSamePrerequisitesTransition(expd))
            {
                continue;
            }
            else
            {
                addDefinition(expd);
            }
        }
    }

    return true;
}

//----------------------------------------------------------------------------
std::vector<TransitionDefinition> StateMachineDefinition::makeExpandedTransitionsForGraph() const
{
    std::vector<std::string> eventNames       = getEventNamesList();
    std::vector<std::string> sourceStateNames = getSourceStateNamesList( true /* skipFinal */ );

    std::vector<TransitionDefinition> sortedTransitions = getPrioritySortedTransitions();

    // Задаем ID для существующих переходов
    std::size_t uid = 0;
    for(auto &tmpTr : sortedTransitions)
    {
        tmpTr.id = ++uid;
    }
    // assignTransitionIds();

    StateMachineDefinition tmpStateMachineDefinition;

    for(const auto &srtd : sortedTransitions)
    {
        auto expanded = srtd.expandEventsAndSourceStates(eventNames, sourceStateNames);

        for(const auto &expd : expanded)
        {
            if (tmpStateMachineDefinition.hasSamePrerequisitesTransition(expd))
            {
                continue;
            }
            else
            {
                tmpStateMachineDefinition.addDefinition(expd);
            }
        }
    }

    std::unordered_set<std::size_t> idSet;

    // Неоторые ID могли не выжить после раскрытия - например, 
    // там была звездочка, и после раскрытия все элементарные переходы
    // уже были явно заданы
    for(const auto &kv : tmpStateMachineDefinition.transitions)
    {
        // std::cerr << "Add ID: " << kv.second.id << "\n";
        idSet.insert(kv.second.id);
    }

    tmpStateMachineDefinition.transitions.clear();

    for(const auto &srtd : sortedTransitions)
    {
        auto expanded = srtd.expandSourceStates(sourceStateNames);

        for(const auto &expd : expanded)
        {
            if (idSet.find(expd.id)==idSet.end()) // Такой id пропал в результате раскрытий
                continue;

            if (tmpStateMachineDefinition.hasSamePrerequisitesTransition(expd))
            {
                continue;
            }
            else
            {
                tmpStateMachineDefinition.addDefinition(expd);
            }
        }
    }


    return tmpStateMachineDefinition.getTransitionsVector();
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::

