from __future__ import annotations
import collections.abc
import oead
import typing
import typing_extensions
_T = typing.TypeVar("_T")
_ParameterValue: typing.TypeAlias = bool | typing.SupportsFloat | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | oead.Vector2f | oead.Vector3f | oead.Vector4f | oead.Color4f | oead.FixedSafeString32 | oead.FixedSafeString64 | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(1)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(2)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(3)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(4)"] | oead.BufferInt | list | oead.BufferF32 | list | oead.FixedSafeString256 | oead.Quatf | oead.U32 | oead.BufferU32 | list | oead.Bytes | bytes | str
__all__: list[str] = ['Name', 'NameTable', 'Parameter', 'ParameterIO', 'ParameterList', 'ParameterListMap', 'ParameterMap', 'ParameterObject', 'ParameterObjectMap', 'get_default_name_table']
class Name:
    def __eq__(self, arg0: Name | int | str) -> bool:
        ...
    def __hash__(self) -> int:
        ...
    @typing.overload
    def __init__(self, name_crc32: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @typing.overload
    def __init__(self, name: str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
    @property
    def hash(self) -> int:
        ...
class NameTable:
    def __copy__(self) -> NameTable:
        ...
    def __deepcopy__(self, arg0: dict) -> NameTable:
        ...
    def __init__(self, with_botw_strings: bool) -> None:
        ...
    def add_name(self, name: str) -> str:
        ...
    def get_name(self, hash: typing.SupportsInt | typing.SupportsIndex, index: typing.SupportsInt | typing.SupportsIndex, parent_name_hash: typing.SupportsInt | typing.SupportsIndex) -> str | None:
        ...
class Parameter:
    class Type:
        """
        Members:
        
          Bool
        
          F32
        
          Int
        
          Vec2
        
          Vec3
        
          Vec4
        
          Color
        
          String32
        
          String64
        
          Curve1
        
          Curve2
        
          Curve3
        
          Curve4
        
          BufferInt
        
          BufferF32
        
          String256
        
          Quat
        
          U32
        
          BufferU32
        
          BufferBinary
        
          StringRef
        """
        Bool: typing.ClassVar[Parameter.Type]  # value = <Type.Bool: 0>
        BufferBinary: typing.ClassVar[Parameter.Type]  # value = <Type.BufferBinary: 19>
        BufferF32: typing.ClassVar[Parameter.Type]  # value = <Type.BufferF32: 14>
        BufferInt: typing.ClassVar[Parameter.Type]  # value = <Type.BufferInt: 13>
        BufferU32: typing.ClassVar[Parameter.Type]  # value = <Type.BufferU32: 18>
        Color: typing.ClassVar[Parameter.Type]  # value = <Type.Color: 6>
        Curve1: typing.ClassVar[Parameter.Type]  # value = <Type.Curve1: 9>
        Curve2: typing.ClassVar[Parameter.Type]  # value = <Type.Curve2: 10>
        Curve3: typing.ClassVar[Parameter.Type]  # value = <Type.Curve3: 11>
        Curve4: typing.ClassVar[Parameter.Type]  # value = <Type.Curve4: 12>
        F32: typing.ClassVar[Parameter.Type]  # value = <Type.F32: 1>
        Int: typing.ClassVar[Parameter.Type]  # value = <Type.Int: 2>
        Quat: typing.ClassVar[Parameter.Type]  # value = <Type.Quat: 16>
        String256: typing.ClassVar[Parameter.Type]  # value = <Type.String256: 15>
        String32: typing.ClassVar[Parameter.Type]  # value = <Type.String32: 7>
        String64: typing.ClassVar[Parameter.Type]  # value = <Type.String64: 8>
        StringRef: typing.ClassVar[Parameter.Type]  # value = <Type.StringRef: 20>
        U32: typing.ClassVar[Parameter.Type]  # value = <Type.U32: 17>
        Vec2: typing.ClassVar[Parameter.Type]  # value = <Type.Vec2: 3>
        Vec3: typing.ClassVar[Parameter.Type]  # value = <Type.Vec3: 4>
        Vec4: typing.ClassVar[Parameter.Type]  # value = <Type.Vec4: 5>
        __members__: typing.ClassVar[dict[str, Parameter.Type]]  # value = {'Bool': <Type.Bool: 0>, 'F32': <Type.F32: 1>, 'Int': <Type.Int: 2>, 'Vec2': <Type.Vec2: 3>, 'Vec3': <Type.Vec3: 4>, 'Vec4': <Type.Vec4: 5>, 'Color': <Type.Color: 6>, 'String32': <Type.String32: 7>, 'String64': <Type.String64: 8>, 'Curve1': <Type.Curve1: 9>, 'Curve2': <Type.Curve2: 10>, 'Curve3': <Type.Curve3: 11>, 'Curve4': <Type.Curve4: 12>, 'BufferInt': <Type.BufferInt: 13>, 'BufferF32': <Type.BufferF32: 14>, 'String256': <Type.String256: 15>, 'Quat': <Type.Quat: 16>, 'U32': <Type.U32: 17>, 'BufferU32': <Type.BufferU32: 18>, 'BufferBinary': <Type.BufferBinary: 19>, 'StringRef': <Type.StringRef: 20>}
        @typing.overload
        def __eq__(self, other: Parameter.Type) -> bool:
            ...
        @typing.overload
        def __eq__(self, other: typing.Any) -> bool:
            ...
        def __getstate__(self) -> int:
            ...
        def __hash__(self) -> int:
            ...
        def __index__(self) -> int:
            ...
        def __init__(self, value: typing.SupportsInt | typing.SupportsIndex) -> None:
            ...
        def __int__(self) -> int:
            ...
        @typing.overload
        def __ne__(self, other: Parameter.Type) -> bool:
            ...
        @typing.overload
        def __ne__(self, other: typing.Any) -> bool:
            ...
        def __repr__(self) -> str:
            ...
        def __setstate__(self, state: typing.SupportsInt | typing.SupportsIndex) -> None:
            ...
        def __str__(self) -> str:
            ...
        @property
        def name(self) -> str:
            ...
        @property
        def value(self) -> int:
            ...
    __hash__: typing.ClassVar[None] = None
    def __copy__(self) -> Parameter:
        ...
    def __deepcopy__(self, arg0: dict) -> Parameter:
        ...
    def __eq__(self, arg0: Parameter | _ParameterValue) -> bool:
        ...
    @typing.overload
    def __init__(self, arg0: oead.F32) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: bool | typing.SupportsFloat | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | oead.Vector2f | oead.Vector3f | oead.Vector4f | oead.Color4f | oead.FixedSafeString32 | oead.FixedSafeString64 | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(1)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(2)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(3)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(4)"] | oead.BufferInt | list | oead.BufferF32 | list | oead.FixedSafeString256 | oead.Quatf | oead.U32 | oead.BufferU32 | list | oead.Bytes | bytes | str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
    def type(self) -> Parameter.Type:
        ...
    @property
    def v(self) -> bool | float | int | oead.Vector2f | oead.Vector3f | oead.Vector4f | oead.Color4f | oead.FixedSafeString32 | oead.FixedSafeString64 | typing.Annotated[list[oead.Curve], "FixedSize(1)"] | typing.Annotated[list[oead.Curve], "FixedSize(2)"] | typing.Annotated[list[oead.Curve], "FixedSize(3)"] | typing.Annotated[list[oead.Curve], "FixedSize(4)"] | oead.BufferInt | oead.BufferF32 | oead.FixedSafeString256 | oead.Quatf | oead.U32 | oead.BufferU32 | oead.Bytes | str:
        """
        Value
        """
    @v.setter
    def v(self, arg1: bool | typing.SupportsFloat | typing.SupportsIndex | typing.SupportsInt | typing.SupportsIndex | oead.Vector2f | oead.Vector3f | oead.Vector4f | oead.Color4f | oead.FixedSafeString32 | oead.FixedSafeString64 | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(1)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(2)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(3)"] | typing.Annotated[collections.abc.Sequence[oead.Curve], "FixedSize(4)"] | oead.BufferInt | list | oead.BufferF32 | list | oead.FixedSafeString256 | oead.Quatf | oead.U32 | oead.BufferU32 | list | oead.Bytes | bytes | str) -> None:
        ...
class ParameterIO(ParameterList):
    __hash__: typing.ClassVar[None] = None
    type: str
    @staticmethod
    def from_binary(buffer: typing_extensions.Buffer) -> ParameterIO:
        ...
    @staticmethod
    def from_text(yml_text: str) -> ParameterIO:
        ...
    def __copy__(self) -> ParameterIO:
        ...
    def __deepcopy__(self, arg0: dict) -> ParameterIO:
        ...
    def __eq__(self, arg0: ParameterIO) -> bool:
        ...
    def __init__(self) -> None:
        ...
    def to_binary(self) -> oead.Bytes:
        ...
    def to_text(self) -> str:
        ...
    @property
    def version(self) -> int:
        ...
    @version.setter
    def version(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class ParameterList:
    __hash__: typing.ClassVar[None] = None
    @property
    def lists(self) -> ParameterListMap:
        ...
    @lists.setter
    def lists(self, value: ParameterListMap | dict) -> None:
        ...
    @property
    def objects(self) -> ParameterObjectMap:
        ...
    @objects.setter
    def objects(self, value: ParameterObjectMap | dict) -> None:
        ...
    def __copy__(self) -> ParameterList:
        ...
    def __deepcopy__(self, arg0: dict) -> ParameterList:
        ...
    def __eq__(self, arg0: ParameterList) -> bool:
        ...
    def __init__(self) -> None:
        ...
class ParameterListMap:
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the map is nonempty
        """
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: Name | int | str) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    def __delitem__(self, arg0: Name | int | str) -> None:
        ...
    def __eq__(self, arg0: ParameterListMap | dict) -> bool:
        ...
    def __getitem__(self, arg0: Name | int | str) -> ParameterList:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, iterator: collections.abc.Iterator) -> None:
        ...
    @typing.overload
    def __init__(self, dictionary: dict) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[Name]:
        ...
    def __len__(self) -> int:
        ...
    def __setitem__(self, arg0: Name | int | str, arg1: ParameterList) -> None:
        ...
    def clear(self) -> None:
        ...
    @typing.overload
    def get(self, key: Name | int | str) -> ParameterList | None:
        ...
    @typing.overload
    def get(self, key: Name | int | str, default: _T) -> ParameterList | _T:
        ...
    def items(self) -> typing.ItemsView[Name, ParameterList]:
        ...
    def keys(self) -> typing.KeysView[Name]:
        ...
    def values(self) -> typing.ValuesView[ParameterList]:
        ...
class ParameterMap:
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the map is nonempty
        """
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: Name | int | str) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    def __delitem__(self, arg0: Name | int | str) -> None:
        ...
    def __eq__(self, arg0: ParameterMap | dict) -> bool:
        ...
    def __getitem__(self, arg0: Name | int | str) -> Parameter:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, iterator: collections.abc.Iterator) -> None:
        ...
    @typing.overload
    def __init__(self, dictionary: dict) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[Name]:
        ...
    def __len__(self) -> int:
        ...
    def __setitem__(self, arg0: Name | int | str, arg1: Parameter | _ParameterValue) -> None:
        ...
    def clear(self) -> None:
        ...
    @typing.overload
    def get(self, key: Name | int | str) -> Parameter | None:
        ...
    @typing.overload
    def get(self, key: Name | int | str, default: _T) -> Parameter | _T:
        ...
    def items(self) -> typing.ItemsView[Name, Parameter]:
        ...
    def keys(self) -> typing.KeysView[Name]:
        ...
    def values(self) -> typing.ValuesView[Parameter]:
        ...
class ParameterObject:
    __hash__: typing.ClassVar[None] = None
    @property
    def params(self) -> ParameterMap:
        ...
    @params.setter
    def params(self, value: ParameterMap | dict) -> None:
        ...
    def __copy__(self) -> ParameterObject:
        ...
    def __deepcopy__(self, arg0: dict) -> ParameterObject:
        ...
    def __eq__(self, arg0: ParameterObject) -> bool:
        ...
    def __init__(self) -> None:
        ...
class ParameterObjectMap:
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the map is nonempty
        """
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: Name | int | str) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    def __delitem__(self, arg0: Name | int | str) -> None:
        ...
    def __eq__(self, arg0: ParameterObjectMap | dict) -> bool:
        ...
    def __getitem__(self, arg0: Name | int | str) -> ParameterObject:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, iterator: collections.abc.Iterator) -> None:
        ...
    @typing.overload
    def __init__(self, dictionary: dict) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[Name]:
        ...
    def __len__(self) -> int:
        ...
    def __setitem__(self, arg0: Name | int | str, arg1: ParameterObject) -> None:
        ...
    def clear(self) -> None:
        ...
    @typing.overload
    def get(self, key: Name | int | str) -> ParameterObject | None:
        ...
    @typing.overload
    def get(self, key: Name | int | str, default: _T) -> ParameterObject | _T:
        ...
    def items(self) -> typing.ItemsView[Name, ParameterObject]:
        ...
    def keys(self) -> typing.KeysView[Name]:
        ...
    def values(self) -> typing.ValuesView[ParameterObject]:
        ...
def get_default_name_table() -> NameTable:
    """
    Just like in C++, this returns the default instance of the name table. It is modifiable.
    """
