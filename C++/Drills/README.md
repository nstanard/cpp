# C++ Drills

Practice drills to memorize C++ syntax and ecosystem.

## How to Use

1. Read a drill task below
2. Create the file and write the code from memory
3. Run the verify script to automatically compile, run, and check all your drills:
   - **Bash:** `./verify.sh` (or `./verify.sh 01` for just drill 01)
   - **PowerShell:** `.\verify.ps1` (or `.\verify.ps1 -drill 01` for just drill 01)
4. Run the clear script to wipe your work and start fresh:
   - **Bash:** `./clear.sh`
   - **PowerShell:** `.\clear.ps1`

**Manual testing:** You can also compile/run manually with `g++ drill01.cpp -o drill01.exe` then `./drill01.exe`

---

## Drill 01: Hello World
**File:** `drill01.cpp`

Write a program that:
- Includes iostream
- Uses the std namespace
- Prints "Hello World!" to console
- Returns 0

**Expected output:** `Hello World!`

---

## Drill 02: Variables and Output
**File:** `drill02.cpp`

Write a program that:
- Declares an int variable called `age` with value 25
- Declares a string variable called `name` with value "Alice"
- Prints: `My name is Alice and I am 25 years old.`

**Expected output:** `My name is Alice and I am 25 years old.`

---

## Drill 03: User Input
**File:** `drill03.cpp`

Write a program that:
- Asks the user: `What is your name? `
- Reads their input into a string variable
- Prints: `Hello, [name]!`

**Expected output:** 
```
What is your name? Bob
Hello, Bob!
```

---

## Drill 04: Basic Math
**File:** `drill04.cpp`

Write a program that:
- Declares two int variables: `a = 10` and `b = 3`
- Calculates and prints their sum, difference, product, and quotient
- Each on a separate line with labels

**Expected output:**
```
Sum: 13
Difference: 7
Product: 30
Quotient: 3
```

---

## Drill 05: If Statement
**File:** `drill05.cpp`

Write a program that:
- Declares an int variable `score = 85`
- Uses if/else to print "Pass" if score >= 60, otherwise "Fail"

**Expected output:** `Pass`

---

## Drill 06: For Loop
**File:** `drill06.cpp`

Write a program that:
- Uses a for loop to print numbers 1 through 5
- Each number on its own line

**Expected output:**
```
1
2
3
4
5
```

---

## Drill 07: While Loop
**File:** `drill07.cpp`

Write a program that:
- Uses a while loop to count down from 5 to 1
- Prints each number on its own line
- After the loop, prints "Blast off!"

**Expected output:**
```
5
4
3
2
1
Blast off!
```

---

## Drill 08: Array
**File:** `drill08.cpp`

Write a program that:
- Declares an array of 5 integers: {2, 4, 6, 8, 10}
- Uses a for loop to print each element

**Expected output:**
```
2
4
6
8
10
```

---

## Drill 09: Function
**File:** `drill09.cpp`

Write a program that:
- Defines a function `int multiply(int x, int y)` that returns x * y
- In main(), calls the function with 7 and 6
- Prints the result

**Expected output:** `42`

---

## Drill 10: Function with Void
**File:** `drill10.cpp`

Write a program that:
- Defines a void function `void greet(string name)` that prints "Hello, [name]!"
- In main(), calls it with "World"

**Expected output:** `Hello, World!`
