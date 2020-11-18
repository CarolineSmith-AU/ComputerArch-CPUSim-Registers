Caroline Smith - cls0097

COMMANDS TO COMPILE AND RUN FILES:
Visual Studio Code: 
>> COMPILE
    >> Open the project files located in the respective project folder.
    >> Open the command prompt OR the Visual Studio Code terminal.
    >> Compile using the following command: g++ -std=c++11 gprSim.cpp -o gprSim

>> RUN
    >> Make sure "gprSimEncoded.txt" and "result.txt" are empty files.
    >> Select Run > Start without debugging in VS code
    >> View the program output in the debug console. View resulting speedup calculations in "result.txt"
    >> To view the output for various string inputs, change the "string_space" string value in "palindrome.s"
       >>> I did find time to add the functionality for prompting the user for a string input instead of 
           having to manually change the string in the code, but it involves amore involved explanation  and 
           since I had some trouble with compilation when I turned in my first project, I decided to go simpler.

//--------------------------------------------------------------------------------------------------------------------------

HOW THE SYSTEM WORKS
>> The "gprSim.cpp" file uses the multiple register-based implementation to detect if a given string is
   a palindrome or not. It reads from the "palindrome.s" file for the souorce code and writes the binary encoded instructions to 
   "gprCodeEncoded.txt".

>> Just like in project 1, I used string arrays as my data structures for the stack, data and text memory segments. Each array is 
   connected to a data segment by typedef constants that are set to the base address I assigned to each segment.

>> The way that binary encoded data is stored is in little endian order, so the code I wrote is lengthy to accomodate
   for the level of detail I wanted my memory segments to implement. See the Helper Functions section in my code
   to see the functions I wrote to assist in handling binary encoded data.

>> The output that is printed in the debug console contains information about the various instructions that are being executed
   so that you can see the actions the simulator is taking as it is executing the encoded instructions.