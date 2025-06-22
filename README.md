# oosem — Object Oriented asSEMbly

A minimalistic, powerful assembly language inspired by object-oriented principles — designed to simplify low-level programming by unifying pointers, integers, and dynamic-sized structs in a single clean VM.

---

## About

oosem is an experimental open-source project aiming to bring accessibility and clarity back to assembly-level programming. It offers:

- Struct-aware memory with precise byte-level access  
- Function calls as numbered code sections with a call stack  
- Dynamic data structures using pointers and linked lists  
- A unified integer-pointer type system simplifying data manipulation 

The first section in your code is always the program entry point, and the language encourages modular source composition by concatenating multiple files.

---

## Philosophy

This project is 100% open source. You are free to use, modify, and redistribute the code **however you like**.  

- There is no license file included intentionally.  
- If you want to create your own version, fork or clone this repo and maintain your changes in your own repository.  
- Feel free to build your own tooling, extensions, or experiment with new features independently.  
- The original source here is the canonical reference implementation — users can always download and compile it directly.  

This approach empowers you to adapt oosem for your needs without restrictions while ensuring the core remains a clean, stable foundation.

---

## Getting Started

### Syntax

The rules are simple: All of the code is made up of lines of 8 bytes length with the first byte being the command. Lines with less bytes are completed with 0x00. the things each command do are listed in "opcodes.txt" along with the syntax. This version only supports binary files because this is a simple language anyway, there's not much of anything to memorize. There are only 31 different opcodes.

Special commands and their meanings:

- 0x00 is the no operation opcode.

- 0x10 is the set command which is the only way to put a literal directly into a register.

- 0x20 is the command that seperates sections (sections being similar to functions in principle). It comes after every section of code and declares the first section to be the main entry point.

- 0x21 is used to define structs. It allows for only 2 fields at first but adding more is possible with the command 0x22. It takes three arguments: two field lengths in bytes and a numerical id from 0x0 to 0xF for the struct type.

- 0x22 is used for adding a new field to a struct with a known id. it takes two arguments: the first is the struct type id, and the second one is the length of the new field in bytes.

- 0x24 and 0x25 are essentially the getter and setter commands for the structs. they take 5 arguments: the first is a register and the others are the values for the base memory address, the struct type id, the field identifier and the location from the field for specifying which byte of the field is to be used. 0x24 gets the byte value into the register and 0x25 sets the byte to the value found in the register.

### Example program:

```
21 02 02 05 00 00 00 00 // define the struct type

10 01 41 00 00 00 00 00 // set register values
10 02 42 00 00 00 00 00
10 03 43 00 00 00 00 00
10 04 44 00 00 00 00 00

25 01 00 05 01 01 00 00 // set data from registers into memory as a struct
25 02 04 05 01 01 00 00
25 03 08 05 01 01 00 00
25 04 0C 05 01 01 00 00

28 01 00 00 00 00 00 00 // call next section

20 00 00 00 00 00 00 00 // end the section

24 04 00 05 01 01 00 00 //get the values from memory in reverse
24 03 04 05 01 01 00 00
24 02 08 05 01 01 00 00
24 01 0C 05 01 01 00 00

40 01 00 00 00 00 00 00 // print the reversed values as characters
40 02 00 00 00 00 00 00
40 03 00 00 00 00 00 00
40 04 00 00 00 00 00 00

29 00 00 00 00 00 00 00 // return from the called section

20 00 00 00 00 00 00 00 // end the section
```

### Compiling and usage

To compile the source code into an executable, just save the program file "main.c" in the same folder with the header file "linkedlist.h" and compile with your preferred c compiler. The executable tells you the correct usage when you run it without command line arguments.

---

## Contributions

Contributions are welcome! If you want to propose changes or improvements, consider:

- Forking this repo and maintaining your own variant  
- Sharing your enhancements as separate repositories or patches  
- Keeping your fork up-to-date with the original if you want to merge new features  

This way, everyone benefits from a vibrant ecosystem while the core stays consistent.

---

## Contact

- E-mail address: borayilmaz2009@hotmail.com
