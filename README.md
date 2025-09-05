# Beanstalk - A General Purpose Programming Language

“Fee-fi-fo-fum,\
I smell the blood of an Englishman.\
Be he alive, or be he dead,\
I’ll grind his bones to make my bread.”

<img src="https://github.com/Jtog123/Beanstalk/blob/3a90e80d5877af6dba166c91c898c4221f4d1f7a/JackBeanstalkGiant.jpg" width="400" height="500">

# Overview

Beanstalk will be a general purpose, dynamically typed OOP language written in C. The language is also interpreted, meaning all instructions are executed one by one as the program is running. It will contain most modern language features, like functions and classes. Some syntax will differ from most modern languages for pure differentiation. 

# Motivation and Purpose

My motivation for the project stemmed from wanting a better understanding of how programming languages work under the hood. Most modern languages are highly abstracted, and while abstractions come with many benefits to programmers, I believe abstractions can make it harder to grasp your computer science fundamentals. Fundamentals like understanding memory management or how our programs are actually running.

The purpose of the project is to build a language from scratch. I take many language features for granted so by building out the feature one by one I hope to solidify my understanding of how they truly work.

The inspiration for the name comes from the old English fairy tale "Jack and the Beanstalk".

# Background and Research

Some of my background on this subject comes from a compilers class I took here at Marywood. Our class built out a Scanner and a Parser for a small instruction set. So I have a very basic understanding of how languages work. 

My research and guide to writing Beanstalk will come mostly from a book called “Crafting Interpreters” by Robert Nystrom. I will also be referencing a book on the C language by Brian Kernighan and Dennis Ritchie. As well as general C documentation.

# Proposed Tools and Technologies

Pure C and github. NO DEPENDENCIES. 

# Planned features and Functionality

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
&nbsp;return a + b;\
}

fi(a > b){
  //do something
}

# How it will function
Compiler **GENERATES** Bytecode. The Bytecode will be **FED TO** the Virtual Machine

**The Compiler:**\
Will probably built with 2 seperate components a Scanner and a Parser.
The Scanner will form the Tokens, the Parser will consume them and generate bytecode instructions and then feed them to the Virtual Machine.

**The Virtual Machine:**\
A virtual machine is essentially a CPU written at the software level. To keep Beanstalk architecture independent we need to write a basic emualtor that understands Beanstalks instruction set.
The virtual machine will contain an instruction pointer and perfrom the basic duties of a real CPU. It will **fetch an instruction**,**decode said instruction**, then **execute said instruction** . Repeat.

**Bytecode over Machine Code:**\
Machine code is much faster, but it's also architecture dependent. In other words if we wanted Beanstalk to compile directly to machine code on both Windows and Mac I would need seperate builds for Intel and ARM chips. 

By using bytecode we avoid seperate builds and generate an architecture neutral representation and then let the virtual machine translate it. Although the code generated won't be as fast as machine code we avoid the headache of cross compiling on different architectures, while also achieving portability.

You can think of the Bytecode as instructions for the Virtual Machine. They tell the virtual machine what operations to perform when we encounter a certain instruction.
For example if the virtual machine reads in an instruction OP_RETURN. The virtual machine will know that this means its time to return from a function, and it will execute that behaviour. Byetcode can also be more formally known as an **intermediate representation**.

# Project Scope and Limitations

May not implement garbage collection, entirely time dependent. It will be a dynamically typed language so no declaring explicit types like Ints or doubles, data types will instead be inferred by the compiler.

# Timeline and Milestones

Week 1 - 2:  Build the underlying Chunk data structures needed to hold instructions.\
Week 2 - 4: Book builds the VM before the lexer, testing hardcoded instructions.\
Week 4 - 5: Building the scanner.\
Week 5 - 6: Parsing basic expressions. More dynamic instruction testing.\
Week 6 - 7: Implementing string, local variables, and global variables by this point.\
Week 7 - 9 if (fi) statements, While statements, (fo) for statements, also (fum) functions hopefully.\
Week 9 - Will update this as I go.

# Success Criteria
At a minimum Beanstalk should support variables, arithmetic, conditionals, loops, and basic functions, and runs in a VM that executes its bytecode (Exactly what you said). Ideally the language should work to the point where you could write a small program in it. Think a baby version of Python.




