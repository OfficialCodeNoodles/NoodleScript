# NoodleScript
This is a simple interpreted programming language that has syntax and features heavily inspired by Python, but with an Italian twist!

# Syntax and Features
## Comments
```
# This is a comment
```
## Variables
```
noodle number = 10
noodle bool = true
noodle string = "Hello"
noodle list = [10 true "Hello"]
```
## Constant Variables
```
frozen noodle x = 10
x = 10  # Doesn't work
```
## IO
### serve()
```
serve("Hello World!\n")  # Outputs `Hello World!` to the console
```
### receive()
```
input = receive()  # Parses input from user as a string
```
### If Statements
```
if condition
  # Body
eat
```
### Else Statements
```
if condition
  # Body
else
  # Body
eat
```
### While Loops
```
while condition
  # Body
eat
```
## Functions
```
recipe add(noodle num1 noodle num2)
  num1 + num2  # notice how no return statement is needed
eat
```
## Nested Functions
```
recipe div(noodle num noodle den)
  recipe safeToDiv(noodle den)
      den != 0
  eat

  if safeToDiv(den)
    num / den
  else
    serve("Cannot divide by zero\n")
  eat
eat
```
## Built-In Functions
```
serve()  # Outputs to console
serveRaw()  # Outputs the runtime value of an expression
receive()  # Parses input as string
abs(num)  # Absolute value
floor(num)  # Rounds down value
round(num)  # Rounds value
ceil(num)  # Rounds value up
sqrt(num)  # Takes square root of value
pow(num num)  # Raises value to power
num(any)  # Converts value to a number
bool(any)  # Converts a value to a bool
str(any)  # Converts a value to a string
at(list num) # Accesses an element inside of a list
set(list num any)  # Sets an element in a list with a new value
append(list any)  # Adds an element to a list
pop(list num)  # Removes and returns an element from a list
```
# Anything Else?
Nope, just enjoy the debugging nightmare if you attempt using this language :)
