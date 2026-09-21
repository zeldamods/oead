from __future__ import annotations
import collections.abc
import oead
from oead import BufferBool as BoolArray
from oead import BufferF32 as FloatArray
from oead import BufferInt as IntArray
from oead import BufferString as StringArray
import typing
_T = typing.TypeVar("_T")
__all__: list[str] = ['BoolArray', 'Field', 'FieldArray', 'FloatArray', 'IntArray', 'Sheet', 'StringArray', 'Struct', 'StructArray', 'parse', 'test_roundtrip']
class Field:
    """
    Grezzo datasheet field.
    """
    class Flag:
        """
        Members:
        
          IsNullable
        
          IsArray
        
          IsKey
        
          Unknown3
        
          IsEnum
        
          Unknown5
        """
        IsArray: typing.ClassVar[Field.Flag]  # value = <Flag.IsArray: 2>
        IsEnum: typing.ClassVar[Field.Flag]  # value = <Flag.IsEnum: 16>
        IsKey: typing.ClassVar[Field.Flag]  # value = <Flag.IsKey: 4>
        IsNullable: typing.ClassVar[Field.Flag]  # value = <Flag.IsNullable: 1>
        Unknown3: typing.ClassVar[Field.Flag]  # value = <Flag.Unknown3: 8>
        Unknown5: typing.ClassVar[Field.Flag]  # value = <Flag.Unknown5: 32>
        __members__: typing.ClassVar[dict[str, Field.Flag]]  # value = {'IsNullable': <Flag.IsNullable: 1>, 'IsArray': <Flag.IsArray: 2>, 'IsKey': <Flag.IsKey: 4>, 'Unknown3': <Flag.Unknown3: 8>, 'IsEnum': <Flag.IsEnum: 16>, 'Unknown5': <Flag.Unknown5: 32>}
        @typing.overload
        def __eq__(self, other: Field.Flag) -> bool:
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
        def __ne__(self, other: Field.Flag) -> bool:
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
    class Type:
        """
        Members:
        
          Struct : C/C++ style structure.
        
          Bool : Boolean.
        
          Int : Signed 32-bit integer.
        
          Float : Single-precision floating point number (binary32)..
        
          String : Null-terminated string.
        """
        Bool: typing.ClassVar[Field.Type]  # value = <Type.Bool: 1>
        Float: typing.ClassVar[Field.Type]  # value = <Type.Float: 3>
        Int: typing.ClassVar[Field.Type]  # value = <Type.Int: 2>
        String: typing.ClassVar[Field.Type]  # value = <Type.String: 4>
        Struct: typing.ClassVar[Field.Type]  # value = <Type.Struct: 0>
        __members__: typing.ClassVar[dict[str, Field.Type]]  # value = {'Struct': <Type.Struct: 0>, 'Bool': <Type.Bool: 1>, 'Int': <Type.Int: 2>, 'Float': <Type.Float: 3>, 'String': <Type.String: 4>}
        @typing.overload
        def __eq__(self, other: Field.Type) -> bool:
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
        def __ne__(self, other: Field.Type) -> bool:
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
    def __init__(self) -> None:
        ...
    def __repr__(self) -> str:
        ...
    @property
    def data_size(self) -> int:
        """
        Size of the field data. For strings and inline types (inline structs, ints, floats, bools,), this is the same as the inline size.
        """
    @data_size.setter
    def data_size(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def fields(self) -> FieldArray:
        """
        [For structs] Fields
        """
    @fields.setter
    def fields(self, arg0: FieldArray | list) -> None:
        ...
    @property
    def flags(self) -> int:
        """
        Flags.
        """
    @flags.setter
    def flags(self, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def inline_size(self) -> int:
        """
        Size of this field in the value structure. For strings and arrays, this is always 0x10.
        """
    @inline_size.setter
    def inline_size(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def name(self) -> str:
        """
        Name (must not be empty).
        """
    @name.setter
    def name(self, arg0: str) -> None:
        ...
    @property
    def offset_in_value(self) -> int:
        """
        Offset of this field in the value structure.
        """
    @offset_in_value.setter
    def offset_in_value(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def type(self) -> Field.Type:
        """
        Field type.
        """
    @type.setter
    def type(self, arg0: Field.Type) -> None:
        ...
    @property
    def type_name(self) -> str:
        """
        Type name.
        """
    @type_name.setter
    def type_name(self, arg0: str) -> None:
        ...
    @property
    def x11(self) -> int:
        """
        Unknown; depth level?
        """
    @x11.setter
    def x11(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class FieldArray:
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __contains__(self, x: Field) -> bool:
        """
        Return true the container contains ``x``
        """
    @typing.overload
    def __delitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Delete the list elements at index ``i``
        """
    @typing.overload
    def __delitem__(self, arg0: slice) -> None:
        """
        Delete list elements using a slice object
        """
    @typing.overload
    def __eq__(self, arg0: FieldArray | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: FieldArray | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> FieldArray:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> Field:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: FieldArray | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[Field]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: FieldArray | list) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: Field) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: FieldArray | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: Field) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: Field) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: FieldArray | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: Field) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> Field:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> Field:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: Field) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class Sheet:
    """
    Grezzo datasheet.
    """
    name: str
    @property
    def root_fields(self) -> FieldArray:
        ...
    @root_fields.setter
    def root_fields(self, value: FieldArray | list) -> None:
        ...
    @property
    def values(self) -> StructArray:
        ...
    @values.setter
    def values(self, value: StructArray | list) -> None:
        ...
    def __init__(self) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def to_binary(self) -> oead.Bytes:
        """
        Convert the sheet to a binary datasheet.
        """
    @property
    def alignment(self) -> int:
        ...
    @alignment.setter
    def alignment(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def hash(self) -> int:
        ...
    @hash.setter
    def hash(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
class Struct:
    """
    Grezzo datasheet struct. In this API, a Struct is represented as a dict-like object, with the field names as keys.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the map is nonempty
        """
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: str) -> bool:
        ...
    @typing.overload
    def __contains__(self, arg0: typing.Any) -> bool:
        ...
    def __delitem__(self, arg0: str) -> None:
        ...
    def __eq__(self, arg0: Struct | dict) -> bool:
        ...
    def __getitem__(self, arg0: str) -> Struct | bool | int | float | str | StructArray | oead.BufferBool | oead.BufferInt | oead.BufferF32 | oead.BufferString | None:
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
    def __iter__(self) -> collections.abc.Iterator[str]:
        ...
    def __len__(self) -> int:
        ...
    def __setitem__(self, arg0: str, arg1: Struct | dict | bool | typing.SupportsInt | typing.SupportsIndex | typing.SupportsFloat | typing.SupportsIndex | str | StructArray | list | oead.BufferBool | list | oead.BufferInt | list | oead.BufferF32 | list | oead.BufferString | list | None) -> None:
        ...
    def clear(self) -> None:
        ...
    @typing.overload
    def get(self, key: str) -> Struct | bool | int | float | str | StructArray | oead.BufferBool | oead.BufferInt | oead.BufferF32 | oead.BufferString | None:
        ...
    @typing.overload
    def get(self, key: str, default: _T) -> Struct | bool | int | float | str | StructArray | oead.BufferBool | oead.BufferInt | oead.BufferF32 | oead.BufferString | None | _T:
        ...
    def items(self) -> typing.ItemsView[str, Struct | bool | int | float | str | StructArray | oead.BufferBool | oead.BufferInt | oead.BufferF32 | oead.BufferString | None]:
        ...
    def keys(self) -> typing.KeysView[str]:
        ...
    def values(self) -> typing.ValuesView[Struct | bool | int | float | str | StructArray | oead.BufferBool | oead.BufferInt | oead.BufferF32 | oead.BufferString | None]:
        ...
class StructArray:
    """
    A list of Struct elements.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __contains__(self, x: Struct | dict) -> bool:
        """
        Return true the container contains ``x``
        """
    @typing.overload
    def __delitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Delete the list elements at index ``i``
        """
    @typing.overload
    def __delitem__(self, arg0: slice) -> None:
        """
        Delete list elements using a slice object
        """
    @typing.overload
    def __eq__(self, arg0: StructArray | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: StructArray | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> StructArray:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> Struct:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: StructArray | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[Struct]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: StructArray | list) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: Struct | dict) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: StructArray | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: Struct | dict) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: Struct | dict) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: StructArray | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: Struct | dict) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> Struct:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> Struct:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: Struct | dict) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
def parse(data: oead.Bytes | bytes) -> Sheet:
    """
    Parse a binary datasheet.
    """
def test_roundtrip(data: oead.Bytes | bytes) -> oead.Bytes:
    """
    Parse a binary datasheet and immediately dump it back for testing purposes.
    """
