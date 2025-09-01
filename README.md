# extexpr

The `extexpr` package extends built-in commands used in `[expr]` command.

## Installation

To install, run 
```bash
./configure
sudo make install
```
If you have different versions of Tcl on the same machine, you can set the path to this version with `-with-tcl=path`
flag to configure script.

On Windows you can use [MSYS64 UCRT64 environment](https://www.msys2.org/), the above steps are identical if you run it
from UCRT64 shell. After installing the package, you can move extexpr package folder (usually located in
`C:\msys64\ucrt64\lib\`) to path listed in `auto_path` variable of your local Tcl installation.

## Supported platforms

Any OS that has tcl8.6/tcl9.0 (Linux, Windows, FreeBSD).

## Description

### Vector/scalar operations

The first block of functions implements vector-vector, vector-scalar, scalar-vector and scalar-scalar operations.
Vectors are represented by lists in Tcl, and scalars - by single numbers.
They are implemented with the same functions of two arguments: `sum()`, `sub()`, `mul()`, `div()`, `pow()` and `dot()`.
The result depends on the size of operands, according to this table:

| Function | 1st argument | 2nd argument | Result | Equation                            |
|:---------|:-------------|:-------------|:-------|:------------------------------------|
| `sum()`  | scalar       | scalar       | scalar | `a+b`                               |
|          | vector       | scalar       | vector | `[a0+b, a1+b, a2+b, ..., aN+b]`     |
|          | scalar       | vector       | vector | `[a+b0, a+b1, a+b2, ..., a+bN]`     |
|          | vector       | vector       | vector | `[a0+b0, a1+b1, a2+b2, ..., aN+bN]` |
| `sub()`  | scalar       | scalar       | scalar | `a-b`                               |
|          | vector       | scalar       | vector | `[a0-b, a1-b, a2-b, ..., aN-b]`     |
|          | scalar       | vector       | vector | `[a-b0, a-b1, a-b2, ..., a-bN]`     |
|          | vector       | vector       | vector | `[a0-b0, a1-b1, a2-b2, ..., aN-bN]` |
| `mul()`  | scalar       | scalar       | scalar | `a*b`                               |
|          | vector       | scalar       | vector | `[a0*b, a1*b, a2*b, ..., aN*b]`     |
|          | scalar       | vector       | vector | `[a*b0, a*b1, a*b2, ..., a*bN]`     |
|          | vector       | vector       | vector | `[a0*b0, a1*b1, a2*b2, ..., aN*bN]` |
| `div()`  | scalar       | scalar       | scalar | `a/b`                               |
|          | vector       | scalar       | vector | `[a0/b, a1/b, a2/b, ..., aN/b]`     |
|          | scalar       | vector       | vector | `[a/b0, a/b1, a/b2, ..., a/bN]`     |
|          | vector       | vector       | vector | `[a0/b0, a1/b1, a2/b2, ..., aN/bN]` |
| `pow()`  | scalar       | scalar       | scalar | `a^b`                               |
|          | vector       | scalar       | vector | `[a0^b, a1^b, a2^b, ..., aN^b]`     |
|          | scalar       | vector       | vector | `[a^b0, a^b1, a^b2, ..., a^bN]`     |
|          | vector       | vector       | vector | `[a0^b0, a1^b1, a2^b2, ..., aN^bN]` |
| `dot()`  | scalar       | scalar       | scalar | `a*b`                               |
|          | vector       | vector       | vector | `a0*b0+a1*b1+a2*b2+...+aN*bN`       |

Consider the examples, first is sum of two vectors:

```tcl
package require extexpr
interp alias {} = {} expr

set a {1 2 3 4 5 6}
set b {9 8 7 6 5 4}
set y [= {sum($a,$b)}]
```
```text
==> 10.0 10.0 10.0 10.0 10.0 10.0
```

Power of vector:

```tcl
set y [= {pow($a,2)}]
```
```text
==> 1.0 4.0 9.0 16.0 25.0 36.0
```

Power of scalar:

```tcl
set y [= {pow(2,$a)}]
```
```text
==> 2.0 4.0 8.0 16.0 32.0 64.0
```

These 5 functions implemented in C code for efficiency.

### Aliases for Tcl list commands

Package add aliases for `lindex`, `llength` and `lrange` and ability to use it in `[expr]` as a function:

- `lindex list ?index ...?` -> `lindex(list,index,...)` or `li(list,index,...)`
- `llength list` -> `llength(list)` or `ll(list)`
- `lrange list first last` -> `lrange(list,first,last)` or `lr(list,first,last)`

Also, commands `::tcl::mathfunc::max` and `::tcl::mathfunc::min` now have `::tcl::mathfunc::maxl` and 
`::tcl::mathfunc::minl` versions that accepts lists instead of many arguments:

```tcl
set numbers {86 982 81 64 1 0.1}
set max [= {maxl($numbers)}]
```
```text
==> 982
```

```tcl
set min [= {minl($numbers)}]
```
```text
==> 0.1
```

### New commands

Some new command was added:

- `tcl::mathfunc::logb value base` - logarithm with arbitary base 


