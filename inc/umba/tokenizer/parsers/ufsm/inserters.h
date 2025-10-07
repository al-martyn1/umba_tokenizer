/*! \file
    \brief Инсертеры - оператор вывода в поток
 */

#pragma once


#include "basic_types.h"

//
#include "marty_cpp/c_escape.h"


//
#include <array>
#include <type_traits>
#include <utility>
#include <cstddef>

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
namespace utils {

template<typename StringType, typename IteratorType>
StringType mergeString(IteratorType b, IteratorType e, const StringType &sep)
{
    StringType res;

    if (b==e)
        return res;

    res.append(*b);

    for(++b; b!=e; ++b)
    {
        res.append(sep);
        res.append(*b);
    }

    return res;
}

template<typename StringType, typename ContainerType>
StringType mergeString(const ContainerType &c, const StringType &sep)
{
    return mergeString(c.begin(), c.end(), sep);
}

} // namespace utils
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const FullQualifiedName &fqn)
{
    if (fqn.name.empty())
        return oss;

    auto it = fqn.name.begin();

    if (fqn.isAbsolute())
        oss << fqn.namespaceSeparator;

    oss << utils::mergeString(fqn.name, fqn.namespaceSeparator);

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const ParentListEntry &v)
{
    if ((v.flags & ParentListEntryFlags::uses) != 0)
        oss << "uses ";
    else if ((v.flags & ParentListEntryFlags::inherits) != 0)
        oss << "inherits ";

    oss << v.name;

    // using ufsm::enum_serialize_flags;

    // if (v.overrideFlags != 0)
    //     oss << " override { " <<  /* ufsm:: */ enum_serialize_flags(v.overrideFlags, ",") << " }";
    // if (v.overrideFlags != 0)
    //     oss << " override { " <<  /* ufsm:: */ enum_serialize_flags_InheritanceOverrideFlags(v.overrideFlags, ",") << " }";

    // InheritanceOverrideFlags   overrideFlags = InheritanceOverrideFlags::none;

    if (v.overrideFlags==InheritanceOverrideFlags::none)
    {
    }
    else if (v.overrideFlags==InheritanceOverrideFlags::all)
    {
        oss << " override";
    }
    else
    {
        std::vector<std::string> flagStrs;
        if ((v.overrideFlags&InheritanceOverrideFlags::actions)!=0)
            flagStrs.push_back("actions");
        if ((v.overrideFlags&InheritanceOverrideFlags::events)!=0)
            flagStrs.push_back("events");
        if ((v.overrideFlags&InheritanceOverrideFlags::predicates)!=0)
            flagStrs.push_back("predicates");
        if ((v.overrideFlags&InheritanceOverrideFlags::states)!=0)
            flagStrs.push_back("states");
        if ((v.overrideFlags&InheritanceOverrideFlags::transitions)!=0)
            flagStrs.push_back("transitions");

        oss << "{" << utils::mergeString(flagStrs, std::string(",")) << "}";
    }

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const EventDefinition &v)
{
    oss << v.name;

    if ((v.flags&EventFlags::external)!=0)
    {
        oss << " : external";

        if ((v.flags&EventFlags::generated)!=0)
            oss << " generated";
    }
    else
    {
        oss << " = " << utils::mergeString(v.basicEvents, std::string(" | "));
    }

    if (!v.description.empty())
        oss << " - " << "\"" << marty_cpp::cEscapeString(v.description) << "\"";

    oss << ";";

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const ActionDefinition &v)
{
    oss << v.name;

    if ((v.flags&ActionFlags::external)!=0)
    {
        oss << " : external";
        if ((v.flags&ActionFlags::generates)!=0)
        {
            oss << " generates " << utils::mergeString(v.generates, std::string(", "));
        }
    }
    else
    {
        oss << " = " << utils::mergeString(v.basicActions, std::string(", "));
    }

    if (!v.description.empty())
        oss << " - " << "\"" << marty_cpp::cEscapeString(v.description) << "\"";

    oss << ";";

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const PredicateDefinition &v)
{
    oss << v.name;

    if ((v.flags&PredicateFlags::external)!=0)
    {
        oss << " : " << "external";
        if ((v.flags&PredicateFlags::validFor)!=0)
        {
            oss << " valid-for ";
            utils::mergeString(v.validForList, std::string(", "));
        }
    }
    else
    {
        oss << " = " 
            << LogicExpressionEvaluator(makeLogicExpressionOperatorTraits()).toString(v.expression);
    }

    if (!v.description.empty())
        oss << " - " << "\"" << marty_cpp::cEscapeString(v.description) << "\"";

    oss << ";";

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const StateActionRefs &v)
{
    oss << utils::mergeString(v.list, std::string(", "));
    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& StateDefinition::print(StreamType& oss, std::size_t indendSize) const
{
    auto indend = std::string(indendSize, ' ');

    oss << indend << name;

    if ((flags&(StateFlags::initial| StateFlags::final))!=0)
    {
        if ((flags&StateFlags::initial)!=0)
            oss << ": " << "initial";
        else
            oss << ": " << "final";
    }

    if (!description.empty())
        oss << " - " << "\"" << marty_cpp::cEscapeString(description) << "\"";

    oss << "\n";
    oss << indend << "{";

    auto kinds = std::array<StateActionKind, 4>{ StateActionKind::stateEnter
                                               , StateActionKind::stateLeave
                                               , StateActionKind::selfEnter
                                               , StateActionKind::selfLeave
                                               };

    for(auto k: kinds)
    {
        auto it = actionRefs.find(k);
        if (it==actionRefs.end())
            continue;

        switch(k)
        {
            case StateActionKind::stateEnter: oss << indend << "    " << "enter"     ; break;
            case StateActionKind::stateLeave: oss << indend << "    " << "leave"     ; break;
            case StateActionKind::selfEnter : oss << indend << "    " << "self-enter"; break;
            case StateActionKind::selfLeave : oss << indend << "    " << "self-leave"; break;
            case StateActionKind::none      : continue;
            case StateActionKind::invalid   : continue;
            default: continue;
        }

        oss << ": " << it->second << ";\n";
    }

    oss << indend << "}\n\n";

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const TransitionSourceState &v)
{
    if ((v.flags&TransitionSourceStateFlags::any)!=0)
    {
        if ((v.flags&TransitionSourceStateFlags::excluded)!=0)
            throw std::runtime_error("StreamType& operator<<(StreamType &oss, const TransitionSourceState &v): excluded flags can't be combined with ANY");

        oss << "*";
    }
    else
    {
        if ((v.flags&TransitionSourceStateFlags::excluded)!=0)
            oss << "!";

        oss << v.name;
    }

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const TransitionSourceStates &v)
{
    if (v.list.empty())
        return oss;

    auto it = v.list.begin();
    oss << *it;

    for(++it; it!=v.list.end(); ++it)
        oss << ", " << *it;

    return oss;
}
 
//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const TransitionEvent &v)
{
    if ((v.flags&TransitionEventFlags::any)!=0)
    {
        if ((v.flags&TransitionEventFlags::excluded)!=0)
            throw std::runtime_error("StreamType& operator<<(StreamType &oss, const TransitionSourceState &v): excluded flags can't be combined with ANY");

        oss << "*";
    }
    else
    {
        if ((v.flags&TransitionEventFlags::excluded)!=0)
            oss << "!";

        oss << v.name;
    }

    return oss;
}
 
//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const TransitionEvents &v)
{
    if (v.list.empty())
        return oss;

    auto it = v.list.begin();
    oss << *it;

    for(++it; it!=v.list.end(); ++it)
        oss << ", " << *it;

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& operator<<(StreamType &oss, const TransitionDefinition &v)
{
    oss << v.sourceStates << ": " << v.events;

    if ((v.flags&TransitionFlags::conditional)!=0)
        oss << " ? " << LogicExpressionEvaluator(makeLogicExpressionOperatorTraits()).toString(v.additionalCondition);

    oss << " -> ";

    if ((v.flags&TransitionFlags::selfTarget)!=0)
        oss << "self";
    else
        oss << v.targetState;

    if (!v.actionRefs.empty())
        oss << " : " << v.actionRefs;
    
    if (!v.description.empty())
        oss << " - " << "\"" << marty_cpp::cEscapeString(v.description) << "\"";

    oss << ";";

    return oss;
}

//----------------------------------------------------------------------------
namespace utils {

//----------------------------------------------------------------------------
template< typename FlagType, typename ItemType, typename ContainerType=insertion_ordered_map<ItemType> >
void splitByOverrideFlag(const ContainerType &vals, std::vector<ContainerType> &splitVec)
{
    if (!splitVec.empty())
    {
        splitVec.clear();
        return;
    }

    if (vals.empty())
        return;

    ContainerType collector;
    bool bOverride = false;

    auto kvIt = vals.begin();

    if ((kvIt->second.flags & FlagType::override )!=0)
        bOverride = true;

    for(; kvIt!=vals.end(); ++kvIt)
    {
        bool newOverride = false;

        if ((kvIt->second.flags & FlagType::override )!=0)
            newOverride = true;

        if (bOverride!=newOverride)
        {
            if (!collector.empty())
                splitVec.emplace_back(collector);
            collector.clear();
            bOverride = newOverride;
        }
    }

    if (!collector.empty())
        splitVec.emplace_back(collector);
}

//----------------------------------------------------------------------------
template <typename T, typename StreamType, typename = void>
struct has_print_method : std::false_type {};

template <typename T, typename StreamType>
struct has_print_method<T, StreamType, 
    std::void_t<
        decltype(std::declval<const T&>().template print<StreamType>(
            std::declval<StreamType&>(), 
            std::declval<std::size_t>()
        ))
    >
> : std::true_type {};

// Переменная шаблона для удобства
template <typename T, typename StreamType>
constexpr bool has_print_method_v = has_print_method<T, StreamType>::value;

//----------------------------------------------------------------------------
template< typename FlagType, typename ItemType, typename StreamType, typename ContainerType=insertion_ordered_map<ItemType> >
void printDefinitionsSection(StreamType &oss, const ContainerType &sec, const std::string &secName, std::size_t indendSize)
{
    if (sec.empty())
        return;

    oss << std::string(indendSize, ' ') << secName;

    const auto &secBeginSecond = sec.begin()->second;

    if ((secBeginSecond.flags&FlagType::override)!=0)
        oss << " : " << "override";

    oss << "\n";
    oss << std::string(indendSize, ' ') << "{\n";

    for(const auto &kv : sec)
    {
        const auto &val = kv.second;
        if constexpr (has_print_method_v<ItemType, StreamType>)
        {
            val.print(oss, indendSize+4);
            oss << "\n";
        }
        else
        {
            oss << std::string(indendSize+4, ' ') << val << "\n";
        }
    }

    oss << std::string(indendSize, ' ') << "}\n";

}

//----------------------------------------------------------------------------
template< typename FlagType, typename ItemType, typename StreamType, typename ContainerType=insertion_ordered_map<ItemType> >
void printDefinitionsSections(StreamType &oss, const std::vector<ContainerType> &secs, const std::string &secName, std::size_t indendSize)
{
    for(const auto &s: secs)
    {
        printDefinitionsSection<FlagType, ItemType, StreamType, ContainerType>(oss, s, secName, indendSize);
    }
}

//----------------------------------------------------------------------------


} // namespace utils

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& StateMachineDefinition::print(StreamType& oss, std::size_t indendSize) const
{
    auto chIndSize = indendSize + 4u;
    auto indend = std::string(indendSize, ' ');
    auto chInd  = std::string(chIndSize , ' '); // child indend

    oss << indend;

    if ((flags&StateMachineFlags::stateMachine)!=0)
        oss << "state-machine";
    else
        oss << "definitions";

    oss << " " << name;

    if (!parents.empty())
    {
        // oss << ":";

        auto cnt = 0;
        for(const auto &kv: parents)
        {
            oss << "\n" << chInd << (cnt?", ":": ") << kv.second;
            ++cnt;
        }
    }

    if (!description.empty())
    {
        if (!parents.empty())
            oss << "\n" << chInd << "- ";
        else
            oss << " - ";

        oss << "\"" << marty_cpp::cEscapeString(description) << "\"";
    }

    oss << "\n" << indend << "{\n";

    {
        std::vector< insertion_ordered_map<EventDefinition> > secs;
        utils::splitByOverrideFlag<EventFlags, EventDefinition>(events, secs);
        utils::printDefinitionsSections<EventFlags, EventDefinition, StreamType>(oss, secs, "events", indendSize+4);
    }

    {
        std::vector< insertion_ordered_map<ActionDefinition> > secs;
        utils::splitByOverrideFlag<ActionFlags, ActionDefinition>(actions, secs);
        utils::printDefinitionsSections<ActionFlags, ActionDefinition, StreamType>(oss, secs, "actions", indendSize+4);
    }

    {
        std::vector< insertion_ordered_map<PredicateDefinition> > secs;
        utils::splitByOverrideFlag<PredicateFlags, PredicateDefinition>(predicates, secs);
        utils::printDefinitionsSections<PredicateFlags, PredicateDefinition, StreamType>(oss, secs, "predicates", indendSize+4);
    }

    {
        std::vector< insertion_ordered_map<StateDefinition> > secs;
        utils::splitByOverrideFlag<StateFlags, StateDefinition>(states, secs);
        utils::printDefinitionsSections<StateFlags, StateDefinition, StreamType>(oss, secs, "states", indendSize+4);
    }

    {
        std::vector< insertion_ordered_map<TransitionDefinition> > secs;
        utils::splitByOverrideFlag<TransitionFlags, TransitionDefinition>(transitions, secs);
        utils::printDefinitionsSections<TransitionFlags, TransitionDefinition, StreamType>(oss, secs, "transitions", indendSize+4);
    }

    oss << indend << "}\n";

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& NamespaceDefinition::print(StreamType& oss, std::size_t indendSize) const
{
    auto indend = std::string(indendSize, ' ');

    std::size_t indendInc = 0u;

    if (!name.empty())
    {
        oss << indend << "namespace " << name << "\n";
        oss << indend << "{\n";
        indendInc = 4u;
    }

    using umba::tokenizer::ufsm::print;

    for(const auto &nseKv: members)
        print(oss, nseKv.second, indendSize+indendInc);

    if (!name.empty())
    {
        oss << indend << "}\n";
    }

    return oss;
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& print(StreamType &oss, const NamespaceEntry &e, std::size_t indendSize)
{
    return std::visit( [&](const auto &a) -> StreamType&
                       {
                           using ArgType = std::decay_t<decltype(a)>;
                           if constexpr (std::is_same_v <ArgType, NamespaceDefinition    >) return a.print(oss, indendSize);
                           if constexpr (std::is_same_v <ArgType, StateMachineDefinition >) return a.print(oss, indendSize);
                       }
                     , e
                     );
}

//----------------------------------------------------------------------------
template<typename StreamType>
StreamType& print(StreamType& oss, const NamespaceDefinition &nsd, std::size_t indendSize=0)
{
    return nsd.print(oss, indendSize);
}


// //----------------------------------------------------------------------------
// template<typename StreamType>
// StreamType& operator<<(StreamType &oss, const &v)
// {
//     return oss;
// }
//  
// //----------------------------------------------------------------------------





//----------------------------------------------------------------------------






//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


#include "impl/basic_types.h"


