# PGHacks
C library implementing Postgres features at a higher level (logical_decoding, pg_dump...)

## What
PGHacks goal is to provide tools over libpq to take advantage of Postgresql
unique features in a portable way. It's mostly done to build you own tools
on top and to write language bindings (see https://github.com/lisael/pylogicaldecoding ).

Current features:
  - logical decoding
  - logical decoding test plugin output parser
  
More to come:
  - connection routines.
    - currently implemented in logicaldecoding module, just have to extract and make it generic 
  - pg_dump: it's quite a huge work, but it would be über-cool to extract db schema in a generic way, usable by any language.
  - query parser
  
Sky is the limit:
  - parts of psql, like completion
  - pgctl ?? mostly os calls
  - ... INSERT YOUR IDEA HERE

## Why
Postgresql comes with great tools like `pg_dump` or good code examples
like `pg_recvlogical.c` but the internal structures and functions of these tools are
hard to reuse in client code. When I started PyLogicaldecoding, I had to
compile it against PG source tree. I can't tell the user to do this, so I
extracted what I needed into PyLogicaldecoding. I then forked Pylogicaldecoding
to extract this lib, and make it usable in C or any serious language (i.e. one with C bindings)

## Who
[Me](https://github.com/lisael)... and I'd gladly review any PR or even share the repo :)
