
  ![Luna programming language](http://f.cl.ly/items/3K2A3k1k3p2s0M3B3j1F/logo.png)

 Luna is an expressive, minimalistic, elegant programming language implemented in C. With cooperative thread concurrency at its core, async I/O, and influences derived from languages such as Lua, io, Python, and Ruby. Luna favours unification and minimalism over minor obscure conveniences, providing the true convenience of a simple effective language. This includes omitting features which facilitate magic such as getters/setters, method_missing etc. This project is _very_ much a work in progress, as I explore the wonderful world of VMs! feel free to join.

## Goals

  - fast!
  - small (~2000 SLOC)
  - simple, elegant, explicit
  - differential inheritance (proto chain)
  - favour simplicity in place of minor syntactic conveniences
  - reflection capabilities
  - register based VM (then JIT)
  - small embeddable core
  - robust & rich standard library
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
  - ◦ code generation
  - ◦ virtual machine
  - ◦ C public/internal apis
  - ◦ garbage collection
  - ◦ continuations
  - ◦ optimizations (TCO etc)
  - ◦ portability
  - ◦ closures
  - ◦ VIM / TM / syntaxes
  - ◦ website

## Operator Precedence Table

 Operator precedence from highest to lowest, note "sp" is used
 for slot access in the table below, however this is simply a space (' ')
 in Luna.

```
operator       |  associativity
---------------|---------------
[ ] ( ) sp     |  left
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
= += -=        |  right
not            |  right
,              |  left
if unless      |  left
while until
```

## Dependencies

  All Luna dependencies are bundled within this repository, and follow the same general goals as Luna, light-weight, performant, and elegant:
  
  - linenoise (tiny repl used by redis)
  - khash
  - kvec

## Syntax

  Luna's syntactic & general philosophy is to encourage simple, explicit programs. For example explicit __return__s rather than implicit, providing (usually) one reasonable way to perform a task rather than several, providing a single method of concurrency rather than many, and the list goes on. The syntax, though very much still a work-in-progress, consists of concepts from various languages, as well as some I've not personally seen.

### Significant Whitespace

__NOTE__: forgive the bad syntax highlighting, until I have time to write a lexer for [pygments](http://pygments.org/) (what GitHub uses for highlighting). 

 Those of you who know me, might think _"but TJ, you hate significant white-space?"_, well that's not quite accurate; the primary issue I have with significant white-space is its abuse. It's very easy to get lost when the indentation level is deep, or when methods (or classes etc) span many rows. For example, class-based languages with significant whitespace make it very easy to lose context, and pairing outdents is more of a visual challenge than braces or other block delimiters.

 In contrast, when used appropriately it can lead to syntactically pleasing code. For this reason I have chosen to adopt significant whitespace for Luna, _however_ since Luna's inheritance is prototype-based, excessive nesting is at a minimum, because the receiving object must be explicitly defined, reaffirming context, as shown in the example below:

```js 
User =: first, last
  user = {}
  user mixin(User)
  user first = first
  user last = last
  ret user

User string =:
  ret self first + ' ' + self last

User inspect =:
  ret '#<User' + self + '>'

tj = User('tj', 'holowaychuk')

stdout write(tj)
// => 'tj holowaychuk'
```

### Operators

  Personally I'm not a fan of "wordy" operators, aka `or and` for `|| &&`, etc, though again they can look ok within reason, but I find the "machine-like" look of operators provide a nice visual separation. When they are chained in large expressions they get a little lost amongst identifiers. The one exception in Luna is `not`, which is a low-precedence negation. For example the following:

     User allowed =: realm
       not banned || blocked_from(realm)

Evaluates as:

     User allowed =: realm
       !(banned || blocked_from(realm))

### "self"

 The `self` argument will be much like the one you find in JavaScript, that is, it will be "unbound", this means `foo bar()`, and `x = foo bar; x()` are not the same.Luna functions called without receivers will use the `self` value of the caller. Consider the following example, where `self visit(node)`, this would reference the `Block` instance, even though it's nested within another function.

``` 
Block visit =:
  self nodes each: node
    self visit(node)
```

 The variable `self` is also aliased as `@`, which is expanded in the lexer.

```
Block visit =:
  @nodes each: node
    @visit(node)
```

### Slot Access

 In the example above, you will notice that the use of whitespace is used to convey member (slot) access, where typically the `.` character is used. I find this pleasing to the eye, while maintaining explicit function calls. Some languages such as Ruby allow optional parenthesis for calls, creating ambiguity with property access:

```ruby 
person.pets.push 'tobi'
person.pets.push 'loki'
person.pets.push 'jane'
person.pets.pop
```

Luna function calls _always_ (one exception :) ) require parenthesis:

```js
person pets push('tobi')
person pets push('loki')
person pets push('jane')
person pets pop()
```

While the former approach is fine in small use-cases, and of course when it's _your_ code, it becomes "word soup" in larger doses.

### Avoiding Operators

 Another aspect I want to avoid, which I consider an annoyance in JavaScript, is using operators such as `typeof`, or `instanceof`, when a simple method or property will do, I would much rather use built-in language features.

```js
tj = User('tj', 'holowaychuk')

tj is_a =: type
  'user' == type

tj is_a('user')
// => true
```

 I have not yet decided on names etc, however these can all easily be implemented in the language itself, avoiding additional keywords.

### Function Literals

 Luna has first-class functions, much like JavaScript, Lua etc. The syntax of a function literal in Luna is as follows (_currently_):

```js
':' params? block
```

where `params` is simply:

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
  stdout write('hello ' + user + '\n')
```

Let's look at some more examples. The following Ruby selects `person`'s ferrets, older than `4`:

```ruby
person.pets.select do |pet|
  pet.species == 'ferret' and pet.age > 4
end
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

### Number Literals

 Luna currently provides literals for base 16 (hexadecimal):

```js 
0xff
```

 as well as base 10 (decimal), which (_optionally_) may contain underscore separators, inspired by Ruby:

```rb
million = 1_000_000.00
```

## Implementation Details

### Lexical & Semantic Analysis

 Both the lexer and parser are hand-coded. The parser
 consists of a small, simple, and recursive descent parser,
 capable of producing an in-language AST for 1mb of Luna script (~200k LOC) in 100ms.

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
