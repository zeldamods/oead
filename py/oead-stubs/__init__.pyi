from __future__ import annotations
import collections.abc
import typing
import typing_extensions
from . import aamp
from . import audio
from . import byml
from . import gsheet
from . import yaz0
_T = typing.TypeVar("_T")
_ParameterValue: typing.TypeAlias = float
__all__: list[str] = ['BufferBool', 'BufferF32', 'BufferInt', 'BufferString', 'BufferU32', 'Bytes', 'Color4f', 'Curve', 'Endianness', 'F32', 'F64', 'File', 'FixedSafeString128', 'FixedSafeString16', 'FixedSafeString256', 'FixedSafeString32', 'FixedSafeString48', 'FixedSafeString64', 'InvalidDataError', 'Quatf', 'S16', 'S32', 'S64', 'S8', 'Sarc', 'SarcWriter', 'TypeError', 'U16', 'U32', 'U64', 'U8', 'Vector2f', 'Vector3f', 'Vector4f', 'aamp', 'audio', 'byml', 'gsheet', 'yaz0']
class BufferBool:
    """
    Mutable list-like object that stores booleans.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __contains__(self, x: bool) -> bool:
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
    def __eq__(self, arg0: BufferBool | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: BufferBool | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> BufferBool:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: BufferBool | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[bool]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: BufferBool | list) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: bool) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: BufferBool | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: bool) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: bool) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: BufferBool | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: bool) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> bool:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> bool:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: bool) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class BufferF32:
    """
    Mutable list-like object that stores binary32 floats.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __buffer__(self, flags: int, /) -> memoryview:
        """
        Return a buffer object that exposes the underlying memory of the object.
        """
    def __contains__(self, x: typing.SupportsFloat | typing.SupportsIndex) -> bool:
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
    def __eq__(self, arg0: BufferF32 | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: BufferF32 | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> BufferF32:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> float:
        ...
    @typing.overload
    def __init__(self, arg0: typing_extensions.Buffer) -> None:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: BufferF32 | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[float]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: BufferF32 | list) -> bool:
        ...
    def __release_buffer__(self, buffer: memoryview, /) -> None:
        """
        Release the buffer object that exposes the underlying memory of the object.
        """
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: BufferF32 | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: typing.SupportsFloat | typing.SupportsIndex) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: BufferF32 | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> float:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> float:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class BufferInt:
    """
    Mutable list-like object that stores signed 32-bit integers.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __buffer__(self, flags: int, /) -> memoryview:
        """
        Return a buffer object that exposes the underlying memory of the object.
        """
    def __contains__(self, x: typing.SupportsInt | typing.SupportsIndex) -> bool:
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
    def __eq__(self, arg0: BufferInt | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: BufferInt | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> BufferInt:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> int:
        ...
    @typing.overload
    def __init__(self, arg0: typing_extensions.Buffer) -> None:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: BufferInt | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[int]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: BufferInt | list) -> bool:
        ...
    def __release_buffer__(self, buffer: memoryview, /) -> None:
        """
        Release the buffer object that exposes the underlying memory of the object.
        """
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: BufferInt | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: typing.SupportsInt | typing.SupportsIndex) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: BufferInt | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> int:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> int:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class BufferString:
    """
    Mutable list-like object that stores strings.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __contains__(self, x: str) -> bool:
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
    def __eq__(self, arg0: BufferString | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: BufferString | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> BufferString:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> str:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: BufferString | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[str]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: BufferString | list) -> bool:
        ...
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: str) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: BufferString | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: str) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: str) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: BufferString | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: str) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> str:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> str:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: str) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class BufferU32:
    """
    Mutable list-like object that stores unsigned 32-bit integers.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __buffer__(self, flags: int, /) -> memoryview:
        """
        Return a buffer object that exposes the underlying memory of the object.
        """
    def __contains__(self, x: typing.SupportsInt | typing.SupportsIndex) -> bool:
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
    def __eq__(self, arg0: BufferU32 | list) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: BufferU32 | list) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> BufferU32:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> int:
        ...
    @typing.overload
    def __init__(self, arg0: typing_extensions.Buffer) -> None:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: BufferU32 | list) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[int]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: BufferU32 | list) -> bool:
        ...
    def __release_buffer__(self, buffer: memoryview, /) -> None:
        """
        Release the buffer object that exposes the underlying memory of the object.
        """
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: BufferU32 | list) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: typing.SupportsInt | typing.SupportsIndex) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: BufferU32 | list) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> int:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> int:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class Bytes:
    """
    Mutable bytes-like object. This is used to avoid possibly expensive data copies.
    """
    __hash__: typing.ClassVar[None] = None
    def __bool__(self) -> bool:
        """
        Check whether the list is nonempty
        """
    def __buffer__(self, flags: int, /) -> memoryview:
        """
        Return a buffer object that exposes the underlying memory of the object.
        """
    def __contains__(self, x: typing.SupportsInt | typing.SupportsIndex) -> bool:
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
    def __eq__(self, arg0: Bytes | bytes) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: Bytes | bytes) -> bool:
        ...
    @typing.overload
    def __getitem__(self, s: slice) -> Bytes:
        """
        Retrieve list elements using a slice object
        """
    @typing.overload
    def __getitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> int:
        ...
    @typing.overload
    def __init__(self, arg0: typing_extensions.Buffer) -> None:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: Bytes | bytes) -> None:
        """
        Copy constructor
        """
    @typing.overload
    def __init__(self, arg0: collections.abc.Iterable) -> None:
        ...
    def __iter__(self) -> collections.abc.Iterator[int]:
        ...
    def __len__(self) -> int:
        ...
    def __ne__(self, arg0: Bytes | bytes) -> bool:
        ...
    def __release_buffer__(self, buffer: memoryview, /) -> None:
        """
        Release the buffer object that exposes the underlying memory of the object.
        """
    def __repr__(self) -> str:
        ...
    @typing.overload
    def __setitem__(self, arg0: typing.SupportsInt | typing.SupportsIndex, arg1: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @typing.overload
    def __setitem__(self, arg0: slice, arg1: Bytes | bytes) -> None:
        """
        Assign list elements using a slice object
        """
    def append(self, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Add an item to the end of the list
        """
    def clear(self) -> None:
        """
        Clear the contents
        """
    def count(self, x: typing.SupportsInt | typing.SupportsIndex) -> int:
        """
        Return the number of times ``x`` appears in the list
        """
    @typing.overload
    def extend(self, L: Bytes | bytes) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    @typing.overload
    def extend(self, L: collections.abc.Iterable) -> None:
        """
        Extend the list by appending all the items in the given list
        """
    def insert(self, i: typing.SupportsInt | typing.SupportsIndex, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Insert an item at a given position.
        """
    @typing.overload
    def pop(self) -> int:
        """
        Remove and return the last item
        """
    @typing.overload
    def pop(self, i: typing.SupportsInt | typing.SupportsIndex) -> int:
        """
        Remove and return the item at index ``i``
        """
    def remove(self, x: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
class Color4f:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Color4f) -> bool:
        ...
    def __init__(self) -> None:
        ...
    @property
    def a(self) -> float:
        ...
    @a.setter
    def a(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def b(self) -> float:
        ...
    @b.setter
    def b(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def g(self) -> float:
        ...
    @g.setter
    def g(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def r(self) -> float:
        ...
    @r.setter
    def r(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class Curve:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Curve) -> bool:
        ...
    def __init__(self) -> None:
        ...
    @property
    def a(self) -> int:
        ...
    @a.setter
    def a(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def b(self) -> int:
        ...
    @b.setter
    def b(self, arg0: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    @property
    def floats(self) -> typing.Annotated[list[float], "FixedSize(30)"]:
        ...
    @floats.setter
    def floats(self, arg0: typing.Annotated[collections.abc.Sequence[typing.SupportsFloat | typing.SupportsIndex], "FixedSize(30)"]) -> None:
        ...
class Endianness:
    """
    Members:
    
      Big
    
      Little
    """
    Big: typing.ClassVar[Endianness]  # value = <Endianness.Big: 0>
    Little: typing.ClassVar[Endianness]  # value = <Endianness.Little: 1>
    __members__: typing.ClassVar[dict[str, Endianness]]  # value = {'Big': <Endianness.Big: 0>, 'Little': <Endianness.Little: 1>}
    @typing.overload
    def __eq__(self, other: Endianness) -> bool:
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
    def __ne__(self, other: Endianness) -> bool:
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
class F32:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> F32:
        ...
    def __add__(self, arg0: F32) -> float:
        ...
    def __eq__(self, arg0: F32) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: F32) -> bool:
        ...
    def __gt__(self, arg0: F32) -> bool:
        ...
    def __init__(self, value: float = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __le__(self, arg0: F32) -> bool:
        ...
    def __lt__(self, arg0: F32) -> bool:
        ...
    def __mul__(self, arg0: F32) -> float:
        ...
    def __neg__(self) -> float:
        ...
    def __pos__(self) -> float:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: F32) -> float:
        ...
    def __truediv__(self, arg0: F32) -> float:
        ...
    @property
    def v(self) -> float:
        """
        Value
        """
    @v.setter
    def v(self, arg1: float) -> None:
        ...
class F64:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> F64:
        ...
    def __add__(self, arg0: F64) -> float:
        ...
    def __eq__(self, arg0: F64) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: F64) -> bool:
        ...
    def __gt__(self, arg0: F64) -> bool:
        ...
    def __init__(self, value: float = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __le__(self, arg0: F64) -> bool:
        ...
    def __lt__(self, arg0: F64) -> bool:
        ...
    def __mul__(self, arg0: F64) -> float:
        ...
    def __neg__(self) -> float:
        ...
    def __pos__(self) -> float:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: F64) -> float:
        ...
    def __truediv__(self, arg0: F64) -> float:
        ...
    @property
    def v(self) -> float:
        """
        Value
        """
    @v.setter
    def v(self, arg1: float) -> None:
        ...
class File:
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
    @property
    def data(self) -> memoryview:
        ...
    @property
    def name(self) -> str:
        ...
class FixedSafeString128:
    __hash__: typing.ClassVar[None] = None
    @typing.overload
    def __eq__(self, arg0: FixedSafeString128) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: str) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
class FixedSafeString16:
    __hash__: typing.ClassVar[None] = None
    @typing.overload
    def __eq__(self, arg0: FixedSafeString16) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: str) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
class FixedSafeString256:
    __hash__: typing.ClassVar[None] = None
    @typing.overload
    def __eq__(self, arg0: FixedSafeString256) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: str) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
class FixedSafeString32:
    __hash__: typing.ClassVar[None] = None
    @typing.overload
    def __eq__(self, arg0: FixedSafeString32) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: str) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
class FixedSafeString48:
    __hash__: typing.ClassVar[None] = None
    @typing.overload
    def __eq__(self, arg0: FixedSafeString48) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: str) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
class FixedSafeString64:
    __hash__: typing.ClassVar[None] = None
    @typing.overload
    def __eq__(self, arg0: FixedSafeString64) -> bool:
        ...
    @typing.overload
    def __eq__(self, arg0: str) -> bool:
        ...
    @typing.overload
    def __init__(self) -> None:
        ...
    @typing.overload
    def __init__(self, arg0: str) -> None:
        ...
    def __repr__(self) -> str:
        ...
    def __str__(self) -> str:
        ...
class InvalidDataError(Exception):
    pass
class Quatf:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Quatf) -> bool:
        ...
    def __init__(self) -> None:
        ...
    @property
    def a(self) -> float:
        ...
    @a.setter
    def a(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def b(self) -> float:
        ...
    @b.setter
    def b(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def c(self) -> float:
        ...
    @c.setter
    def c(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def d(self) -> float:
        ...
    @d.setter
    def d(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class S16:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> S16:
        ...
    def __add__(self, arg0: S16) -> int:
        ...
    def __and__(self, arg0: S16) -> int:
        ...
    def __eq__(self, arg0: S16) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: S16) -> bool:
        ...
    def __gt__(self, arg0: S16) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: S16) -> bool:
        ...
    def __lshift__(self, arg0: S16) -> int:
        ...
    def __lt__(self, arg0: S16) -> bool:
        ...
    def __mod__(self, arg0: S16) -> int:
        ...
    def __mul__(self, arg0: S16) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: S16) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: S16) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: S16) -> int:
        ...
    def __truediv__(self, arg0: S16) -> int:
        ...
    def __xor__(self, arg0: S16) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class S32:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> S32:
        ...
    def __add__(self, arg0: S32) -> int:
        ...
    def __and__(self, arg0: S32) -> int:
        ...
    def __eq__(self, arg0: S32) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: S32) -> bool:
        ...
    def __gt__(self, arg0: S32) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: S32) -> bool:
        ...
    def __lshift__(self, arg0: S32) -> int:
        ...
    def __lt__(self, arg0: S32) -> bool:
        ...
    def __mod__(self, arg0: S32) -> int:
        ...
    def __mul__(self, arg0: S32) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: S32) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: S32) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: S32) -> int:
        ...
    def __truediv__(self, arg0: S32) -> int:
        ...
    def __xor__(self, arg0: S32) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class S64:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> S64:
        ...
    def __add__(self, arg0: S64) -> int:
        ...
    def __and__(self, arg0: S64) -> int:
        ...
    def __eq__(self, arg0: S64) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: S64) -> bool:
        ...
    def __gt__(self, arg0: S64) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: S64) -> bool:
        ...
    def __lshift__(self, arg0: S64) -> int:
        ...
    def __lt__(self, arg0: S64) -> bool:
        ...
    def __mod__(self, arg0: S64) -> int:
        ...
    def __mul__(self, arg0: S64) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: S64) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: S64) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: S64) -> int:
        ...
    def __truediv__(self, arg0: S64) -> int:
        ...
    def __xor__(self, arg0: S64) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class S8:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> S8:
        ...
    def __add__(self, arg0: S8) -> int:
        ...
    def __and__(self, arg0: S8) -> int:
        ...
    def __eq__(self, arg0: S8) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: S8) -> bool:
        ...
    def __gt__(self, arg0: S8) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: S8) -> bool:
        ...
    def __lshift__(self, arg0: S8) -> int:
        ...
    def __lt__(self, arg0: S8) -> bool:
        ...
    def __mod__(self, arg0: S8) -> int:
        ...
    def __mul__(self, arg0: S8) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: S8) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: S8) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: S8) -> int:
        ...
    def __truediv__(self, arg0: S8) -> int:
        ...
    def __xor__(self, arg0: S8) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class Sarc:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Sarc) -> bool:
        ...
    def __init__(self, data: typing_extensions.Buffer) -> None:
        ...
    def are_files_equal(self, arg0: Sarc) -> bool:
        ...
    def get_data_offset(self) -> int:
        ...
    def get_endianness(self) -> Endianness:
        ...
    @typing.overload
    def get_file(self, name: str) -> File | None:
        ...
    @typing.overload
    def get_file(self, index: typing.SupportsInt | typing.SupportsIndex) -> File:
        ...
    def get_files(self) -> collections.abc.Iterator[File]:
        ...
    def get_num_files(self) -> int:
        ...
    def guess_min_alignment(self) -> int:
        ...
class SarcWriter:
    class FileMap:
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
        def __eq__(self, arg0: SarcWriter.FileMap | dict) -> bool:
            ...
        def __getitem__(self, arg0: str) -> Bytes:
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
        def __setitem__(self, arg0: str, arg1: Bytes | bytes) -> None:
            ...
        def clear(self) -> None:
            ...
        @typing.overload
        def get(self, key: str) -> Bytes | None:
            ...
        @typing.overload
        def get(self, key: str, default: _T) -> Bytes | _T:
            ...
        def items(self) -> typing.ItemsView[str, Bytes]:
            ...
        def keys(self) -> typing.KeysView[str]:
            ...
        def values(self) -> typing.ValuesView[Bytes]:
            ...
    class Mode:
        """
        Members:
        
          Legacy
        
          New
        """
        Legacy: typing.ClassVar[SarcWriter.Mode]  # value = <Mode.Legacy: 0>
        New: typing.ClassVar[SarcWriter.Mode]  # value = <Mode.New: 1>
        __members__: typing.ClassVar[dict[str, SarcWriter.Mode]]  # value = {'Legacy': <Mode.Legacy: 0>, 'New': <Mode.New: 1>}
        @typing.overload
        def __eq__(self, other: SarcWriter.Mode) -> bool:
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
        def __ne__(self, other: SarcWriter.Mode) -> bool:
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
    @property
    def files(self) -> SarcWriter.FileMap:
        ...
    @files.setter
    def files(self, value: SarcWriter.FileMap | dict) -> None:
        ...
    @staticmethod
    def from_sarc(archive: Sarc) -> SarcWriter:
        ...
    def __init__(self, endian: Endianness = ..., mode: SarcWriter.Mode = ...) -> None:
        ...
    def add_alignment_requirement(self, extension_without_dot: str, alignment: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def set_endianness(self, endian: Endianness) -> None:
        ...
    def set_min_alignment(self, alignment: typing.SupportsInt | typing.SupportsIndex) -> None:
        ...
    def set_mode(self, mode: SarcWriter.Mode) -> None:
        ...
    def write(self) -> tuple[int, Bytes]:
        ...
class TypeError(Exception):
    pass
class U16:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> U16:
        ...
    def __add__(self, arg0: U16) -> int:
        ...
    def __and__(self, arg0: U16) -> int:
        ...
    def __eq__(self, arg0: U16) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: U16) -> bool:
        ...
    def __gt__(self, arg0: U16) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: U16) -> bool:
        ...
    def __lshift__(self, arg0: U16) -> int:
        ...
    def __lt__(self, arg0: U16) -> bool:
        ...
    def __mod__(self, arg0: U16) -> int:
        ...
    def __mul__(self, arg0: U16) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: U16) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: U16) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: U16) -> int:
        ...
    def __truediv__(self, arg0: U16) -> int:
        ...
    def __xor__(self, arg0: U16) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class U32:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> U32:
        ...
    def __add__(self, arg0: U32) -> int:
        ...
    def __and__(self, arg0: U32) -> int:
        ...
    def __eq__(self, arg0: U32) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: U32) -> bool:
        ...
    def __gt__(self, arg0: U32) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: U32) -> bool:
        ...
    def __lshift__(self, arg0: U32) -> int:
        ...
    def __lt__(self, arg0: U32) -> bool:
        ...
    def __mod__(self, arg0: U32) -> int:
        ...
    def __mul__(self, arg0: U32) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: U32) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: U32) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: U32) -> int:
        ...
    def __truediv__(self, arg0: U32) -> int:
        ...
    def __xor__(self, arg0: U32) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class U64:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> U64:
        ...
    def __add__(self, arg0: U64) -> int:
        ...
    def __and__(self, arg0: U64) -> int:
        ...
    def __eq__(self, arg0: U64) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: U64) -> bool:
        ...
    def __gt__(self, arg0: U64) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: U64) -> bool:
        ...
    def __lshift__(self, arg0: U64) -> int:
        ...
    def __lt__(self, arg0: U64) -> bool:
        ...
    def __mod__(self, arg0: U64) -> int:
        ...
    def __mul__(self, arg0: U64) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: U64) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: U64) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: U64) -> int:
        ...
    def __truediv__(self, arg0: U64) -> int:
        ...
    def __xor__(self, arg0: U64) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class U8:
    __hash__: typing.ClassVar[None] = None
    def __abs__(self) -> U8:
        ...
    def __add__(self, arg0: U8) -> int:
        ...
    def __and__(self, arg0: U8) -> int:
        ...
    def __eq__(self, arg0: U8) -> bool:
        ...
    def __float__(self) -> float:
        ...
    def __ge__(self, arg0: U8) -> bool:
        ...
    def __gt__(self, arg0: U8) -> bool:
        ...
    def __index__(self) -> int:
        ...
    def __init__(self, value: int = 0) -> None:
        ...
    def __int__(self) -> int:
        ...
    def __invert__(self) -> int:
        ...
    def __le__(self, arg0: U8) -> bool:
        ...
    def __lshift__(self, arg0: U8) -> int:
        ...
    def __lt__(self, arg0: U8) -> bool:
        ...
    def __mod__(self, arg0: U8) -> int:
        ...
    def __mul__(self, arg0: U8) -> int:
        ...
    def __neg__(self) -> int:
        ...
    def __or__(self, arg0: U8) -> int:
        ...
    def __pos__(self) -> int:
        ...
    def __repr__(self) -> str:
        ...
    def __rshift__(self, arg0: U8) -> int:
        ...
    def __str__(self) -> str:
        ...
    def __sub__(self, arg0: U8) -> int:
        ...
    def __truediv__(self, arg0: U8) -> int:
        ...
    def __xor__(self, arg0: U8) -> int:
        ...
    @property
    def v(self) -> int:
        """
        Value
        """
    @v.setter
    def v(self, arg1: int) -> None:
        ...
class Vector2f:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Vector2f) -> bool:
        ...
    def __init__(self) -> None:
        ...
    @property
    def x(self) -> float:
        ...
    @x.setter
    def x(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def y(self) -> float:
        ...
    @y.setter
    def y(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class Vector3f:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Vector3f) -> bool:
        ...
    def __init__(self) -> None:
        ...
    @property
    def x(self) -> float:
        ...
    @x.setter
    def x(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def y(self) -> float:
        ...
    @y.setter
    def y(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def z(self) -> float:
        ...
    @z.setter
    def z(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class Vector4f:
    __hash__: typing.ClassVar[None] = None
    def __eq__(self, arg0: Vector4f) -> bool:
        ...
    def __init__(self) -> None:
        ...
    @property
    def t(self) -> float:
        ...
    @t.setter
    def t(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def x(self) -> float:
        ...
    @x.setter
    def x(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def y(self) -> float:
        ...
    @y.setter
    def y(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def z(self) -> float:
        ...
    @z.setter
    def z(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class _SpanView:
    def __buffer__(self, flags: int, /) -> memoryview:
        """
        Return a buffer object that exposes the underlying memory of the object.
        """
    def __release_buffer__(self, buffer: memoryview, /) -> None:
        """
        Release the buffer object that exposes the underlying memory of the object.
        """
