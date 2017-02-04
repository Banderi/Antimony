# Antimony's Coding Conventions
These are the coding conventions used in the Antimony project. Please follow these guidelines if you would like to contribute to the project.

### Code styles
Lines are indented with tab characters and code blocks (curly brackets) follow the "Allman" style. Example:
```C++
int Sum(int a, int b)
{
	return a + b;
}
```
If statements and other one-word block controller keywords also have a space before them, and allow one-line conditions:
```C++
while (con == true)
{
	for (int i = 0; i < 10; i++)
	{
		if (foo.isTrue())
			con = false;
		else
			foo.setBool(true);
	}
	foo.setBool(con);
}
```

### Caseing
- Free functions follow _PascalCase_ style, while methods (member functions) follow the lower _camelCase_ style
- Variables (and constants) are written in lowercase or _snake_case_; global, unique objects (e.g. the main DirectX device context) are named in a single clear indentifier, whereas local-scope variables are generally abbreviated
- Private/protected member variables have the m_ prefix and are written in _camelCase_
- Structs/class names for generic objects follow _PascalCase_ style like free functions; inner structures like the DirectX VERTEX enumeration follow upper _SNAKE_CASE_ style
- Macro definitions (`#define`s) follow upper _SNAKE_CASE_ style for constants/enums, lowercase/lower _snake_case_ for types (recommended to use `typedef` instead) and _PascalCase_ for functions

### Comments
(Guidelines for comments are more explicative and not to be strictly followed, but it's encouraged to do so as to keep the whole thing consistent)
- Variable comments start with capital letter to aid IntelliSense
- Statement comments start without capital letter and are written in second person; blocks of short single-line statements usually have comments aligned on the same column
- Block comments all start the lines with the double slashes (`//`); multi-line commenting (`/*...*/`) is reserved for commented-out code

### Files
The order of content in the files is:
- Heading
	- Inclusion guards `#pragma once` (if header file)
	- Header inclusion
		- System headers
		- Project (internal) headers
			- `Warnings.h`
			- Corresponding header (if .cpp file)
			- All other headers
		- External headers
	- Library inclusion
- Macros
	- Namespace statements
	- Typedefs
	- Macro definitions
		- Functions
		- Constants
- Global variables forward declaration (.h) or definition (.cpp)
- Class/structs
	- Interface definition (.h) or methods implementation (.cpp)
	- Global items forward declaration (.h) or definition (.cpp)
- Functions	
	- Prototypes/functions forward declaration (.h) or implementation (.cpp)