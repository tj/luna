
# Luna

 The Luna programming language.

## About

 Luna is an expressive, minimalistic, elegant programming language with reactor based concurrency at the core, or at least it will be :). This project is _very_ much a work in progress, as I explore the wonderful world of VMs! feel free to join me!

## Goals

  - simple, small, elegant, explicit
  - prototypal inheritance
  - reflection capabilities
  - reactor concurrency model (event loop)
  - callee evaluated messages
  - stack based VM
  - ...

## Build

 To build Luna, simply run:
 
    $ make

 When successful you may then execute the binary:
 
    $ ./luna --help

## Status

  I _just_ started Luna, so don't expect much yet! first up is lexical analysis; currently Luna will just spit out debugging information, nothing fancy.

## Operator Precedence Table

 Operator precedence from highest to lowest.

    operator       |  associativity
    ---------------|---------------
    []             |  left
    ++ --          |  right
    ! ~ + -        |  right
    * / %          |  left
    + -            |  left
    << >>          |  left
    < <= > >=      |  left
    == !=          |  left
    &              |  left
    ^              |  left
    |              |  left
    &&             |  left
    ||             |  left
    ?:             |  right
    =              |  right
    not            |  right
    ,              |  left

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