# Beanstalk

“Fee-fi-fo-fum,\
I smell the blood of an Englishman.\
Be he alive, or be he dead,\
I’ll grind his bones to make my bread.”

<img src="https://github.com/Jtog123/Beanstalk/blob/3a90e80d5877af6dba166c91c898c4221f4d1f7a/JackBeanstalkGiant.jpg" width="400" height="500">

#Overview

Beanstalk will be a general purpose, dynamically typed OOP language written in C. It will contain most modern language features, like functions and classes. Some syntax will differ from most modern languages for pure differentiation. 

#Motivation and Purpose

My motivation for the project stemmed from wanting a better understanding of how programming languages work under the hood. Most modern languages are highly abstracted, and while abstractions come with many benefits to programmers, I believe abstractions can make it harder to grasp your computer science fundamentals. Fundamentals like understanding memory management or how instructions are being executed.

The purpose of the project is to build a language from scratch. By building out the language features I take for granted one by one I hope to solidify my understanding of how they work.

The inspiration for the name comes from the old English fairy tale "Jack and the Beanstalk".

#Background and Research

Some of my background on this subject comes from a compilers class I took here at Marywood. Our class built out a Scanner and a Parser for a small instruction set. So I have a very basic understanding of how languages work. 

My research and guide to writing Beanstalk will come mostly from a book called “Crafting Interpreters” by Robert Nystrom. I will also be referencing a book on the C language by Brian Kernighan and Dennis Ritchie. As well as general C documentation.

#Proposed Tools and Technologies

Pure C and github. NO DEPENDENCIES. 

#Planned features and Functionality

1. Dynamically typed Values: Ints, Doubles, etc
2. Strings
3. Global Variables
4. Local Variables
5. If (fi) statements, while loops, for (fo) statements
6. Functions (Fum)
7. Classes (Fee)
8. Garbage Collection?

The language will look like most other C style languages with a few minor differences implemented for pure differentiation.

OOP (Object Oriented Programming) languages come with a few key language features which Beanstalk will also implement. These features are Classes, functions, for loops, and if statements.
Typically in most languages they would be defined something like this:

class Object;\
int functionName {};\
for(int i = 0; i < conditionall; i++);\
if(conditional);

Beanstalk will maintain the functionality of these features, but adjust the syntax

Fee = Class/Object\
fi = if statements\
fo = for loops\
fum = functions

Like the Giant from Jack and the Beanstalk.

Some snippets might look something like

fum add(var a, var b) {\
  return a + b;\
}

fi(a > b){\
  //do something
}

# How it will function
Compiler **GENERATES** Bytecode **FED TO** Virtual Machine

**The Compiler:**\
Will probably built with 2 seperate components the Scanner and the Parser.\
The Scanner will form the Tokens, the Parser will consume them and generate bytecode instructions and then feed them to the Virtual Machine.

**The Virtual Machine:**\




Bytecode over Machine Code\
Machine code is much faster, but it's also architecture dependent. In other words if we wanted Beanstalk to compile directly to machine code on both Windows and Mac I would need seperate builds for Intel and ARM chips. 

By using bytecode we avoid seperate builds and generate an architecture neutral representation and then let the virtual machine translate it during runtime. Although the code generated won't be as fast as machine code we avoid the headache of cross compiling on different architectures, while also achieving portability.

What exactly is Bytecode?\
Bytecode is intermediary code designed to run on virtual machines rather than directly onto CPU's. Structurally it somewhat resembles machine code. It's a sequence of binary instructions, but its much simpler and higher level than any real chip. To make the bytecode instructions execute we will need to write a piece of software that interprets the bytecode one instruction at a time. This piece of software is the virtual machine.

The virtual machine layer is what will make Beanstalk slower, but allow it to run on whatever hardware we would like.

TLDR\
In short I'm writing a compiler that generates bytecode instructions, and a virtual machine which reads/runs those bytecode instructions rather than the CPU.

Specs:
Background and Research: The creation of the Beanstalk language will rely on a couple different references. The first and primary piece of reference material will be “Crafting Interpreters” by Robert Nystrom. From this book I hope to learn about generating bytecode, creating a virtual machine, and learning the nuts and bolts of memory management. From a past compilers class I have some familiarity with basic lexing and parsing for a toy instruction set called ILOC, I may also be referencing that code for this project.
The second piece(s) of reference material will be “The C Programming language” by Brian Kernighan and Dennis Ritchie, and just C documentation in general. I took an Operating Systems class where we programmed mostly in C, but it’s not my strongest language, so I will need to be referencing documentation.
Proposed Tools and Technology: Plain C, Git for version control.
Features and Functionality: As mentioned previously there will be syntax differences created for pure differentiation. 
Fee = Classes, fi = if statements, fo = for loops, fum = functions
A minimum working version of Beanstalk will hopefully look something like a baby version of Python. 
Variables (Local and global) are a must, loops are a must, strings are a must, conditional statements are a most, functions are a must. I hope to have time to also implement Classes. I also believe the book covers garbage collection which I would love to learn time permitting. If I’m able to do the garbage collection I would like to keep extending and optimising the language but for now any features besides the ones I said are a must I will have to leave undefined.
Scopes and Limitations: May not implement garbage collection, entirely time dependent. It will be a dynamically typed language so no declaring explicit types like Ints or doubles, data types will instead be inferred by the compiler. 
Timeline:
Hadn’t really thought about this I may get a lot done one day, and get stuck on a difficult concept the next. I will say
Week 1-2:  Build the underlying Chunk data structures needed to hold instructions.
Week 2-4: Book builds the VM before the lexer, I would like to go very slow through this part.
Week 4-5: Build the lexer, have previous experience with this, can probably move faster.
Week 5-6: Hope to be correctly parsing basic expressions by this point.
Week 6-7: Implementing string, local variables, and global variables by this point.
Week 7-9 if (fi) statements, While statements, (fo) for statements, also (fum) functions hopefully.
Week 9- will update this as I go
Success Criteria: At a minimum Beanstalk should support Beanstalk supports variables, arithmetic, conditionals, loops, and basic functions, and runs in a VM that executes its bytecode (Exactly what you said). Ideally though it supports even more.




