/*! \file
    \brief Утилиты
 */

#pragma once

//----------------------------------------------------------------------------
// umba::tokenizer::utils::
namespace umba {
namespace tokenizer {
namespace utils {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline std::string_view removePrefix(std::string_view &sv, std::size_t n) { sv.remove_prefix(n); return sv; }
inline std::string_view removeSuffix(std::string_view &sv, std::size_t n) { sv.remove_suffix(n); return sv; }

inline std::string removePrefix(std::string &str, std::size_t n)
{
    // basic_string& erase( size_type index = 0, size_type count = npos );
    // https://en.cppreference.com/w/cpp/string/basic_string/erase

    if (n>str.size())
        n = str.size(); // Для строк - не задаём вопросов

    str.erase(0, n);

    return str;
}

inline std::string removeSuffix(std::string &str, std::size_t n)
{
    // basic_string& erase( size_type index = 0, size_type count = npos );
    // https://en.cppreference.com/w/cpp/string/basic_string/erase

    if (n>str.size())
        n = str.size(); // Для строк - не задаём вопросов

    str.erase(str.size()-n /* , n */ ); // Удаляем до конца

    return str;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Делаем вектор string_view из строки текста. Строка разделяется только символами LF ('\n')
// Символ перевода строки также входит в string_view
inline
std::vector<std::string_view> makeTextStringViewsHelper(const std::string &text)
{
    std::vector<std::string_view> resVec; resVec.reserve(text.size()/64);

    std::size_t idxStart = 0;
    std::size_t idx      = 0;

    for(; idx!=text.size(); ++idx)
    {
        if (text[idx]=='\n')
        {
            std::size_t idxNext = idx+1;
            std::size_t size    = idxNext - idxStart;
            resVec.emplace_back(text.data()+idxStart, size);
            idxStart = idxNext;
        }
    }

    if (idxStart!=idx)
    {
        resVec.emplace_back(text.data()+idxStart, idx-idxStart);
    }

    return resVec;
}

//----------------------------------------------------------------------------
// Обрезаем переводы строки из вектора string_view
inline
std::vector<std::string_view> stripLinefeedsFromStringViewsVector(const std::vector<std::string_view> &svVec)
{
    std::vector<std::string_view> svRes = svVec;
    for(auto &sv : svRes)
    {
        while(!sv.empty())
        {
            if (sv.back()=='\r' || sv.back()=='\n')
                sv.remove_suffix(1);
            else
                break;
        }
    }

    return svRes;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename IteratorType>
std::size_t ltrim_distance(IteratorType b, IteratorType e)
{
    std::size_t idx = 0;
    for(; b!=e; ++b, ++idx)
    {
        if (!(*b==' ' || *b=='\t' || *b=='\r' || *b=='\n'))
            break;
    }
    return idx;
}

template<typename SomethingStringLike>
std::size_t ltrim_distance(const SomethingStringLike &str) { return ltrim_distance(str.begin(), str.end()); }

inline void             ltrim     (std::string_view &sv)   { removePrefix(sv, ltrim_distance(sv.begin(), sv.end())); }
inline std::string_view ltrim_copy(std::string_view  sv)   { ltrim(sv); return sv; }
inline void             ltrim     (std::string      &sv)   { removePrefix(sv, ltrim_distance(sv.begin(), sv.end())); }
inline std::string      ltrim_copy(std::string       sv)   { ltrim(sv); return sv; }

//----------------------------------------------------------------------------
template<typename IteratorType>
std::size_t rtrim_distance(IteratorType b, IteratorType e)
{
    // IteratorType bcp = b;
    std::size_t idx = 0;
    //for(; e!=b; --e, ++idx)
    //for(; e--!=b; ++idx)
    for(; e!=b; ++idx)
    {
        e--;
        if (!(*e==' ' || *e=='\t' || *e=='\r' || *e=='\n'))
            return idx;
    }
    //return std::distance(bcp, e);
    return idx;
}

    // std::size_t idx = sv.size();
    // while(idx-->0)
    // {
    //     if (!(sv[idx]==' ' || sv[idx]=='\t' || sv[idx]=='\r' || sv[idx]=='\n'))
    //         break;
    // }
    //  
    // std::size_t rmSize = sv.size()-idx-1;
    // sv.remove_suffix(rmSize);
    // sv.remove_suffix(rtrim_distance(sv.begin(), sv.end()));


//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
template<typename SomethingStringLike>
std::size_t rtrim_distance(const SomethingStringLike &str) { return rtrim_distance(str.begin(), str.end()); }

inline void             rtrim     (std::string_view &sv)   { removeSuffix(sv, rtrim_distance(sv.begin(), sv.end())); }
inline std::string_view rtrim_copy(std::string_view  sv)   { rtrim(sv); return sv; }
inline void             rtrim     (std::string      &sv)   { removeSuffix(sv, rtrim_distance(sv.begin(), sv.end())); }
inline std::string      rtrim_copy(std::string       sv)   { rtrim(sv); return sv; }

//----------------------------------------------------------------------------
inline void             trim      (std::string_view &sv)   { ltrim(sv); rtrim(sv); }
inline std::string_view trim_copy (std::string_view  sv)   { return ltrim_copy(rtrim_copy(sv)); }
inline void             trim      (std::string      &sv)   { ltrim(sv); rtrim(sv); }
inline std::string      trim_copy (std::string       sv)   { return ltrim_copy(rtrim_copy(sv)); }

//----------------------------------------------------------------------------
inline
void rtrim(std::vector<std::string_view> &svVec)
{
    for(auto &sv : svVec)
        rtrim(sv);
}

//----------------------------------------------------------------------------
inline
std::vector<std::string_view> rtrim_copy(std::vector<std::string_view> svVec)
{
    rtrim(svVec);
    return svVec;
}

//----------------------------------------------------------------------------
inline
char tosame(char ch)
{
    return ch;
}

//----------------------------------------------------------------------------
inline
char tolower(char ch)
{
    if (ch>='A' && ch<='Z')
        ch = ch - 'A' + 'a';
    return ch;
}

//----------------------------------------------------------------------------
inline
std::string tolower_copy(std::string s)
{
    for(auto &ch : s)
    {
        ch = tolower(ch);
    }

    return s;
}

//----------------------------------------------------------------------------
inline
bool is_equal_chars(char ch1, char ch2, bool ci)
{
    return ci ? tolower(ch1)==tolower(ch2) : ch1==ch2;
}

//----------------------------------------------------------------------------
inline
bool is_equal(std::string_view sv1, std::string_view sv2)
{
    if (sv1.size()!=sv2.size())
        return false;

    return sv1==sv2;
}

//----------------------------------------------------------------------------
inline
bool is_equal(std::string_view sv, const std::string &str)
{
    return is_equal(sv, std::string_view(str.data(), str.size()));
}

//----------------------------------------------------------------------------
inline
bool is_equal(const std::string &str, std::string_view sv)
{
    return is_equal(sv, std::string_view(str.data(), str.size()));
}

//----------------------------------------------------------------------------
template<typename CharEqual>
bool is_equal(std::string_view sv1, std::string_view sv2, CharEqual charEqual)
{
    if (sv1.size()!=sv2.size())
        return false;

    auto it1 = sv1.begin();
    auto it2 = sv2.begin();

    for(; it1!=sv1.end(); ++it1, ++it2)
    {
        if (!charEqual(*it1, *it2))
            return false;
    }

    return true;
}

//----------------------------------------------------------------------------
template<typename CharEqual>
bool is_equal(std::string_view sv, const std::string &str, CharEqual charEqual)
{
    return is_equal(sv, std::string_view(str.data(), str.size()), charEqual);
}

//----------------------------------------------------------------------------
template<typename CharEqual>
bool is_equal(const std::string &str, std::string_view sv, CharEqual charEqual)
{
    return is_equal_ci(sv, std::string_view(str.data(), str.size()), charEqual);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Модифицирует содержимое вьюхи, забивая все непробельные символы
// Так издеваться можно только над вьюхами, которые сделаны из неконстантных массивов/строк
// Вполне можно использовать, если часть строк в тексте, которые представлены вьюхами, надо забить пробелами
inline
void make_string_view_space_only(std::string_view &sv)
{
    auto ptr = const_cast<char*>(sv.data());
    for(std::size_t i=0u; i!=sv.size(); ++i)
    {
        ptr[i] = ' ';
    }
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// Про YAML - https://habr.com/ru/articles/710414/
// JSON - https://www.json.org/json-en.html
// The Jekyll static site generator popularized YAML front matter which is deliminated by YAML section markers.
inline
bool extractYamlFrontMatter(std::vector<std::string_view> &svVec, std::vector<std::string> *pfm=0)
{
    if (svVec.empty())
        return false;

    const std::string_view hiphenus = "---";
    if (rtrim_copy(svVec[0])!=hiphenus)
        return false;

    std::size_t idx = 1;
    for(; idx!=svVec.size(); ++idx)
    {
        if (rtrim_copy(svVec[idx])==hiphenus)
        {
            if (pfm)
                *pfm = std::vector<std::string>(svVec.begin()+std::ptrdiff_t(1), svVec.begin()+std::ptrdiff_t(idx));
            std::size_t size = idx+1;
            for(idx=0; idx!=size; ++idx)
            {
                make_string_view_space_only(svVec[idx]);
                //svVec[idx] = std::string_view();
            }
            return true;
        }
    }

    return false;
}

//----------------------------------------------------------------------------
// Строки должны быть rtrimmed
inline
bool extractMarkeredPart(std::vector<std::string_view> &svVec, const std::string &startMarker, const std::string &endMarker, bool ci, std::vector<std::string> *pMarkered=0)
{
    auto isEqual = [&](auto s1, auto s2)
    {
        return is_equal( s1, s2, [&](char ch1, char ch2) { return is_equal_chars(ch1, ch2, ci); });
    };


    std::size_t idx = 0;
    for(; idx!=svVec.size(); ++idx)
    {
        if (isEqual(rtrim_copy(svVec[idx]), startMarker))
           break;
    }

    if (idx==svVec.size())
        return false;

    std::size_t idxStart = idx;

    for(++idx; idx!=svVec.size(); ++idx)
    {
        if (isEqual(rtrim_copy(svVec[idx]), endMarker))
           break;
    }

    if (idx==svVec.size())
        return false;

    std::size_t idxEnd = idx+1;

    if (pMarkered)
    {
        *pMarkered = std::vector<std::string>(svVec.begin()+std::ptrdiff_t(idxStart), svVec.begin()+std::ptrdiff_t(idxEnd));
    }

    for(idx=idxStart; idx!=idxEnd; ++idx)
    {
        make_string_view_space_only(svVec[idx]);
    }

    return true;
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringType>
StringType concat_copy(const StringType &s1, const StringType &s2, const StringType &sep)
{
    return rtrim_copy(s1) + sep + ltrim_copy(s2);
}

template<typename StringType>
void concat(StringType &concatTo, const StringType &str, const StringType &sep)
{
    concatTo = concat_copy(concatTo, str, sep);
}

//----------------------------------------------------------------------------
// Разбирает ямловский заголовок, считая, что один параметр - ровно одна строка. Значения тэгов приводятся к нижнему регистру и кладутся в мапу
// Возвращает true, если был найден хоть один тэг
inline
bool simpleParseYamlFrontMatter(const std::vector<std::string> &lines, std::unordered_map<std::string, std::string> &tags)
{
    UMBA_USED(lines);
    UMBA_USED(tags);

    if (lines.empty())
        return false;

    std::size_t baseIndent = 0;

    std::size_t lineIdx = 0;

    // Ищем базовый отступ
    for(; lineIdx!=lines.size(); ++lineIdx)
    {
        auto ltrDistance = ltrim_distance(lines[lineIdx]);
        if (ltrDistance>=lines[lineIdx].size()) // строка пустая
            continue;
        if (ltrDistance>baseIndent)
            baseIndent = ltrDistance;
        break;
    }

    std::string lastKey;

    // Базовый отступ есть, если строка имеет другой отступ, она конкатенируется к предыдущей
    for(lineIdx=0; lineIdx!=lines.size(); ++lineIdx)
    {
        const auto &line = lines[lineIdx];
        auto ltrDistance = ltrim_distance(line);
        if (ltrDistance!=baseIndent)
        {
            concat(tags[lastKey], trim_copy(line), std::string(" "));
        }
        else
        {
            auto colonPos = line.find(':');
            if (colonPos==line.npos)
            {
                lastKey = tolower_copy(trim_copy(line));
            }
            else
            {
                lastKey       = tolower_copy(trim_copy(std::string(line, 0, colonPos)));
                tags[lastKey] = trim_copy(std::string(line, colonPos+1, line.npos));
            }
        }
    }
    
    return !tags.empty();
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// text must be LF only
inline
void prepareTextForParsing(const std::string &text, std::vector<std::string> *pStyle, std::unordered_map<std::string, std::string> *pFmTags)
{
    auto viewsVec = rtrim_copy(makeTextStringViewsHelper(text));

    std::vector<std::string> fmLines;
    if (extractYamlFrontMatter(viewsVec, &fmLines) && pFmTags)
    {
        simpleParseYamlFrontMatter(fmLines, *pFmTags);
    }

    extractMarkeredPart(viewsVec, "<style>", "</style>", true, pStyle);
}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace utils
} // namespace tokenizer
} // namespace umba
