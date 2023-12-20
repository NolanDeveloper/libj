# libj 

This is libj -- JSON library. It provides "from string" and "to string"
coversions as well as functions that allow to access and modify JSON.

## Goals

### RFC8259 compliant

Code correctness is of top importance.

### Make "wrong" usage inconvenient

Convenience is a tool that API employs to guide its users to what it considers
"right". 

~~This doesn't mean however that "right" usage would be convenient.~~

What's considered "right" by this library:
* Errors must be handled
* Users should use C-strings
* ... 
* *The list is being updated*

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

This becomes a challenge because C-strings use '\0' as terminator character.
Despite being considered a bad design choice, C-strings are still inherent to
the language and are widely used. So many of the users of the library would
certainly want to work with C-string.

Thus JSON library implemented in C has to make a compromise.

Solution that this library eploys:

* Internally store size-limited (as opposed to null-terminated) strings and
  don't treat '\0' as terminator character;
* Provide **convenient** API for users that work with C-strings and return
  special error code whenever input or output string contains '\0';
* Provide **some** API for users that work with size-limited strings to support
  input and output of strings containing '\0';

### Support arbitrary precision JSON numbers

### Support access to multiple duplicate keys

Non-goals:

> High performance is not one of the project's goals.

Main features:
* C11 standard



