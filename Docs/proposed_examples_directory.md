# Proposed Directory Layout for ../examples

```txt
examples/
  memory/
    pointer_deref.flo
    pointer_math.flo
    ptr.flo
    new_prune.flo
    sizeof.flo
    realloc.flo
    memory.flo        // imports and runs all memory tests
  types/
    int.flo
    float.flo
    double.flo
    char.flo
    null.flo
    cast.flo
    string.flo
    types.flo         // imports and runs all type tests
  control/
    if_else.flo
    while.flo
    for.flo
    not.flo
    unary_neg.flo
    binary_ops.flo
    control.flo       // imports and runs all control tests
  test.flo            // master test runner that imports all modules
```
