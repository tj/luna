  ![Luna programming language](http://f.cl.ly/items/3K2A3k1k3p2s0M3B3j1F/logo.png)

 Luna is an expressive, minimalistic, elegant programming language implemented in C. With cooperative thread concurrency at its core, async I/O, and influences derived from languages such as Lua, io, Rust, Ruby, and C. Luna favours unification and minimalism over minor obscure conveniences, providing the true convenience of a simple effective language. This includes omitting features which facilitate magic such as getters/setters, method_missing-style delegation etc. This project is _very_ much a work in progress, as I explore the wonderful world of VMs! feel free to join.

## Goals

  - fast!
  - small (~2000 SLOC)
  - simple, elegant, explicit
  - small learning curve
  - statically typed with inference
  - high concurrency out of the box via coroutines
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
  
**Note:** Luna doesn't run any code yet, it only parses the code and builds an AST. There are still some decisions to be made about certain aspects of the language. If you would like to join and finish designing Luna with the community, check out the [issue tracker](https://github.com/tj/luna/issues) and share your thoughts.

## Typing

  Luna is statically typed, however mostly optional thanks to type inference. Some declarations such as function parameters _must_ specify a type:

```ruby
def greet(name:string)
  return "Hello " + name
end
```

  Types that can be properly inferred may be, for example function return types as seen above. When fully-qualified the same function would look like this:

```ruby
def greet(name:string): string
  return "Hello " + name
end
```

## Function overloading

  Luna plans to provide multiple dispatch support. This will drastically cut down on verbosity and fragmentation. For example suppose you have a `vec_sum(v)` function, in Luna you would simply create a `sum(v)` function:

```ruby
def sum(v:vec): int
  ...
end
```

  Thanks to the typing system Luna can choose the correct function to invoke for the given parameters. This technique addresses another fundamental problem of many languages, fragmentation and delocalization. For example it is often tempting to extend native prototypes or classes provided by the host language, such as `Array#sum()`.

  Because no such construct exists in Luna you're free to "extend" these types elsewhere simply by defining functions that act on those types, without polluting "global" classes or objects, removing a layer of indirection, as it's often not clear where these additions came from, and they regularly conflict.

## Method syntactic sugar

  As previously mention Luna has no concept of classes, methods, or prototypes. To increase readability you may invoke functions as if they were methods. For example the following would be equivalent:

```ruby
cat('urls.txt').grep('https://').print()
```

```ruby
print(grep(cat('urls.txt'), 'https://'))
```


## Fork / join

  More often than not you want to perform several tasks in parallel, and "join" their results. For this luna provides the `&` postfix operator, which is syntax sugar for wrapping the expression in a `fork()` function call:

```ruby
a = get('http://google.com').grep('<title>') &
b = get('http://likeaboss.com').grep('<title>') &
c = get('http://cuteoverload.com').grep('<title>') &
res = join(a, b, c)
```

  This wraps each statement in a coroutine which may run independently.

## Operator precedence

 Operator precedence from highest to lowest:

```
operator                |  associativity
------------------------|---------------
[ ] ( ) .               |  left
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

## luna(1)

```

  Usage: luna [options] [file]

  Options:

    -A, --ast       output ast to stdout
    -T, --tokens    output tokens to stdout
    -h, --help      output help information
    -V, --version   output luna version

  Examples:

    $ luna < some.luna
    $ luna some.luna
    $ luna some
    $ luna

```

## Community

 - __irc__: #luna-lang on irc.freenode.net

## License

  MIT

