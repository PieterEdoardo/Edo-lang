# Edo-lang
Low level programming language that aims to deliver 'controlled de-abstraction'. It achieves this by giving access to low level features like registers in a 'C-like' syntax. The compiler is currently in development and has a parser, but still misses the emitter. 

## Example usage
Example of how the syntax will work:

```c
arch x86_64 {
    register rax = registers_86_64_accumulation_64_RAX,
    register rdi = registers_86_64_destination_64_RDI,
    register rsi = registers_86_64_source_64_RSI,
    register rdx = registers_86_64_dataio_64_RDX
    register r8 = registers_86_64_R8
}

machine print(char* text, int size) {
    rax = 1;
    rdi = 1;
    rsi = text;
    rdx = size;
    syscall();
}

r8 = 5;
print(r8, 1);
```
