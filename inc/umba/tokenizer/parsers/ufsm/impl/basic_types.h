/*! \file
    \brief Базовые типы для парсера ufsm - реализация функций
 */

#pragma once


//----------------------------------------------------------------------------
// umba::tokenizer::ufsm::
namespace umba {
namespace tokenizer {
namespace ufsm {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline bool TransitionSourceState::isAny() const      { return (flags & TransitionSourceStateFlags::any)!=0; }

inline bool TransitionSourceState::isExcluded() const { return (flags & TransitionSourceStateFlags::excluded)!=0; }

inline
std::string TransitionSourceState::getCanonicalName() const
{
    std::string res = isAny() ? std::string("*") : name;
    return std::string(isExcluded() ? "!" : "") + res;
}

inline
int TransitionSourceState::compare(const TransitionSourceState&other) const
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

inline bool TransitionSourceState::operator< (const TransitionSourceState &other) const { return compare(other)<0; }
inline bool TransitionSourceState::isEqual   (const TransitionSourceState &other) const { return isAny()==other.isAny() && isExcluded()==other.isExcluded() && name==other.name; }
inline bool TransitionSourceState::operator==(const TransitionSourceState &other) const { return isEqual(other); }
inline bool TransitionSourceState::operator!=(const TransitionSourceState &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline bool TransitionEvent::isAny() const      { return (flags & TransitionEventFlags::any)!=0; }
inline bool TransitionEvent::isExcluded() const { return (flags & TransitionEventFlags::excluded)!=0; }

inline
std::string TransitionEvent::getCanonicalName() const
{
    std::string res = isAny() ? std::string("*") : name;
    return std::string(isExcluded() ? "!" : "") + res;
}

inline
int TransitionEvent::compare(const TransitionEvent &other) const
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

inline bool TransitionEvent::operator< (const TransitionEvent &other) const { return compare(other)<0; }
inline bool TransitionEvent::isEqual   (const TransitionEvent &other) const { return isAny()==other.isAny() && isExcluded()==other.isExcluded() && name==other.name; }
inline bool TransitionEvent::operator==(const TransitionEvent &other) const { return isEqual(other); }
inline bool TransitionEvent::operator!=(const TransitionEvent &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline void TransitionSourceStates::append   ( const TransitionSourceState &st) { stateList.push_back(st); }

inline void TransitionSourceStates::push_back( const TransitionSourceState &st) { stateList.push_back(st); }

inline
bool TransitionSourceStates::checkForAny(bool *pHasNormalAny, bool *pHasExcludedAny) const
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

inline std::vector<TransitionSourceState> TransitionSourceStates::getSortedStates() const  { auto res = stateList; std::sort(res.begin(), res.end()); return res; };

inline
std::string TransitionSourceStates::getCanonicalName() const
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

inline
bool TransitionSourceStates::isEqual(const TransitionSourceStates &other) const
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

inline
int TransitionSourceStates::compare(const TransitionSourceStates &other) const
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

inline bool TransitionSourceStates::operator< (const TransitionSourceStates &other) const { return compare(other)<0; }
inline bool TransitionSourceStates::operator==(const TransitionSourceStates &other) const { return isEqual(other); }
inline bool TransitionSourceStates::operator!=(const TransitionSourceStates &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
//struct TransitionEvents

inline void TransitionEvents::append   ( const TransitionEvent &st) { eventList.push_back(st); }
inline void TransitionEvents::push_back( const TransitionEvent &st) { eventList.push_back(st); }

inline
bool TransitionEvents::checkForAny(bool *pHasNormalAny=0, bool *pHasExcludedAny=0) const
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

inline
std::vector<TransitionEvent> TransitionEvents::getSortedEvents() const  { auto res = eventList; std::sort(res.begin(), res.end()); return res; };

inline
std::string TransitionEvents::getCanonicalName() const
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

inline
bool TransitionEvents::isEqual(const TransitionEvents &other) const
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

inline
int TransitionEvents::compare(const TransitionEvents &other) const
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

inline bool TransitionEvents::operator< (const TransitionEvents &other) const { return compare(other)<0; }
inline bool TransitionEvents::operator==(const TransitionEvents &other) const { return isEqual(other); }
inline bool TransitionEvents::operator!=(const TransitionEvents &other) const { return !isEqual(other); }

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
} // namespace ufsm
} // namespace tokenizer
} // namespace umba
// umba::tokenizer::ufsm::


