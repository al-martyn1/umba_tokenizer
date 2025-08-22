- [TokenCollection](#tokencollection)

# TokenCollection

```cpp
void
setLineNumber(
  std::size_t lineNumber
);
```

```cpp
void
setFileId(
  file_id_type fileId
);
```

```cpp
file_id_type
getFileId() const;
```

```cpp
tokenizer_type &
getTokenizer();
```

```cpp
string_type &
getText();
```

#!prototype{}  tokenizer/token_collection.h#`char_type* getTextPointer`*-{};

```cpp
std::string
getTokenTextLine(
  const TokenCollectionItemType *ptki
)
const;

std::string
getTokenTextLine(
  const TextPositionInfo &textPosInfo
)
const;
```

```cpp
bool
getLastTokenizeResult() const;
```

```cpp
std::size_t
getNumFetchedTokens() const;
```

#!prototype{}  tokenizer/token_collection.h#`token_parsed_data_type* getTokenParsedData`*-{};

```cpp
TextPositionInfo
getTokenPositionInfo(
  const TokenCollectionItemType *ptki
)
const;
```

```cpp
string_type
getTokenText(
  const TokenCollectionItemType *ptki
)
const;
```

```cpp
token_pos_type
getCurTokenPos() const;
```

```cpp
token_pos_type
getNextTokenPos() const;
```

```cpp
void
setTokenPos(
  token_pos_type pos
);
```

```cpp
void
clearFetched();
```

#!prototype{}  tokenizer/token_collection.h#`TokenCollectionItemType* getToken`*-{};

#!prototype{}  tokenizer/token_collection.h#`TokenCollectionItemType* peekToken`*-{};

```cpp
std::size_t
getNumberOfTokensTotal() const;
```

```cpp
std::size_t
getNumberOfTokenDataTotal() const;
```

```cpp
std::size_t
getBytesOfTokensTotal() const;
```

```cpp
std::size_t
getBytesOfTokenDataTotal() const;
```

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};

!!! Invalid tag string: '``*-{};'
#!prototype{}  tokenizer/token_collection.h#``*-{};


Класс `TokenCollection` предназначен для:

1. хранит список токенов;
2. по команде умеет очищать выбранные из лексера токены;
