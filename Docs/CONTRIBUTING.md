# Contributing to Flower

Flower welcomes experiments, improvements, and new ideas. This document explains the process of contributing.

## Philosophy

Flower values simplicity, so before adding a feature, ask:

- Does this earn its place?
- Can it be simpler?
- Does it fit the language's goals?

## Getting Started

```bash
git clone https://github.com/ivymycelia/flower.git
cd flower
clang %s -o %s
make test
```

## Making Changes

### Branch making

When it comes to contributing to Flower, it's important for development purposes that everything is labelled correctly to make sure priority and discussion remains on-task.

When you make a branch, there's six different **prefixes**  you can use:

- `feature/name`    — New language feature or capability
- `fix/name`        — Bug fix
- `experiment/name` — Experimental optimization, syntax, or feature (something that uses a different implementation than currently applied)
- `docs/name`       — Documentation
- `refactor/name`   — Code restructuring
- `chore/name`      — Maintence, tooling, cleanup, CI, build scripts, etc (anything that doesn't fit above or doesn't have to do with docs or code)

For example, if you wanted to make a **feature** contribution that implements **booleans**, you could do:

`feature/boolean-implementation`

NOTE: If you are contributing an 'experiment,' please include a `synopsis.md` that explains your methodology, reason for contributing, and an outline of what you've accomplished. Any Pull Requests for experiments without one will will take longer to review, which may delay any feedback / merge. 

### Workflow

1. Create a branch from `main`:

```bash
`git checkout -b [prefix]-[your-idea]`
```

2. Make changes, test frequently:

```bash
make bootstrap # Safe compile with backups
make test      # Run test suite to ensure no regression
```

3. Write clear commit messages:

```md
feature: add boolean type to language
* Implement boolean as an alias over `1` and `0` truth values
* Add boolean literal `true` / `false`
* Update parser for boolean syntax
* Add examples in examples/types/bool.flo
```

4. Push and create a **Pull Request**:

```bash
git push origin [prefix]/[your-idea]
```

5. After review, your contribution will either be **merged** or **closed** with feedback regarding the decision

## Testing

All changes that effect the **Flower Compiler** must **pass** the *full test suite*:

```bash
make test
```

If a contribution implements a new feature or changes an existing feature's syntax, tests must be added / updated under examples:

- `examples/types/`     — Type features
- `examples/control/`   — Control flow
- `examples/memory/`    — Memory tooling and management
- `examples/io/`        — Input / Output

## Code Style

No strict formatter yet, but aim for:

- 4-space indentation
- Clear variable names
- Comments for non-obvious logic (when in doubt, comment)
- One statement per line (usually)

Example:

```lua
int calculate(x: int, y: int):
    sum: int = x + y
    product: int = x * y

    if product > sum:
        return product
    end
    return sum
end
```

## Compiler Architecture

The compiler pipeline (all in Flower):

1. **Lexer** (`src/lexer.flo`, `include/lexer_h.flo`)   — Tokenizes source
2. **Parser** (`src/parser.flo`, `src/ast.flo`)         — Builds AST
3. **Codegen** (`src/codegen.flo`)                      — Generates C code

Output: Cfile —> Compiled with Clang —> Executable

Changes to any party needs testing through the full pipeline.

## Experiments

Have an optimization idea? A change in syntax? Try it:

```bash
git checkout -b experiment/[your-idea]
# modify parser, codegen, etc
make bootstrap
make test

# Both bootstrap and test must pass. Just because one or the other passes doesn't mean it worked.

# Benchmark if performance-related
# Does it help? Create PR with synopsis and results.
# Doesn't? Document what you learned.
```

Even failed experiments are valuable—document findings in the PR. It could even lead to a new idea later on!

## Large Changes

For signifigant features (new type system, module system, etc):

1. Open an issue to first discuss
2. Propose implementation approach
3. Get feedback before starting major work
4. Break into smaller PRs if possible / needed

Following this step-by-step process helps avoid someone putting in a lot of effort into something that has already been done or could use more planning

## Questions?

- Open an issue and discuss
- Check existing issues / discussions
- Look at [ROADMAP.md](docs/) for planned work

## Code of Conduct

Be respectful. We're all learning. Disagreements are fine; *personal attacks are **not***.
