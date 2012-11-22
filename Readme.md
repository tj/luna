
  ![Luna programming language](http://f.cl.ly/items/3K2A3k1k3p2s0M3B3j1F/logo.png)

 Luna is an expressive, minimalistic, elegant programming language implemented in C. With cooperative thread concurrency at its core, async I/O, and influences derived from languages such as Lua, io, Rust, Ruby, and C. Luna favours unification and minimalism over minor obscure conveniences, providing the true convenience of a simple effective language. This includes omitting features which facilitate magic such as getters/setters, method_missing etc. This project is _very_ much a work in progress, as I explore the wonderful world of VMs! feel free to join.

## Goals

  - fast!
  - small (~2000 SLOC)
  - simple, elegant, explicit
  - static + inferred typing
  - high concurrency out of the box
  - small embeddable / hackable core
  - ...

## Build

 To build Luna, simply run:
 
    $ make

 Check out the help:
 
    $ ./luna --help

## Status

  I _just_ started Luna, so don't expect much yet! Until the project reaches `0.0.1` there will be no tickets, just the simple todo list below:

  - ✔ lexer
  - ✔ parser
  - ✔ test suite
  - ✔ ast
  - ✔ linenoise integration for REPL
  - ◦ register machine
  - ◦ C public/internal apis
  - ◦ garbage collection
  - ◦ continuations
  - ◦ optimizations (TCO etc)
  - ◦ portability
  - ◦ closures
  - ◦ VIM / TM / syntaxes
  - ◦ website
  - ◦ context threading

## Operator precedence

 Operator precedence from highest to lowest, note "sp" is used
 for slot access in the table below, however this is simply a space (' ')
 in Luna.

```
operator                |  associativity
------------------------|---------------
[ ] ( ) sp              |  left
++ --                   |  right
! ~ + -                 |  right
* / %                   |  left
+ -                     |  left
<< >>                   |  left
< <= > >=               |  left
== !=                   |  left
&                       |  left
^                       |  left
|                       |  left
&&                      |  left
||                      |  left
?:                      |  right
= += -= /= *= ||= &&=   |  right
not                     |  right
,                       |  left
```

  TODO: fill me out...

## License 

  MIT

