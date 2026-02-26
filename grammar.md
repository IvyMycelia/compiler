# Grammar

## File Structures

### Header File

```rust
/* custom.headerExtension */
x: int
void mone()
int mtwo(name: type)
```

### Module FIle

```rust
/* custom.extensionTBD */
let x: int == 0

void mone() :>
    // Code
end

int mtwo(name: type) :>
    // Code
end

float helperFunction(name: type) :> // Private function because not in header file
    // Code
end
```

### Importing File

```rust
// main.extensionTBD
import "./math.extensionTBD" as math    // When given an alias, functions and variables are accessed via alias.var / alias.func()
import <system.extensionTBD>            // When no alias provided, functions and variables are accessed by their names e.g; var, func()
import "custom.extensionTBD"            // No importing specifics; everything in the header file is what is imported, but no overhead because compiler only inlines what's needed not everything (helps with file size and compile time)
/*
You can import absolute file path, relative file path, and assuming you have compiler configs set (to be implemented eventually), you can just do <> for system path (such as usr/opt) libs and "relative" (assuming relative filepath is set, e.g; ./include/)
*/

let x: float64 = math.PI // Or just PI if imported with no alias
let y: float64 = sqrt(x) // Or math.sqrt(x) if imported with alias
```

### Example main.extensionTBD

```rust
/* Option custom _start() */
void _start() :>
    main()
end

/* Default program entry */
int main(args: string[], argc: int) :>
    if (argc > 0) :>
        if (args[2] == "--p") :>
            print("P Mode enabled\n")
            return 1
        else if (args[2] == "--b") :>
            // B mode code
        else :>
            print("Unrecognized symbol\n"); return -1
        end
    else print("Not enough arguments\n")

    return 0; // Semicolons optional to end a line
end
```
