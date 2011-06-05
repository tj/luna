
# Luna

 The Luna programming language.

## About

 Luna is an expressive, minimalistic, elegant programming language implemented in ANSI C. With reactor based concurrency at it's core, async I/O, and influences derived from phenomenal languages such as IO and Lua. This project is _very_ much a work in progress, as I explore the wonderful world of VMs! feel free to join.

## Goals

  - simple, small, elegant, explicit
  - fast, fast, fast and fast
  - prototypal inheritance
  - reflection capabilities
  - reactor concurrency model (event loop)
  - callee evaluated messages
  - register based VM
  - ...

## Build

 To build Luna, simply run:
 
    $ make

 When successful you may then execute the binary:
 
    $ ./luna --help

## Status

  I _just_ started Luna, so don't expect much yet! Until the project reaches `0.0.1` there will be no tickets, just the simple todo list below:

  - ✔ lexer
  - ✔ parser
  - ◦ virtual machine
  - ◦ opcode generation
  - ◦ C public/internal apis
  - ◦ garbage collection
  - ◦ optimizations (TCO etc)
  - ◦ linenoise integration for REPL
  - ◦ libev integration
  - ◦ portability
  - ◦ closures
  - ◦ module system
  - ◦ test suite
  - ◦ VIM / TM / syntaxes
  - ◦ website

## Operator Precedence Table

 Operator precedence from highest to lowest, note "sp" is used
 for slot access in the table below, however this is simply a space (' ')
 in Luna.

    operator       |  associativity
    ---------------|---------------
    [] () sp       |  left
    ++ --          |  right
    ! ~ + -        |  right
    * / %          |  left
    .              |  left
    + -            |  left
    << >>          |  left
    < <= > >=      |  left
    == !=          |  left
    &              |  left
    ^              |  left
    |              |  left
    &&             |  left
    \|\|           |  left
    ?:             |  right
    = :=           |  right
    not            |  right
    ,              |  left

## Dependencies

  All Luna dependencies are bundled within this repository, and includes Marc Lehmann and Emanuele Giaquinta's [libev](http://software.schmorp.de/pkg/libev.html) and [libeio](http://software.schmorp.de/pkg/libeio.html), and Ryan Dahl's high performance [HTTP parser](https://github.com/ry/http-parser).

## Inspiration

 Luna's inspiration comes from bits of many languages that I have worked work as I aim for the minimalism and performance of [Lua](http://www.lua.org/), elegance of Steve Dekorte's [io](http://www.iolanguage.com/), and bits of syntax inspired by Python (whitespace) and Ruby (low precedence `not`, `**` etc). Luna's reactor is of course, inspired by the fantastic work of Ryan Dahl, and the entire community of [nodejs](http://nodejs.org).

## Syntax

 The syntax, though very much still a work-in-progress, consists of concepts from various languages, as well as some I've not personally seen.

### Significant Whitespace

 Those of you who know me, might think "but TJ, you hate significant white-space?", well I don't; the primary issue I have with significant white-space is it's abuse. It's very easy to get lost when the indentation level is deep, or when methods (or classes etc) span many rows. For example, class-based languages with significant whitespace make it very easy to loose context, and pairing outdents is more of a visual challenge than braces or other block delimiters.

 In contrast, when used appropriately it can lead to syntactically pleasing code. For this reason I have chosen to adopt significant whitespace for Luna, _however_ since Luna's inheritance is prototype-based, excessive nesting is at a minimum, because the receiving object (or class in class-based languages), must be explicitly defined, reaffirming context, as shown in the example below:
 
     Person = Object clone

     Person init =: first, last
       self first = first
       self last = last

     Person toString =:
       self first . ' ' . self last

     Person inspect =:
       '#<Person' . self toString() . '>'

     tj = Person init('tj', 'holowaychuk')
     stdout write(tj)
     // => ''

## License 

(The MIT License)

Copyright (c) 2011 TJ Holowaychuk &lt;tj@vision-media.ca&gt;

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
