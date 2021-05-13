# Lesson 2

This lesson will teach us how to to decide between two possibilities using a new 
keyword **if**.

## Your task

Write a new program that also can add numbers (+), but also can subtract numbers (-).
* ask the user if he wants to do addition (+) or subtraction(-) using printf and scanf
* the scanf for the user input needs a *char* to store the answer
* use the **if** keyword to program both addition and subtraction
* show the result to the user using printf
  
## Examples of scanf with characters and if

```
char answer = 0x00;        // this creates a places to save a character in
scanf( "%c", &answer );    // this reads the answer (+ or -) when the user types it
                           // into the console and saves it in **answer**
```

```
int result = 0;
if( answer == '-' )
{
    // calculate subtraction
}

if( answer == '+')
{
    // calculate addition
}
```
