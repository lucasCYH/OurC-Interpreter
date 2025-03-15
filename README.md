# OurC Interpreter

## Introduction

OurC is a simplified C-like programming language with a custom grammar and interpreter. This project implements an interpreter for OurC, enabling the execution of OurC programs with basic operations, control structures, and error handling.

## Features

- Supports primitive data types: `int`, `float`, `char`, `bool`, `string`
- Implements standard operators: `=, +=, -=, *=, /=, %=, ? :, &&, ||, !, ==, !=, <, >, <=, >=, <<, >>, +, -, *, /, %`
- Implements control structures: `if-else`, `while`, `do-while`
- Supports array operations
- Provides `cout` for output and `cin` for input
- Implements lexical, syntactical, and semantic error handling
- Type compatibility handling with automatic coercion

## Grammar Overview

The OurC grammar follows structured parsing rules with defined precedence levels. Key components include:

- **Expressions**: Built from `basic_expression` elements separated by `,`.
- **Operators Precedence**: Operators are classified by precedence levels, ensuring correct parsing.
- **Function Calls**: Functions can be defined and called with parameter passing.
- **Variable Declarations**: Supports primitive types and arrays.

## Interpreter Implementation

### Error Handling

OurC interpreter detects and handles three types of errors:

1. **Lexical Errors**: Invalid tokens (e.g., unexpected characters)
2. **Syntactical Errors**: Unexpected tokens that violate grammar rules
3. **Semantic Errors**: Undefined variables or incorrect expressions

### Type Compatibility

- `string` is compatible with any type in `+` operations
- `boolean` is only compatible with `string`
- `int` and `float` are mutually compatible
- `char` is incompatible with other types except `string`
- `<<, >>, %` require integer operands

### Input and Output

- `cin` allows user input using `cin >> var;`
- `cout` enables formatted output with `cout << expr;`
- `InputBufferAppend` and `EndInputBufferAppend` handle input buffering

## Usage

1. Compile and run the interpreter.
2. Enter OurC statements interactively.
3. Use `cout` for output and `cin` for input.
4. Utilize provided test cases to validate implementation.
