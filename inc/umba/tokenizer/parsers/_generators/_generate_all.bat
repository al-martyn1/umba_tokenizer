@set UINT8=--underlaying-type=std::uint8_t
@set UINT16=--underlaying-type=std::uint16_t
@set UINT32=--underlaying-type=std::uint32_t
@set INT=--underlaying-type=int

@set HEX2=--hex-width=2
@set HEX4=--hex-width=4
@set HEX6=--hex-width=6
@set HEX8=--hex-width=8

@rem set EXTRA=--enum-flags=extra
@rem set FLAGENUM_EXTRA=--enum-flags=serialize-set,deserialize-set

@set FLAGS=--enum-flags=flags
@set DECL=--enum-flags=type-decl
@set CLS=--enum-flags=enum-class
@set DECLCLS=--enum-flags=type-decl,enum-class
@set FMTHEX=--enum-flags=fmt-hex
@set SERIALIZE=--enum-flags=serialize,deserialize,lowercase

@set TPL_OVERRIDE=--override-template-parameter=EnumNameFormat:$(ENAMNAME)
@set GEN_OPTS=--enum-values-style=CamelStyle

@set VALUES_CAMEL=--enum-values-style=CamelStyle
@set SERIALIZE_CAMEL=--serialize-style=CamelStyle
@set VALUES_PASCAL=--enum-values-style=PascalStyle
@set SERIALIZE_PASCAL=--serialize-style=PascalStyle
@set SERIALIZE_HYPHEN=--serialize-style=HyphenStyle


@set SNIPPETOPTIONS_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL% --enum-serialize-style=All

@set FLAGS=--enum-flags=flags

@rem --override-template-parameter=EnumNameFormat:E$(ENAMNAME)                                                ^

umba-enum-gen %GEN_OPTS% %HEX2% %TPL_OVERRIDE% %SNIPPETOPTIONS_GEN_FLAGS%                                    ^
    --namespace=umba/tokenizer/marmaid                                                                       ^
    --override-template-parameter=EnumNameFormat:$(ENAMNAME)                                                 ^
    %UINT32% %HEX4% -E=PacketDiagramDisplayOptions          -F=@MarmaidPacketDiagramDisplayOptions.txt       ^
    %UINT32% %HEX4% -E=PacketDiagramType                    -F=@MarmaidPacketDiagramType.txt                 ^
    %UINT32% %HEX4% -E=MemoryModel                          -F=@MarmaidPacketDiagramMemoryModel.txt          ^
    %FLAGS%                                                                                                  ^
    %UINT32% %HEX4% -E=PacketDiagramDisplayOptionFlags      -F=@MarmaidPacketDiagramDisplayOptionFlags.txt   ^
    %UINT32% %HEX4% -E=PacketDiagramParsingOptions          -F=@MarmaidPacketDiagramParsingOptions.txt       ^
..\marmaid_packet_diagram_enums.h

