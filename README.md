Cavatina - a minimalistic parser for config files
=================================================

This library provides utilities to parse simple config files, as used by many
systems worldwide.

Requirements
------------

Just C.

Installation
------------

cavatina provices a simple Makefile; to build from scratch:

    make all
    make install

Usage
-----

The cavatina.h file is fully documented, and gives a relatively good idea of the
API; the bullet points are:

The kv_parser struct holds the data from parsing one or more strings. Strings
themselves are not allocated inside the parser, that instead holds pointers to
the string from which it was parsed, so parsed strings must live as long as the
parser itself.

The structure is initialized with the kv_init function and destroyed with
kv_destroy. To parse a string and add all groups, keys, and values to the
parser, use the kv_parse function.

In addition to parsing, you can add new groups, keys, and values to the parsed
data, and dump it again to a new string.
