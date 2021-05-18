This doc and reproduce code can be found [here](https://github.com/DataCorrupted/llc-func-removal-report)

# llc function call removal bug

## Summary

When instrumenting function calls, all of them are instrumented correctly in LLVM IR.
But __SOME__ of the function calls are removed when generating assembly(before linking).
These function calls are in 3rd party library and are not present until linking, therefore, they shouldn't be optimized out.
I couldn't figure out what is the criteria for such removal.

## Scope

The bug can be reproduced in Ubuntu 1604 and 1804, using any LLVM > 10.0.0.

The demo included uses a docker environment for easier debugging, it runs in Ubuntu 1604 and LLVM 11.1.0.

## Reproduce

To reproduce, you may run `./scripts/reproduce.sh`.
It would compile a docker image and run the code.

Or you can do 

```sh
# If you don't have llvm in your local machine
./scripts/install_llvm.sh
./scripts/build.sh
cd src
make test
```

The final lines of output should look like this:

```
llvm-dis ./build/bin/main.0.5.precodegen.bc 
dummy_func calls in bitcode before assembly generation: 
  %11 = call i8 @dummy_func_getc()
  %19 = call i8 @dummy_func_getc()
  %25 = call i8 @dummy_func_icmp()
declare i8 @dummy_func_icmp() #3
declare i8 @dummy_func_getc() #3

llc ./build/bin/main.0.5.precodegen.bc 
dummy_func calls after assembly generation: 
	callq	dummy_func_getc

When executing main: 
dummy_func called before _IO_getc().

When executing main-pm: 
dummy_func called before _IO_getc().
```

Indicating that 3 function calls are inserted to LLVM IR, only one remains in assembly and binary.

## Code description

The example code here shows one example.
The pass is fairly simple, for each ICMP instruction, instrument a function call to `dummy_func_icmp` beforehand; for `getc` function call, instrument a call to `dummy_func_getc`.

Both `char dummy_func_*()` functions are copmiled into a 3rd library and linked to the program later.
These functions, as name suggests, does nothing except printing out a statement saying that it is executed.
(I think having IO in these functions makes them impossible to be optimized out.)

The `main.c` only gets two bytes from `stdin`, allocate a buffer and does some ICMP.

When instrumenting code in `src`, LLVM IR shows that two `dummy_func_getc` and one `dummy_func_icmp` are instrumented correctly.
But when generating assembly using `llc` and executing final binary, only one `dummy_func_getc` is preserved.

## Thoughts

Here are some possible reasons that I have ruled as impossible.

1. It's the asssembler's optimizer. However, the assembler hasn't seen what `dummy_func*` looks like, so there is no reason it should remove a 3rd library function call.
2. It's LTO. I used LTO for easier bitcode and assembly generation, as shown in `make main-pm`, even use legacy pass mamager in earlier stages, the generated binray still only have ONE instrumented function instead of three.
