# Beanstalk

“Fee-fi-fo-fum,\
I smell the blood of an Englishman.\
Be he alive, or be he dead,\
I’ll grind his bones to make my bread.”


Beanstalk in a general purpose interpreted programming language. The creation of this language serves as an excerise in trying to understand the nuts and bolts of programming language design and creation.

The inspiration for the name comes from the old English fairy tale "Jack and the Beanstalk". The inspiration for the idea comes from Robert Nystrom's book "Crafting Interpreters" which will also serve as the guide to constructing this language.

<img src="https://github.com/Jtog123/Beanstalk/blob/3a90e80d5877af6dba166c91c898c4221f4d1f7a/JackBeanstalkGiant.jpg" width="400" height="500">

# How it will look

The language will look like most other C style languages with a few key differences implemented for pure differentiation.

OOP (Object Oriented Programming) languages come with a few key language features which Beanstalk will also implement. These features are Classes, functions, for loops, and if statements.
Typically in most languages they would be defined something like this:

class Object;\
int functionName {};\
for(int i = 0; i < conditionall; i++);\
if(conditional);

Beanstalk will maintain the functionality of these language features and only adjust the syntax

Fee = Class/Object\
fi = if statements\
fo = for loops\
fum = functions

For example...

# How it will function
compiler -> bytecode -> Virtual Machine execution

Bytecode over Machine Code\
Machine code is much faster, but it's also architecture dependent. In other words if we wanted Beanstalk to compile directly to machine code on both Windows and Mac I would need seperate builds for Intel and ARM chips. 

By using bytecode we avoid seperate builds and generate an architecture neutral representation and then let the virtual machine translate it during runtime. Although the code generated won't be as fast as machine code we avoid the headache of cross compiling on different architectures, while also achieving portability.

What exactly is Bytecode?\
Bytecode is intermediary code designed to run on virtual machines rather than directly onto CPU's. Structurally it somewhat resembles machine code. It's a sequence of binary instructions, but its much simpler and higher level than any real chip. To make the bytecode instructions execute we will need to write a piece of software that interprets the bytecode one instruction at a time. This piece of software is the virtual machine.

The virtual machine layer is what will make Beanstalk slower, but allow it to run on whatever hardware we would like.

TLDR\
In short I'm writing a compiler that generates bytecode instructions, and a virtual machine which reads/runs those bytecode instructions rather than the CPU.



