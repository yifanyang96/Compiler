# Code Generator

PA5.pdf, cool-runtime.pdf, cool-manual.pdf, SPIM_Manual.pdf and `trap.handler` are useful! Some of the generated code are so hard to think out that I referred to `coolc`'s code (like `case` and `new`). 

## Overall Structure

The following tasks are performed:

1. Code the `class_nameTab`

2. Code the prototype objects

3. Code the dispatch tables

4. Code the `class_objTab`

5. Code the initialization methods

6. Code the methods

## The Class Tags

The class tags are assigned recursively to the node and its children, which means its children's class tags are always bigger than its own. The reason will be clear in the case expression section.

## The Prototype Objects

As specified in the runtime manual, every time a new object is allocated, the prototype object of that type is first copied by `Object.copy` and passed in `$a0` and then initialization methods are called. It is OK to set all the attributes to `void`. Furthermore, when coding operation expressions `Int` prototype objects may be useful (any concrete object is equivalent).

## The Dispatch Tables

To implement the data struture of dispatch tables, a method name list is maintained in each node and method names are enqueued from the top of the inheritance hierarchy. The offset can be obtained by traversing the list. In order to know in which class the method is defined a symbol table mapping method names to class names is also maintained.

## Code Expressions

### Operations

Since the return value in `$a0` of every expression must be an object, the result of an operation must be stored in a new object.

### Identifiers

How to load and store identifiers is the most difficult part. I created two lists in each node recording let and case identifiers and formal identifiers. The runtime values (for formal bindings means dispatch parameters) are all pushed onto the stack. It must be kept in mind that identifiers in let and case bindings will not be passed in the method body (see operational semantics), which means parameters and let / case identifiers are separated by `$fp`. Therefore the offsets from `$fp` can be obtained by traversing the lists (parameters have positive offsets and let / case identifiers have negative ones). The elements are popped when the code is leaving the scopes. The lookup order for identifiers is let / case bindings, formal bindings and attribute bindings.

### `SELF_TYPE`

In code generation phase, the only place where `SELF_TYPE` needs to be considered is the `new` expression. `new SELF_TYPE` is coded by copying the prototype object in the `class_objTab` at the offset of `$s0`'s class tag and calling the following initialization method.

### Case Branching

It is solved by an elegant algorithm. See [Case Braching Algorithm](case.pdf). Therefore, we need to collect the branch types first and then sort them.

### Dispatches

The key question is: how do I know what the expression's runtime type is? The answer is you do not need to know because the method is guaranteed by the type checker to be defined inside the static type. I think this is the only reason to annotate types on the AST.

### Registers

I only use `$s1 $t1 $t2 $t3`. Whenever `$s1` is used remember to save it first (in some operation expressions and all methods).

## GC

I do not fully understand the mechanism of the GC so I just add codes for `GC_Assign`. You can refer to the original paper: [Generational GC](GC.pdf) and comments in `trap.handler`. 