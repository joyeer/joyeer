# The Joyeer Programming Language


## Lexical

### Keyword
class
struct
var
let
if
else
for
while
import
func
try
return

### Primary Type
string
bool
int
uint
float
double

### Primary Value
true, false, nil

### Lexical Structure

#### whitespace lexical grammar
whitespace -> line-break | single-line-comment | multi-lines-comment | whitespace /opt/

line-break -> U+000A | U+000D | U+000D followed by U+000A

single-line-comment -> // single-comment-text
single-line-comment-text -> comment-text-item {single-line-comment-text}
single-line-comment-text-item -> Any unicode scalar value except U+000A or U+000D

multi-lines-comment -> /*   Any unicode scalar value */

#### identifier lexical grammar

identifier -> identifier-head identifier-characters /opt/

identifier-head -> [a-z] | [A-Z]
identifier-head -> _

identifier-character -> Digit 0 through 9
identifier-character -> identifier-head
identifier-characters -> identifier-character identifier-characters /opt/

#### literal grammar
literal -> numeric-literal | string-literal | boolean-literal | nil-literal
boolean-literal -> true | false
nil-literal -> nil

integer-literal -> decimal-literal | hex-literal

> decimal-literal ->  decimal-head decimal-digits
> decimal-head -> Digit 1 through 9
> decimal-digit -> Digit 0 through 9
> decimal-digits -> decimal-head {decimal-digits}

> hex-literal -> 0x hex-digit
> hex-digit -> Digit 0 though 9, a through f, or A through F
> hex-digits  -> hex-digit hex-digits /opt/

> floating-literal -> decimal-literal decimal-fraction /opt/ decimal-exponent /opt/
> decimal-fraction -> . decimal-literal
> decimal-exponent -> floating-point-p sign /opt/ decimal-literal
> floating-point-e -> e | E
> floating-point-p -> p | P
> sign -> + | -

> string-literal = “  quoted-text \opt\ “
> quoted-text -> quoted-text-item quoted-text /opt/
> quoted-text-item -> Any unicode scalar value except \
> quoted-text-item -> escaped-newline
> escaped-newline -> line-break

#### Operator
operator -> operator-head operator-characters /opt/
operator-head -> / | = | - | + | ! | * | % |  < | > | & | | | ^ | ~ | ?
operator-character -> operator-head
operator-characters -> operator-character operator-characters /opt/

prefix-operator -> operator
binary-operator -> operator
postfix-operator -> operator

## Grammar
### Type
type -> array-type
type -> dictionary-type
type -> Any
type -> optional-type
type -> implicitly-unwrapped-optional-type

array-type -> [ type  ]
dictionary-type -> [ type: type ]
optional-type -> type ?
implicitly-unwrapped-optional-type -> type !
type-annotation -> `:` type

### Pattern
pattern -> identifier-pattern type-annotation /opt/
identifier-pattern -> identifier

### Expression
expression -> prefix-expression  binary-expressions /opt/

> binary-expression -> binary-operator prefix-expression
> binary-expression -> assignment-operator prefix-expression
> binary-expression -> conditional-operator  prefix-expression
> binary-expressions -> binary-expression binary-expressions /opt/

> prefix-expression -> prefix-operator /opt/ postfix-expression

> postfix-expression -> primary-expression
> postfix-expression -> postfix-expression postfix-operator
> postfix-expression -> function-call-expression
> postfix-expression -> subscript-expression

subscript-expression -> `[` expr `]`

function-call-expression -> identifier function-call-argument-clause

function-call-argument-clause -> `(` `)` | `(` function-call-argument-list `)`
function-call-argument-list -> function-call-argument | function-call-argument `,` function-call-argument-list
function-call-argument -> identifier `:` expression

> primary-expression -> identifier
> primary-expression -> literal-expression
> primary-expression -> parenthesized-expression
> primary-expression -> self-expression

self-expression -> self | self-method-expression
self-method-expression -> self `.` identifier

parenthesized-expression -> `(` expression `)`

> literal-expression -> literal
> literal-expression -> array-literal | dictionary-literal
> array-literal -> [ array-literal-items /opts/ ]
> array-literal-items -> array-literal-item , /opt/ |  array-literal-item, array-literal-items
> dictionary-literal -> [ dictionary-literal-items ]  | [ : ]
> dictionary-literal-items -> dictionary-literal-item, /opt/ | dictionary-literal-item, dictionary-literal-items
> dictionary-literal-item -> expression: expression

assignment-operator -> =
conditional-operator -> ? expression :

### Statement

statement -> expression
statement -> declaration
statement -> loop-statement
statement -> branch-statement
statement -> return-statement

return-statement -> `return`  expression /opt/

> loop-statement -> for-in-statement
> loop-statement -> while-statement
>
> for-in-statement -> `for` pattern `in` expression code-block
> while-statement -> `while` expression code-block

branch-statement  -> if-statement

if-statement -> if condition-list code-block else-clause /opt/
else-clause -> `else` code-block | `else` if -statement


### Declarations
> declaration -> import-declaration
> declaration -> constant-declaration
> declaration -> variable-declaration
> declaration -> function-declaration
> declaration -> initializer-declaration
> declaration -> class-declaration
> declarations -> declaration declarations

initializer-declaration -> initializer-head parameter-clause code-block

code-blocks -> `{` statements /opts/  `}`

> import-declaration -> `import` import-path
> import-path -> import-path

constant-declaration -> `let` pattern-initializer
pattern-initializer -> pattern initializer /opt/
initializer -> `=` expression

variable-declaration -> variable-declaration-head pattern-initializer
variable-declaration-head -> `var`

function-declaration -> function-head function-name function-signature function-body
function-head -> `func`
function-name -> identifier
function-signature -> parameter-clause function-result
parameter-clause -> `(`  `)` | `(` parameter-list `)`
parameter-list -> parameter | parameter, parameter-list
parameter -> local-parameter-name type-annotation
local-parameter-name -> identifier
function-body -> code-block

class-declaration -> `class` class-name type-inheritance-clause class-body
class-name -> identifier
class-body -> `{` class-members /opt/ `}`

class-members -> class-member class-members /opt/
class-member -> declaration

###