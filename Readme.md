
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

  Generalized status:

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

## Typing

  Luna is statically typed, however mostly optional thanks to type inference. Some declarations such as function parameters _must_ specify a type: 

```ruby
def greet(name:str) {
  ret "Hello " + name
}
```

  Types that can be properly inferred may be, for example function return types as seen above. When fully-qualified the same function would look like this:

```ruby
def greet(name:str): str {
  ret "Hello " + name
}
```

## Multiple dispatch

  Luna plans to provide multiple dispatch support. This will drastically cut down on verbosity and fragmentation. For example suppose you have a `vec_sum(v)` function, in Luna you would simply create a `sum(v)` function:

```ruby
def sum(v:vec): int {
  ...
}
```

  Thanks to the typing system Luna can choose the correct function to invoke for the given parameters. This technique addresses another fundamental problem of many languages, fragmentation and delocalization. For example it is often tempting to extend native prototypes or classes provided by the host language, such as `Array#sum()`. Because no such construct exists in Luna you're free to "extend" these types elsewhere simply by defining functions that act on those types, without polluting "global" classes or objects, removing a layer of indirection, as it's often not clear where these additions came from, and they regularly conflict.

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

