# Flower Programming Language

Minimalistic expressive programming language for systems programming, where control to write simple code that works and does powerful things is the center of decentralization.

## Philosophy

The anti-thesis of modern-day programming languages. 

Corpo-slop, governance, abstraction maximilism; all things that modern systems have drifted towards for its own sake. A machine-based ecosystem in a machine-based world, where people are no longer at the center. Flower exists as a rejection to that trajectory.

Uniformity and obscurity are broing. Flower embraces clarity over ceremony, control over opacity, and experimentation over enforced staleness.

- Write less, mean just as much (and more!)
- Explicity over implicity, what you say goes
- One way to do it (usually)
- Born from the ashes of conformity and standardization (C)

## What Flower Is

A compiled, self-hosting language that targets to C (ignore the irony in it being "born from the ashes"). You write Flower, FloC generates C (for now..), you get a binary. No runtimes, no VMs, and no magic other than the feeling of euphoria it gives off.

**Self-Hosting**: The Flower Compiler (FloC) is written in Flower, though the original FloC is written in C (can be accessed under `vendor/flower_c_reference/`).

**Minimal**: 30 Keywords (Less than it's prede~~opre~~scer!); Pointers with `@`, memory with `new` / `prune`, that's pretty much it.

**Extensible**: Different branches have different experiments / implementations. See what interests and tickles your fancy, what excels, and what absolutely sucks and deserves to remain abandoned.

## Examples

```lua
import <stdio>

int factorial(n: int):
    if n <= 1:
        return 1
    end
    return n * factorial(n - 1)
end

int main():
    printf("5! = %d\n", factorial(5))
    return 0
end
```

Structs with manual memory:

```lua
struct Node {
    value: int,
    next: @Node
}

@Node create_list(values: int[], len: int):
    head: @Node = null
    current: @Node = null

    for i in 0..len:
        node: @Node = new Node
        node.value = values[i]
        node.next = null

        if head == null:
            head = node
        else:
            current.next = node
        end
        current = node
    end

    return head
end
```

## Building

Preamble: You will need to compile [`bin/Flower.c`](https://github.com/IvyMycelia/flower/blob/main/bin/Flower.c) or download the latest [`Flower`](https://github.com/IvyMycelia/flower/releases/tag/Stable) release first to get the executable.

```bash
make build      # Compiles from .flo source and creates new Flower binary under `bin/` if there's no errors
make bootstrap  # Build with verification to ensure the new executable is valid before overwriting the stable, working one
make test       # Run the test suite with `/bin/Flower`
```

## Using Flower

```bash
./bin/Flower program.flo output_file
./output_file
```

Flower generates C code, compiles it with Clang, and produces a binary. It will run the binary if there's any output (this is for development purposes).

## Language Features

- **Variables & Types**: `int`, `float`, `double`, `char`, `string`, ~~`bool`~~ (BOOL NOT IMPLEMENTED YET)
- **Pointers**: `@int` for pointer to int, `&x` for address-of, `@x` for dereference
- **Control Flow**: `if` / `else`, `while`, `for`, `continue`, `break`, `return`
- **Functions**: Named parameters, explicit types, no overloading (YET)
- **Structs & Unions**: Group data, no methods (use functions instead)
- **Memory**: `new` allocates, `prune` frees
- **Imports**: Load other files, aliases, organize code into modules (TO BE IMPROVED)

## Experimentation & Contribution

Flower is a platform for compiler research and language design. The whole premise of this project was to see what was possible, and Flower derives its magic from its ability to "branch-off" and be experimented with.

**Try ideas in branches:**

```bash
git checkout -b experiment/optimization-x
# Implement ideas, test, benchmark
# Does it help? Submit a Pull Request! Doesn't? Learn and continue
```

Current experiments welcome. See [Docs/CONTRIBUTING.md](Docs/CONTRIBUTING.md) for specific details on how to contribute.

## Project Status

- **v1.0.0**: First self-hosted release
- **Stable enough to use**, still volatile enough for major changes
- Core language mostly stable; stdlib still growing
- Compiler works; error messages ~~could~~ WILL improve

## Next

- Error Recovery (better compiler feedback, non-terminating parser)
- String operations
- Standard library creation / expansion
- Performance optimizations
- Proper compilation (not just C transpiling)

See [Docs/ROADMAP.md](Docs/ROADMAP.md) for more

## Why Flower?

You want a language that:

- Doesn't hide what's happening,
- Lets you write fast and easily readable code,
- Stays out of your way,
- Doesn't try to appeal explicitly to machines

Flower is that, and much much more

---

**Getting Started**: Read[]()
