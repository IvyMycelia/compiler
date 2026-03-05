# Grammar

## File Structures

### Header File

```rust
/* custom.floh */
x: int
void mone()
int mtwo(name: type)
```

### Module FIle

```rust
/* custom.flo */
let x: int = 0

void mone():
    // Code
end

int mtwo(name: type):
    // Code
end

float helperFunction(name: type): // Private function because not in header file
    // Code
end
```

### Importing File

```rust
// main.flo
import "./math.floh" as math    // When given an alias, functions and variables are accessed via alias.var / alias.func()
import <system.floh>            // When no alias provided, functions and variables are accessed by their names e.g; var, func()
import "custom.floh"            // No importing specifics; everything in the header file is what is imported, but no overhead because compiler only inlines what's needed not everything (helps with file size and compile time)
/*
You can import absolute file path, relative file path, and assuming you have compiler configs set (to be implemented eventually), you can just do <> for system path (such as usr/opt) libs and "relative" (assuming relative filepath is set, e.g; ./include/)
*/

let x: float64 = math.PI // Or just PI if imported with no alias
let y: float64 = sqrt(x) // Or math.sqrt(x) if imported with alias
```

### Example main.extensionTBD

```rust
/* Option custom _start() */
void _start():
    main()
end

/* Default program entry */
int main(args: string[]):
    if args.length > 0:
        if args[2] == "--p":
            print("P Mode enabled\n")
            return 1
        else if args[2] == "--b":
            // Empty block, stack is still pushed but nothing is inside this block. Waits for end to terminate or an else to attach
        else:
            print("Unrecognized symbol\n"); return -1
        end // This end tells the compiler that if y == 0 block is done and that anything that follows is for the original if x block
    else print("Not enough arguments\n") // No end is needed because the else only has one statement. The compiler knows this is the end of the whole block, because there's no : after the else meaning whatever follows HAS to be the last statement

    return 0; // Semicolons optional to end a line
end
```

## How If Conditionals Work

`else`/`else if` before seeing `end` —> belongs to the **innermost open** `if`
`end` closes the innermost open `if`, "surfacing" to the parent block
`else` with no `:` —> single-statement, no `end` needed, and **implicitly terminates** the entire chain

The compiler maintains a stack: each `if ... :` pushes, each `end` pops, and an `else`/`else if` hooks onto whatever is on top. An implicit `else`/`else if`/`if` self-enforces: because there's no `:` telling the compiler to push a stack, it automatically registers the following statement is part of the conditional's output and treats anything afterwards as independent of the conditional. There's no way to accidentally leave a dangling block, since the absence of `:` structurally prevents continuation.

```rust
if condition:       // colon = open block, ALWAYS needs end
    statements
end

if condition statement          // no colon = single statement, terminated by newline or ;
else if condition statement     // same
else statement                  // same

if condition:       // colon form can still have else/else if before end
    statements
else if condition:
    statements
end
```

### Legal Example

```rust
if x:
    if y:
        print("both")
    end
end
```

### Illegal Example

```rust
if x:
    // ...
end
else if y: // This is illegal because it comes after the stack is popped and the compiler does not know what `else` is referring to
```

```rust
if x
// Empty
// EoF
```

The above is illegal because the compiler never appends anything to the block nor terminates it. Instead of panicking it recognizes that there's no trailing statement following the conditional, and should return an error at compilation. If the user has a statement afterwards, no matter the intention, it will be attached to the block since the language is white-space insesnitive.
