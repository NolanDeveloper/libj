# libj 

This is libj - JSON library for written in C11. It provides "from string" and
"to string" coversions as well as functions that allow to access and modify
JSON.

## Goals

### RFC8259 compliant

Code correctness is of top importance.

### UTF-8 all the way

C has a concept of execution character set which is implementation-defined
character set interpreted by execution environment. It may not be able to
represent the whole set of Unicode characters. So it's impossible to use it
internally in JSON handling without sacrificing RFC8259 compliance. 

The library stores strings in UTF-8 and all string related functions expect as
input and produce as output valid UTF-8 sequences.

### Handle '\u0000' 

Although '\0' cannot be part of JSON text, according to Section 7 RFC8259 it
may be encoded as an escape '\u0000'. So in order to comply with RFC8259 the
parser has to accept it.

This becomes a challenge because C-strings use '\0' as the terminator. Despite
being considered a bad design choice, C-strings are still inherent to the
language and are widely used. So many of the users of the library would want to
work with C-string.

Thus JSON library implemented in C has to make a compromise.

Solution that this library employs:

* Internally store size-limited (as opposed to null-terminated) strings and
  don't treat '\0' as the terminator.
* Provide **convenient** API for the users that work with C-strings and return
  a special error code whenever the input or output string contains '\0'.
* Provide **extended** API for the users that work with size-limited strings to
  support input and output of strings containing '\0'.

### Support arbitrary precision JSON numbers

RFC8259 doesn't set a limit on the length and precision of JSON numbers. But
libc doesn't support arbitrary precision integers and real numbers. Linking
specific arbitrary precision library may limit usage of this library.

Solution that this library employs:

* Provide **convenient** API to handle fixed size integers and real
  numbers.
* Provide **extended** API that treats numbers as strings for users that are
  able to handle arbitrary precision numbers.

### Preserve order and support duplicate keys in the object

RFC8259 doesn't specify whether order of the keys has to be preserved and how
to handle duplicate keys. Implementations that preserve order and support
duplicate keys achive better interoperability. So this library both preserves
order and supports duplicate keys.

## Non-goals

### High performance

This project values elegance of interface and elegance of implementation higher
than performance. The user should benchmark to decide whether performance of
this library fits their needs.

