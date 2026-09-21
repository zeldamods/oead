from __future__ import annotations
import collections.abc
import oead
import typing
import typing_extensions
_T = typing.TypeVar("_T")
__all__: list[str] = ['Array', 'Hash', 'from_binary', 'from_text', 'get_bool', 'get_double', 'get_float', 'get_int', 'get_int64', 'get_string', 'get_uint', 'get_uint64', 'to_binary', 'to_text']
class Array:
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __contains__(self, x: None | str | oead.Bytes | bytes | Array | list | Hash | dict | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64) -> bool:
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
    def __eq__(self, arg0: Array | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: Array | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> Array:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: Array | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: Array | list) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: None | str | oead.Bytes | bytes | Array | list | Hash | dict | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: Array | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: None | str | oead.Bytes | bytes | Array | list | Hash | dict | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: None | str | oead.Bytes | bytes | Array | list | Hash | dict | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: Array | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: None | str | oead.Bytes | bytes | Array | list | Hash | dict | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: None | str | oead.Bytes | bytes | Array | list | Hash | dict | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class Hash:
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
    def __eq__(self, arg0: Hash | dict) -> bool:
        ...
    def __getitem__(self, arg0: str) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64:
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
    def __setitem__(self, arg0: str, arg1: None | str | oead.Bytes | bytes | Array | list | Hash | dict | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64) -> None:
        ...
    def clear(self) -> None:
        ...
    @typing.overload
    def get(self, key: str) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64:
        ...
    @typing.overload
    def get(self, key: str, default: _T) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64 | _T:
        ...
    def items(self) -> typing.ItemsView[str, None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64]:
        ...
    def keys(self) -> typing.KeysView[str]:
        ...
    def values(self) -> typing.ValuesView[None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64]:
        ...
def from_binary(buffer: typing_extensions.Buffer) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64:
    """
    :return: An Array or a Hash.
    """
def from_text(yml_text: str) -> None | str | oead.Bytes | Array | Hash | bool | oead.S32 | oead.F32 | oead.U32 | oead.S64 | oead.U64 | oead.F64:
    """
    :return: An Array or a Hash.
    """
def get_bool(data: typing.Any) -> bool:
    ...
def get_double(data: typing.Any) -> float:
    ...
def get_float(data: typing.Any) -> float:
    ...
def get_int(data: typing.Any) -> int:
    ...
def get_int64(data: typing.Any) -> int:
    ...
def get_string(data: typing.Any) -> str:
    ...
def get_uint(data: typing.Any) -> int:
    ...
def get_uint64(data: typing.Any) -> int:
    ...
def to_binary(data: typing.Any, big_endian: bool, version: typing.SupportsInt | typing.SupportsIndex = 2) -> oead.Bytes:
    ...
def to_text(data: typing.Any) -> str:
    ...
