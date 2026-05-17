# Flower Roadmap

Where Flower is going. Subject to change based on feedback, experiments, and progression.

## v1.1.0 (Current - Stability)

Focus: Polish the working compiler.

- [ ] Error Recovery            : Continue parsing after first error
- [ ] Better Error Messages     : Context, suggestions, etc
- [ ] Standard Library Basics   : More stdio, string operations
- [ ] Documentation             : Complete language spec examples

## v1.2.0 (Strings & Bools)

Core feature: Proper String and Boolean support.

- [ ] String operations
- [ ] Boolean support distinct from `1` and `0`
- [ ] Examples and tests

**Why String Operations and Booleans first?** Many programs need them. Current work arounds are verbose.

## v1.3.0 (Better Types)

Improve the type system.

- [ ] Type alias: `type MyInt = int`
- [ ] Nullable types: `?int` for optional values
- [ ] Better type inference in some contexts
- [ ] Union types (experimental)

## v1.4.0 (Standard Library)

Build out what is needed — stop relying on C backend.

- [ ] File I/O (read, write, open, close)
- [ ] Array Utilities (length, slice, sort)
- [ ] Math functions (sqrt, sin, cos, etc)
- [ ] String utilities (trim, split, replace)

## v1.5.0+ (Experiments)

- **Generics**: `<T> T max(a: T, b: T)`
- **Pattern Matching**: Better than `if` / `else`
- **Immutability by Explicity**: `x: int` vs `const x: int`
- **Operator Overloading**: Explicit overloads for operators on custom types
- **Defer Statement**: Cleanup code without explicit `prune` at end of scope
- **Error Types**: Structured return types for success / failureresults (e.g. Result-like enums)
- **Intrfaces**: Define contracts without inheritance or OOP hierarchies

## v2.0.0 (Non-C Codegen)

Rewriting the Flower Codegen to output to either Assembly, Binary, IR, or other machine code instead of C; Direct compilation.

- **Decide Target**: Assembly, Binary, IR, ByteCode, etc
- **Rewrite Codegen**: Instead of outputting to a .c file, output to .o or other
- **Optimization**: Short-circuit evalutation, pruning dead-branches, etc

## Non-Goals

Things Flower probably *won't* do:

- **Garbage Collection**: Manual memory exists; use it.
- **Object-Oriented**: No classes, inheritance, or virtual methods. Use Structs and Functions.
- **Implicit Conversion**: Be explicit. `x: int = 5.0 as int`
- **Macros**: Keep it simple. Generate C if you need metaprogramming.
- **Module System Beyond Imports**: Current system works, no overengineering.

## How to Help

### Testing

- Use the current version, find bugs, report issues, test edge cases
- Benchmark and optimize
- Test on different platforms (MacOS, Linux, Windows, Embedded)

### New Documentation

- Write examples
- Clarify confusing parts of the spec
- Create tutorials

### Experimentation

- Branch, try ideas, report results
- Push boundaries
- Help decide what belongs in Flower

### Standard Library

- Implement missing functions
- Write tests for stdlib
- Optimize common operations

## How Priority is Decided

1. **Blocking Issues**: Bugs that prevent normal use
2. **Language Gaps**: Things you can't express properly (like bools)
3. **Developer Experience**: Error messages, warnings, tooling
4. **Performance**: Optimizations that help real programs
5. **Nice to Have**: Everything else

## Feedback

This roadmap is a guide, not gospel. As Flower grows, priorities shift. Input you share helps shape its direction.

- Open an issue with ideas
- Comment on proposed ideas
- Show off what you've made with Flower

That's what determines what matters.
