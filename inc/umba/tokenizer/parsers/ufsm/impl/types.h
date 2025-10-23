/*! \file
    \brief Типы для парсера ufsm - реализация функций
 */

#pragma once


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
std::vector<TransitionDefinition> StateMachineDefinition::getPrioritySortedTransitions() const
{
    std::vector<TransitionDefinition> sorted; sorted.reserve(transitions.size());
    for(const auto &kv : transitions)
        sorted.emplace_back(kv.second);

#if defined(_DEBUG) || defined(DEBUG)
    std::size_t i = 0;
    std::size_t j = 0;
    std::size_t k = 0;
    //auto logCmp = [&](auto c, auto t1, auto t2)
    auto logCmp = [&](auto , auto , auto )
    {
        // std::cerr << "i: " << i << ", j: " << j << ", k: " << k << ", cmp: " << c << std::endl;
        // std::cerr << "t1: " << t1 << std::endl;
        // std::cerr << "t2: " << t2 << std::endl;
        // std::cerr << std::endl;
    };
    auto less = [&](const TransitionDefinition &td1, const TransitionDefinition &td2) -> bool
    {
        ++i;
#else
    auto logCmp = [&](auto c, auto t1, auto t2)
    {
    };
    auto less = [](const TransitionDefinition &td1, const TransitionDefinition &td2)
    {
#endif
        int cmp = td1.sourceStates.compareForPrioritySort(td2.sourceStates);

#if defined(_DEBUG) || defined(DEBUG)
        logCmp(cmp, td1.sourceStates, td2.sourceStates);
#endif
        
        if (cmp<0)
            return true;
        if (cmp>0)
            return false;

#if defined(_DEBUG) || defined(DEBUG)
        ++j;
#endif

        cmp = td1.events.compareForPrioritySort(td2.events);
#if defined(_DEBUG) || defined(DEBUG)
        logCmp(cmp, td1.events, td2.events);
#endif

        if (cmp<0)
            return true;
        if (cmp>0)
            return false;

#if defined(_DEBUG) || defined(DEBUG)
        ++k;
#endif

        return false;
    };

    std::stable_sort(sorted.begin(), sorted.end(), less);

    return sorted;
}

//----------------------------------------------------------------------------
// bool StateMachineDefinition::expandTransitions()
// {
//     // insertion_ordered_map<TransitionDefinition>  transitionsSorted = transitions;
//  
//     // std::string       name        ;
//     // insertion_ordered_map<TransitionDefinition>    transitions;
//  
//     return true;
// }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::

