
# Luna

 The Luna programming language.

## About

 Luna is an expressive, minimalistic, elegant programming language implemented in ANSI C. With reactor based concurrency at it's core, async I/O, and influences derived from languages such as Lua, io, Python, and Ruby. This project is _very_ much a work in progress, as I explore the wonderful world of VMs! feel free to join.

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
  - ◦ PCRE
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

__NOTE__: forgive the bad syntax highlighting, until I have time to write a lexer for [pygments](http://pygments.org/) (what GitHub uses for highlighting). 

 Those of you who know me, might think _"but TJ, you hate significant white-space?"_, well that's not quite accurate; the primary issue I have with significant white-space is it's abuse. It's very easy to get lost when the indentation level is deep, or when methods (or classes etc) span many rows. For example, class-based languages with significant whitespace make it very easy to loose context, and pairing outdents is more of a visual challenge than braces or other block delimiters.

 In contrast, when used appropriately it can lead to syntactically pleasing code. For this reason I have chosen to adopt significant whitespace for Luna, _however_ since Luna's inheritance is prototype-based, excessive nesting is at a minimum, because the receiving object (or class in class-based languages), must be explicitly defined, reaffirming context, as shown in the example below:

```js 
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
// => 'tj holowaychuk'
```

### Slot Access

 In the example above, you will notice that the use of whitespace is used to convey member (slot) access, where typically the `.` character is used. I find this pleasing to the eye, while maintaining explicit function calls. Typically languages such as Ruby, or CoffeeScript allow optional parenthesis for calls, creating ambiguity with property access:

```ruby 
person.pets.push 'tobi'
person.pets.push 'loki'
person.pets.push 'jane'
person.pets.pop
```

or in the case of CoffeeScript due to ambiguity issues, they are required when no arguments are given:

```js
person.pets.push 'tobi'
person.pets.push 'loki'
person.pets.push 'jane'
person.pets.pop()
```

Luna function calls _always_ (almost, one exception) require parenthesis:

```js
person pets push('tobi')
person pets push('loki')
person pets push('jane')
person pets pop()
```

While the former approach is fine in small use-cases, and of course when it's _your_ code, it becomes "word soup" in larger doses.

### Operators Avoidance 

 Another aspect I want to avoid, which I consider an annoyance in JavaScript, is using operators such as `typeof`, or `instanceof`, when a simple method or property will do, I would much rather use language features such as functions unless performance is crucial.

```js
User = Object clone

tj = User clone

tj proto == User
// => true

tj proto proto == Object
// => true
```

 I have not yet decided on names etc, however these can all easily be implemented in the language itself, avoiding additional keywords. One might ask, _"well why not implement most operators as methods?"_, the answer to that would be, _performance_. Many languages provide type coercion in operations, for the following is legal JavaScript:

```js
var tj = { valueOf: function(){ return 23; }};
5 + tj;
// => 28
```

or:

```js
5 + '5'
// => '55'
```

 this feature adds overhead, and in my opinion is rarely overly useful. In Luna arithmetic operators are strictly for arithmetic operations, _not_ concatenation etc, allowing Luna to optimize expressions such as `5 + 10` into a single instruction.

### Concatenation

  Concatenation is performed with the `.` operator:

```js 
'foo ' . ' bar'
``` 

### Function Literals

 Luna has first-class functions, much like JavaScript, Lua etc. The syntax of a function literal in Luna is as follows (_currently_):

```js
':' params? block
```

where `params` is:

```js
(id (',' id)*)
```

and `block` is:

     INDENT ws (stmt ws)+ OUTDENT

so, for comparison the following JavaScript:

```js
 var greet = function(user) {
   console.log('hello ' + user)
 }
```

would be defined in Luna as:

```js
greet =: user
  stdout write('hello '.user.'\n')
```

Let's look at some more examples. The following Ruby selects `person`'s ferrets, older than `4`:

```ruby
person.pets.select do |pet|
  pet.species == 'ferret' and pet.age > 4
end
```

and the following CoffeeScript:

```js
person.pets.filter (pet) ->
  pet.species == 'ferret' and pet.age > 4
```

The equivalent canonical Luna would look like this:

```js
person pets select(: pet
  pet species == 'ferret' && pet age > 4
)
```

Slightly ugly right? the parenthesis requirement gets in the way here, however nearly _every_ case of passing a function as an argument is for a _single_ callback, though there are a few exceptions in libraries such as jQuery where two functions are passed for toggle states etc. For this reason Luna has some syntax sugar allowing a tail function argument to be passed trailing the closing paren `)`:

```js
person pets select(): pet
  pet species == 'ferret' && pet age > 4
```

On top of this, in cases where no other arguments are passed, we can omit the parens all together:

```js
person pets select: pet
  pet species == 'ferret' && pet age > 4
```

This approach is syntactically similar to Ruby "blocks", though simply sugar on top of regular old first-class Luna functions.

To make things even sexier, Luna will allow for callee-evaluated expressions, that is, expressions or "messages" are conditionally evaluated by the function called. For example the following statement means exactly how it reads, select users with `age` above 20, this is _not_ passing the result of `age > 20` to `select()`, we are messaging `select()` which then may choose to evaluate the message against each user, essentially expanding to `user age > 20`:

```js
users select(age > 20)
```

### Number Literals

 Luna currently provides literals for base 16 (hexadecimal):

```js 
0xff
```

 as well as base 10 (decimal), which (_optionally_) may contain underscore delimiters, inspired by Ruby:

```rb
million = 1_000_000.00
```

### Case Statement

 I have not decided yet, but Luna _may_ have two switch-like statements. The syntax is undecided as well. One will be used for arbitrary values such as strings as shown below in an argument parser using `with` (not sold on the name), while the other would be a regular `switch`, restricted to numbers for optimization purposes, to be used in high traffic state machines (multipart parsers etc). 

```js
files = []
args = process argv slice(2)

while args length
  with flag
    '-h', '--help', 'help':
      stdout puts(usage)
      process exit(0)
    '-v', '--version':
      stdout puts(version)
      process exit(0)
```

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
