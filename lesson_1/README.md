# Lesson 1

In the first lesson we want to learn how to programm a
basic calculator. For this we use the C programming language.

## C programming language basics

For a simple calculator we need to know how to print and read from the commandline.

The following **functions** will be used:
- **printf()** to print text to the console
- **scanf()** to read from the console
- **main()** as a starting point for the program

The following C elements will be used:
- **if** to make a yes or no decission
- **int** to store numbers
- **char** to store characters

## How to start

Every C program starts with a function called main.

```
#include <stdio.h> // this includes printf and scanf

int main()
{
    // and here is your program

    // and return 0 tells the computer that your program
    // was successful. Any other number than 0 tells the
    // computer that there was an error.
    return 0;
}
````

## What to do in this lesson

Write a C program that tells the user what it does and then asks for two numbers and show the result for
adding (+) the numbers.
