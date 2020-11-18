#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

//Caroline Smith
//Project 1 - Accumulator based machine

using namespace std;

//function declerations
string readFromMem(int offset, string *memArray, int sizeInBytes);
string readStringFromMem(int offset, string *memArray);
void writeDataToMem(int offset, string hexValue, string *memArrayPointer, int sizeInBytes);
void writeStringToMem(int offset, string hexValue, string *memArrayPointer);
string eraseFromMem(int offset, string *memArrayPointer, int sizeInBytes);
string hexToDec(string hexVal);
string decToHex(int decVal, int numToPrepend);
string prependZeros(string value, int numToPrepend);
string stringToASCII(const string& value);
void initDataMemory(ifstream& inStream, string *dataArrayPointer);
void initTextMemory(ifstream& inStream, string *textArrayPointer);
string trimWhiteSpace(string stringToTrim, string charOfChoice);
string trimComment(string stringToTrim);
void disassembleData(ifstream& inStream, ofstream& outStream, map<string, string> &addMap, map<string, string> &sizeMap);
void disassembleText(ifstream& inStream, ofstream& outStream, map<string, string> &addMap, map<string, string> &sizeMap);
void disassembleCodeLabelSegments(ifstream& inStream, ofstream& outStream, map<string, string> &addMap, map<string, string> &sizeMap);
void findAllCodeLabelSegments(ifstream& inStream, map<string, string> &addMap, map<string, string> &sizeMap);
string encodeADDIData(string instruction);
string encodeBData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap);
string encodeBEQZData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap);
string encodeBGEData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap);
string encodeBNEData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap);
string encodeLAData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap);
string encodeLBData(string instruction);
string encodeLIData(string instruction);
string encodeSUBIData(string instruction);
string encodeSYSCALLData(string instruction);
void executeCode(string *dataArrayPointer, string *textArrayPointer, int& accumulator);
int determineReg(string reg);
int getValueOfReg(int reg);
int setReg(int reg, int value, string action);
bool compareVal(int val1, int val2, string action);
void addi(int regDest, int regSource, int immVal);
void b(string address);
bool beqz(string address, int reg);
bool bge(string address, int regDest, int regSource);
bool bne(string address, int regDest, int regSource);
void la(string address, int reg);
void lb(int regDest, int regSource, string *dataArrayPointer);
void li(int regDest, int value);
void subi(int regDest, int regSource, int immVal);
bool syscall(string address, string *dataArrayPointer);
string fetchInstruction(string *memArray);
string memoryAccess(int offset, string *memArray, int sizeInBytes, string action, string value);
void decodeInstructionAndRegFetch();
int aluArith(int reg, int immVal, string action);
bool aluComp(int reg1, int reg2, string action);
int wb(int reg, int immVal, string action);
void setPC(int offset);

//------------------------------Typedefs------------------------------
//Bit sizing
typedef unsigned int uint8;   //8-bit unsigned integer
typedef unsigned int uint16;  //16-bit unsigned integer
typedef unsigned int uint32;  //32-bit unsigned integer
typedef int int32;            //32-bit signed integer
typedef unsigned int uint48;  //48-bit unsigned integer (size of instruction stored in memory)

//Byte sizing
const uint8 Hex_Digits_In_Byte = 2;   //2 hex digits per 1 byte
const uint16 Hex_Digits_In_Word = 4;  //4 hex digits per 2 byte
const uint32 Hex_Digits_In_DWord = 8; //8 hex digits per 4 byte

//Instruction and stack pointer typedefs
typedef unsigned int Instruction_Pointer;

//Instruction opcode names
typedef uint8 Instruction_Opcode;
typedef uint8 Syscall_Code;

//Memory segment base address
typedef uint32 Segement_Base_Address;

//Size of instruction in memory
typedef uint48 Mem_Item_Size;

//Register
typedef uint8 Register;

//Defining typedef constants
const Instruction_Opcode Code_Label = 0;
const Instruction_Opcode ADDI = 1;
const Instruction_Opcode B = 2;
const Instruction_Opcode BEQZ = 3;
const Instruction_Opcode BGE = 4;
const Instruction_Opcode BNE = 5;
const Instruction_Opcode LA = 6;
const Instruction_Opcode LB = 7;
const Instruction_Opcode LI = 8;
const Instruction_Opcode SUBI = 9;
const Instruction_Opcode SYSCALL = 10;
const Instruction_Opcode End = 11;
const Instruction_Opcode No_Reg_Involved = 65535;
const Instruction_Opcode No_Imm_Involved = 65535;
const Instruction_Opcode No_Addr_Involved = 4294967295;
const Instruction_Opcode No_Size_Involved = 65535;
const Instruction_Opcode Allocate_Space = 255;
const Segement_Base_Address Data = 512;  //HexVal = 0200h
const Segement_Base_Address Text = 513;  //HexVal = 0201h
const Mem_Item_Size Instruction_Size = 15; 

//Syscall $v0 codes
const Syscall_Code print_string = 4;
const Syscall_Code terminate_execution = 10;
const Syscall_Code read_string = 8;

//Registers
Register $0 = 0; //zero
Register $1 = 0; //at
Register $2 = 0; //v0
Register $3 = 0; //v1
Register $4 = 0; //a0
Register $5 = 0; //a1
Register $6 = 0; //a2
Register $7 = 0; //a3
Register $8 = 0; //t0
Register $9 = 0; //t1
Register $10 = 0; //t2
Register $11 = 0; //t3
Register $12 = 0; //t4
Register $13 = 0; //t5
Register $14 = 0; //t6
Register $15 = 0; //t7
Register $16 = 0; //s0
Register $17 = 0; //s1
Register $18 = 0; //s2
Register $19 = 0; //s3
Register $20 = 0; //s4
Register $21 = 0; //s5
Register $22 = 0; //s6
Register $23 = 0; //s7
Register $24 = 0; //t8
Register $25 = 0; //t9
Register $26 = 0; //k0
Register $27 = 0; //k1
Register $28 = 0; //gp
Register $29 = 0; //sp
Register $30 = 0; //fp
Register $31 = 0; //ra

//Relevant files
const string SOURCE_FILE = "palindrome.s";
const string ENCODED_FILE = "gprCodeEncoded.txt";
const string RESULT_FILE = "result.txt";

//Program size variables
int InstructionsExecuted = 0;
int ProgramClockCycles = 0;

const int Text_Seg_Branch_Size = 1000;
const string whitespace = " \n\r\t\f\v";
Instruction_Pointer pc;

int main() {
   //String array typedefs to represent memory segments
   typedef string Data_Segment[20000];
   typedef string Text_Segment[20000];

   //Accumulator
   int32 Accumulator = 0;

   //Empty(uninitialized) memory array declarations
   Data_Segment dataArray;
   Text_Segment textArray;

   //HashMaps
   map<string, string> addMap;  //maps labels to hex value addresses represented as strings
   map<string, string> sizeMap;    //maps labels to their sizes (in bytes)

    //Read source code and translate to binary
    ifstream inStream;
    inStream.open(SOURCE_FILE.c_str(), std::ifstream::in);
    if (inStream.fail()) {
		cout << "Failed opening file." << endl;
		exit(1);
	}
    ofstream outStream;
    outStream.open(ENCODED_FILE.c_str(), ios::app);
    disassembleData(inStream, outStream, addMap, sizeMap);
    inStream.clear();
    inStream.seekg(0, ios::beg);
    disassembleText(inStream, outStream, addMap, sizeMap);
    outStream.close();
    inStream.close();

    //Read disassembled code and load memory
    inStream.open(ENCODED_FILE.c_str(), std::ifstream::in);
    if (inStream.fail()) {
		cout << "Failed to opening file." << endl;
		exit(1);
	}
    initDataMemory(inStream, dataArray);
    initTextMemory(inStream, textArray);
    inStream.close();

    //Execute code instructions
    executeCode(dataArray, textArray, Accumulator);

    outStream.open(RESULT_FILE.c_str(), ios::app);
    outStream << "Number of instructions executed in program: " << InstructionsExecuted << endl;
    outStream << "Number of clock cycles: " << ProgramClockCycles << endl;
    double speedup = (8 * InstructionsExecuted) / (float)ProgramClockCycles;
    // std::ofstream << std::fixed;
    // std::ofstream << std::setprecision(2);
    outStream << "Calculated speedup of multicycle processor: " << speedup << "\n" << endl;


    cout << "--------------------------------- Program Statistics ---------------------------------\n" << endl;
    cout << "Number of instructions executed in program: " << InstructionsExecuted << "\n" << endl;
    cout << "Number of clock cycles: " << ProgramClockCycles << "\n" << endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    cout << "Calculated speedup of multicycle processor: " << speedup << "\n" << endl;

    return 0;
}

//***Returns the hexadecimal value data located at specified address
//***PARAMS: offset - decimal offset of data in memory segment
//           *memArray - the array corresponding to the memory segment data is stored in
//           sizeInBytes - number of bytes data holds in memory 
string readFromMem(int offset, string *memArray, int sizeInBytes) {
    string dataString = "";
    int MSBPosition = offset + (sizeInBytes - 1);
    for (int i = MSBPosition; i >= offset; i--) {
        dataString += memArray[i];
    }
    return dataString;
}

string readStringFromMem(int offset, string *memArray) {
    string dataString = "";
    int i = offset;
    while (memArray[i] != "00") {
        string val = memArray[i];
        int asciiVal = stoi(hexToDec(memArray[i]));
        char asciiChar = asciiVal;
        dataString += asciiChar;
        i++;
    }
    return dataString;
}

//***Writes data to memory using little endien order
//***PARAMS: offset - decimal offset of data in memory segment
//           hexValue - hexadecimal value to be stored in memory
//           *memArrayPointer - pointer to the corresponding memory array
//           sizeInBytes - number of bytes data holds in memory 
void writeDataToMem(int offset, string hexVal, string *mArrayPointer, int sizeInBytes) {
    int count = 0;
    int MSBPosition = offset + (sizeInBytes - 1);
    for (int i = MSBPosition; i >= offset; i--) {
        mArrayPointer[i] = hexVal.substr(count, Hex_Digits_In_Byte);
        count += 2;
    }
}

void writeStringToMem(int offset, string hexValue, string *memArrayPointer) {
    int count = 0;
    for (int i = offset; count <= hexValue.length(); i++) {
        memArrayPointer[i] = hexValue.substr(count, Hex_Digits_In_Byte);
        count += 2;
    }
}

//***Erases data from specified memory address
//***PARAMS: offset - decimal offset of data in memory segment
//           *memArrayPointer - the array corresponding to the memory segment data is stored in
//           sizeInBytes - number of bytes to erase
string eraseFromMem(int offset, string *memArrayPointer, int sizeInBytes) {
    string erasedVal;
    for (int i = sizeInBytes; i > 0; i--) {
        erasedVal = erasedVal.insert(0, memArrayPointer[offset]);
        memArrayPointer[offset] = "";
        offset++;
    }
    return erasedVal;
}

//***Uses binary encoded file to initialize data memory segment
//***PARAMS: instream - an open ifstream
//           *dataArrayPointer - the array corresponding to the memory segment data is stored in
 void initDataMemory(ifstream& inStream, string *dataArrayPointer) {
     string line = "";
     int memOffset; //memory offset from base address
     string hexValueDataSegment = decToHex(Data, 4);
     string hexValueEndInstr = decToHex(End, 2) + "0000000000000000000000000000";

     //Various string positions
     const int startOfSizeByte = 0;
     const int endOfSizeByte = 2;
     const int startOfAddr = 2;
     const int endOfAddr = 8;
     const int startOfValue = 10;
     const int startOfOffset = 6;
     const int endOfOffset = 4;

    while (line.compare(hexValueDataSegment) != 0) { //loop until ".data" segment is found
         getline(inStream, line);
         line = trimWhiteSpace(line, whitespace);
     }
     if (inStream.eof()) { //".data" section was not found in file
         cout << "End of file reached. .data section not found.";
     } else { //.data segment found
         getline(inStream, line);
         line = trimWhiteSpace(line, whitespace);
         while (line.compare(hexValueEndInstr) != 0) { //loop through ".data" segment until ".text" or "eof" is reached
             if (line.compare("") != 0) {
                 int sizeInBytes;
                 string address;
                 string value;

                 //Get data from encoded string
                 sizeInBytes = stoi(hexToDec(line.substr(startOfSizeByte, endOfSizeByte)));
                 address = line.substr(startOfAddr, endOfAddr);
                 value = line.substr(startOfValue);
                 memOffset = stoi(hexToDec(address.substr(startOfOffset, endOfOffset)));

                 if (sizeInBytes == Allocate_Space) {
                     sizeInBytes = stoi(hexToDec(value));
                     value = prependZeros("", sizeInBytes * 2);
                 } else if (sizeInBytes > 8) {
                     //Save string to data segment of memory
                     writeStringToMem(memOffset, value, dataArrayPointer);
                 } else {
                    //Save value to data segment of memory
                    writeDataToMem(memOffset, value, dataArrayPointer, sizeInBytes);
                 }
             }
             getline(inStream, line);
             line = trimWhiteSpace(line, whitespace);
         }
     }
 }

//***Returns number of instructions after reading binary encoded file and initializinng text memory segment
//***PARAMS: instream - an open ifstream
//           *textArrayPointer - the array corresponding to the memory segment data is stored in
 void initTextMemory(ifstream& inStream, string *textArrayPointer) {
     int instrCount = 0;
     string line = "";
     int opcode;
     int size;
     int offset;
     string hexValueTextSegment = decToHex(Text, 4);
     string hexValueEndInstr = decToHex(End, 2) + "0000000000000000000000000000";

     //Various string positions
     const int startOfOpcode = 0;
     const int endOfOpcode = 2;
     const int startOfSize = 2; 
     const int endOfSize = 4;
     const int startOfAddress = 18;
     const int endOfAddress = 8;
     const int startOfOffset = 22;
     const int endOfOffset = 4;

    while (line.compare(hexValueTextSegment) != 0) { //loop until ".text" segment is found
         getline(inStream, line);
         line = trimWhiteSpace(line, whitespace);
     }
     if (inStream.eof()) { //".text" section was not found in file
         cout << "End of file reached. .DATA section not found.";
     } else { //.text segment found
         getline(inStream, line);
         line = trimWhiteSpace(line, whitespace);
         while (line.compare(hexValueEndInstr) != 0) { //loop through ".text" segment until END is reached
             if (line.compare("") != 0) {
                 opcode = stoi(hexToDec(line.substr(startOfOpcode, endOfOpcode)));
                 size = stoi(hexToDec(line.substr(startOfSize, endOfSize)));
                 offset = stoi(hexToDec(line.substr(startOfOffset, endOfOffset)));
                 if (opcode == Code_Label) {
                     writeDataToMem(offset, line, textArrayPointer, Instruction_Size);
                     getline(inStream, line);
                     offset += Instruction_Size;
                     line = trimWhiteSpace(line, whitespace);
                     while (size != 0) {
                         writeDataToMem(offset, line, textArrayPointer, Instruction_Size);  

                         offset += Instruction_Size;
                         size--;

                         getline(inStream, line);
                         line = trimWhiteSpace(line, whitespace);
                     }
                 }        
             }
         }
         writeDataToMem(offset, line, textArrayPointer, Instruction_Size);
     }
 }

//***Reads ASCII source code file to create data segment of encoded file
//***PARAMS: instream - an open ifstream
//           outStream - an open ofstream
//           &addMap - map of labels and corresponding addresses
//           &sizeMap - map of labels and corresponding sizes (in bytes)
 void disassembleData(ifstream& inStream, ofstream& outStream, map<string, string> &addMap, map<string, string> &sizeMap) {
     string line = "";
     int memOffset = 0; //memory offset from base address

     while (line.compare(".data") != 0) { //loop until ".data" segment is found
         getline(inStream, line);
         //trim end of line comments and surrounding whitespace
         line = trimComment(line);
         line = trimWhiteSpace(line, whitespace);
     }
     if (inStream.eof()) { //".data" section was not found in file
         cout << "End of file reached. .DATA section not found.";
     } else { //.data segment found
         outStream << decToHex(Data, 4) + "\n";
         getline(inStream, line);
         //trim end of line comments and surrounding whitespace
         line = trimComment(line);
         line = trimWhiteSpace(line, whitespace);
         while (line.compare("END") != 0) { //loop through ".data" segment until ".text" or "eof" is reached
             if (line.compare("") != 0 && line.find_first_of("#") != 0) {
                 string label;
                 string address;
                 string sizeString;
                 string size;
                 string value;
                 string encodedData;

                 //trim end of line comments and surrounding whitespace
                 line = trimComment(line);
                 line = trimWhiteSpace(line, whitespace);

                 //Get memory address label 
                 label = line.substr(0, line.find_first_of(":"));
                 label = trimWhiteSpace(label, whitespace);
                 address = decToHex(Data, 4) + decToHex(memOffset, 4);
                 addMap[label] = address;

                 //Get size of label in bytes
                 sizeString = line.substr(line.find_first_of(":") + 1, line.length());
                 sizeString = trimWhiteSpace(sizeString, whitespace);
                 value = sizeString.substr(sizeString.find_first_of(whitespace) + 1, sizeString.find_last_not_of(whitespace) + 1);
                 value = trimWhiteSpace(value, whitespace);

                 sizeString = sizeString.substr(0, sizeString.find_first_of(whitespace));
                 if (sizeString.compare(".asciiz") == 0) {
                    value = value.substr(value.find_first_of("\"") + 1, value.find_last_of("\"") - 1);
                    value = trimWhiteSpace(value, whitespace);
                 }

                 if (sizeString.compare(".byte") == 0) { //byte => 1 byte
                     size = "1";
                     sizeMap[label] = prependZeros(size, 3);
                 } else if (sizeString.compare(".word") == 0) { //word => 2 bytes
                     size = "2";
                     sizeMap[label] = prependZeros(size, 3);
                 } else if (sizeString.compare(".dword") == 0) { //doubleword => 4 bytes
                     size = "4";
                     sizeMap[label] = prependZeros(size, 3);
                 } else if (sizeString.compare(".asciiz") == 0) { //asciiz => 1 * <string_length> bytes
                     size = to_string(1 * value.length());
                     sizeMap[label] = decToHex(stoi(size), 4);
                 } else { //space => reserve <value> number of bytes in memory
                     size = value;
                     sizeMap[label] = decToHex(stoi(value), 4);
                 }
                if (value.compare("?") == 0) {
                    encodedData = prependZeros(size, 1) + address + decToHex(0, 4);
                } else if (sizeString.compare(".asciiz") == 0) {
                    value = stringToASCII(value);
                    encodedData = decToHex(stoi(size), 2) + address + value;
                } else if (sizeString.compare(".space") == 0) {
                    encodedData = "ff" + address + decToHex(stoi(value), Hex_Digits_In_DWord);
                } else {
                     encodedData = prependZeros(size, 1) + address + decToHex(stoi(value), Hex_Digits_In_DWord);
                     cout << encodedData << endl;
                }
                outStream << encodedData + "\n";
                memOffset += stoi(size) + 1;
             }
             getline(inStream, line);
             //trim end of line comments and surrounding whitespace
             line = trimComment(line);
             line = trimWhiteSpace(line, whitespace);
         }
         outStream << decToHex(End, 2) + "0000000000000000000000000000" + "\n";
     }
 }

//***Reads ASCII source code file to creaate text segment of encoded file
//***PARAMS: instream - an open ifstream
//           outStream - an open ofstream
//           &addMap - map of labels and corresponding addresses
//           &sizeMap - map of labels and corresponding sizes (in bytes)
 void disassembleText(ifstream& inStream, ofstream& outStream, map<string, string> &addMap, map<string, string> &sizeMap) {
     string line = "";
     string hexValueTextSegment = decToHex(Text, 4);

     while (line.compare(".text") != 0) { //loop until ".text" segment is found
         getline(inStream, line);
         //trim end of line comments and surrounding whitespace
         line = trimComment(line);
         line = trimWhiteSpace(line, whitespace);
     }
     if (inStream.eof()) { //".text" section was not found in file
         cout << "End of file reached - .text section not found.";
     } else { //.text segment found
         outStream << hexValueTextSegment + "\n";

        findAllCodeLabelSegments(inStream, addMap, sizeMap);

        inStream.clear(); //reset ifstream to beginning of file
        inStream.seekg(0, ios::beg);

         getline(inStream, line);
         //trim end of line comments and surrounding whitespace
         line = trimComment(line);
         line = trimWhiteSpace(line, whitespace);

         while (line.compare(".text") != 0) { //loop until ".text" segment is found
            getline(inStream, line);
            //trim end of line comments and surrounding whitespace
            line = trimComment(line);
            line = trimWhiteSpace(line, whitespace);
         }
         disassembleCodeLabelSegments(inStream, outStream, addMap, sizeMap);
     }
 }

 void disassembleCodeLabelSegments(ifstream& inStream, ofstream& outStream, map<string, string> &addMap, map<string, string> &sizeMap) {
     string line;
     string label;
     string instruction;
     string encodedData;
     string codeLabelAddress;
     string instrAddress;
     int memOffset = 0;
     int codeLabelOffset;

     string hexValueTextSegment = decToHex(Text, 4);

     while (line.compare("END") != 0) { //loop through ".text" segment until END is reached
             if (line.compare("") != 0 && line.find_first_of("#") != 0) { //skip over blank lines and comments
                int indexOfColon = line.find_first_of(":");
                if (indexOfColon >= 0) {
                    label = line.substr(0, line.find_first_of(":"));
                    label = trimWhiteSpace(label, whitespace);
                    codeLabelAddress = hexValueTextSegment + decToHex(memOffset, 4);
                    addMap[label] = codeLabelAddress;
                    
                    encodedData = decToHex(Code_Label, 2) + sizeMap[label] + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + codeLabelAddress + decToHex(No_Imm_Involved, 4);
                    outStream << encodedData + "\n";
                } else {
                    instruction = line.substr(0, line.find_first_of(whitespace));
                    if (instruction.compare("addi") == 0) {
                        encodedData = encodeADDIData(line);
                    } else if (instruction.compare("b") == 0) {
                        encodedData = encodeBData(line, addMap, sizeMap);
                    } else if (instruction.compare("beqz") == 0) {
                        encodedData = encodeBEQZData(line, addMap, sizeMap);
                    } else if (instruction.compare("bge") == 0) {
                        encodedData = encodeBGEData(line, addMap, sizeMap);
                    } else if (instruction.compare("bne") == 0) {
                        encodedData = encodeBNEData(line, addMap, sizeMap);
                    } else if (instruction.compare("la") == 0) {
                        encodedData = encodeLAData(line, addMap, sizeMap);
                    } else if (instruction.compare("lb") == 0) {
                        encodedData = encodeLBData(line);
                    } else if (instruction.compare("li") == 0) {
                        encodedData = encodeLIData(line);
                    } else if (instruction.compare("subi") == 0) {
                        encodedData = encodeSUBIData(line);
                    } else if (instruction.compare("syscall") == 0) {
                        encodedData = encodeSYSCALLData(line);
                    }
                    outStream << encodedData + "\n";                
                }
                memOffset += Instruction_Size;
             }
             getline(inStream, line);
             //trim end of line comments and surrounding whitespace
             line = trimComment(line);
             line = trimWhiteSpace(line, whitespace);
         }
         outStream << decToHex(End, 2) + "0000000000000000000000000000" + "\n";
 }

 void findAllCodeLabelSegments(ifstream& inStream, map<string, string> &addMap, map<string, string> &sizeMap) {
         string line;
         string address;
         string label;
         int memOffset = 0;
         int instrCount = 0;
         string hexValueTextSegment = decToHex(Text, 4);
         
         getline(inStream, line);
         //trim end of line comments and surrounding whitespace
         line = trimComment(line);
         line = trimWhiteSpace(line, whitespace);

         while (line.compare("END") != 0) { //check if not end of line
            if (line.compare("") != 0 && line.find_first_of("#") != 0) {
                int indexOfColon = line.find_first_of(":");
                if (indexOfColon >= 0) { //find all code labels in ".text" section
                    if (label.length() > 0) {
                        //holds instruction count for prev code segment
                        sizeMap[label] = decToHex(instrCount, 4);
                        string size = sizeMap[label];
                        instrCount = 0;
                    }

                    label = line.substr(0, line.find_first_of(":"));
                    label = trimWhiteSpace(label, whitespace);

                    address = hexValueTextSegment + decToHex(memOffset, 4);
                    addMap[label] = address;
                } else {
                    instrCount++;
                }
                memOffset += Instruction_Size;
            }
            getline(inStream, line);
            //trim end of line comments and surrounding whitespace
            line = trimComment(line);
            line = trimWhiteSpace(line, whitespace);    
         }
         if (label.length() > 0) {
            //holds instruction count for prev code segment
            sizeMap[label] = decToHex(instrCount, 4);
            string size = sizeMap[label];
            memOffset -= (instrCount + 1) * Instruction_Size;
            address = hexValueTextSegment + decToHex(memOffset, 4);
            addMap[label] = address;
        }
 }

//***Executes code using accumulator implementation
//***PARAMS: Instruction_Count - holds number of instructions to execute
//           *dataArrayPointer - references data memory segment array
//           *textArrayPointer - references text memory segment array
//           &accumulator - holds the value of the accumulator
 void executeCode(string *dataArrayPointer, string *textArrayPointer, int& accumulator) {     
     const int Mem_Offset_Opcode = 14;      //the offset of opcode byte from instruction pointer (pc)
     const int Mem_Offset_Size = 12;        //the offset of size byte from instruction pointer (pc)
     const int Mem_Offset_Base_Address = 4; //the offset of base address from instruction pointer (pc)
     const int Mem_Offset_Offset = 2;       //the offset of memory offset from instruction pointer (pc)
     const int Mem_Offset_Imm_Val = 0;      //the offset of memory offset from instruction pointer (pc)
     const int Mem_Offset_Reg_Dest = 10;    //the offset of memory offset from instruction pointer (pc)
     const int Mem_Offset_Reg_Source_1 = 8; //the offset of memory offset from instruction pointer (pc)
     const int Mem_Offset_Reg_Source_2 = 6; //the offset of memory offset from instruction pointer (pc)

     string hexValueEndInstr = decToHex(End, 2) + "0000000000000000000000000000";

     int opCode;
     int byteSize;
     int baseAddress;
     int offset;
     int immVal;
     int regDest;
     int regSource1;
     int regSource2;
     string address;

     //Fetch instruction
     string fullInstruction = fetchInstruction(textArrayPointer);
     
     while (fullInstruction.compare(hexValueEndInstr) != 0) {
        //Decode Instruction (Register fetch included in this clock cycle)
        opCode = stoi(hexToDec(readFromMem(pc + Mem_Offset_Opcode, textArrayPointer, 1)));
        byteSize = stoi(hexToDec(readFromMem(pc + Mem_Offset_Size, textArrayPointer, 2)));
        baseAddress = stoi(hexToDec(readFromMem(pc + Mem_Offset_Base_Address, textArrayPointer, 2)));
        offset = stoi(hexToDec(readFromMem(pc + Mem_Offset_Offset, textArrayPointer, 2)));
        immVal = stoi(hexToDec(readFromMem(pc + Mem_Offset_Imm_Val, textArrayPointer, 2)));
        regDest = stoi(hexToDec(readFromMem(pc + Mem_Offset_Reg_Dest, textArrayPointer, 2)));
        regSource1 = stoi(hexToDec(readFromMem(pc + Mem_Offset_Reg_Source_1, textArrayPointer, 2)));
        regSource2 = stoi(hexToDec(readFromMem(pc + Mem_Offset_Reg_Source_2, textArrayPointer, 2)));
        address = readFromMem(pc + Mem_Offset_Offset, textArrayPointer, 4);
        decodeInstructionAndRegFetch();

         //Decode instruction opcode
         switch (opCode) {
         case Code_Label:
            pc = offset + Instruction_Size;
            fullInstruction = readFromMem(pc, textArrayPointer, Instruction_Size);
            continue;
         case ADDI:
             addi(regDest, regSource1, immVal);
             break;
         case B:
             b(address);
             fullInstruction = fetchInstruction(textArrayPointer);
             continue;
         case BEQZ:
            if (beqz(address, regDest) == true) {
                fullInstruction = fetchInstruction(textArrayPointer);
                continue;
            } else {
                break;
            }
         case BGE:
            if (bge(address, regDest, regSource1) == true) {
                fullInstruction = fetchInstruction(textArrayPointer);
                continue;
            } else {
                break;
            }
         case BNE:
            if (bne(address, regDest, regSource1) == true) {
                fullInstruction = fetchInstruction(textArrayPointer);
                continue;
            } else {
                break;
            }
         case LA:
            la(address, regDest);
            break;
         case LB:
            lb(regDest, regSource1, dataArrayPointer);
            break;
         case LI:
            li(regDest, immVal);
            break;
         case SYSCALL:
            if (syscall(address, dataArrayPointer) == false) {
                cout << "Terminating execution - goodbye cruel world." << endl;
            }
            break;
         case SUBI:
            subi(regDest, regSource1, immVal);
            break;
         default:
             cout << "Unknown instruction opcode\n" << endl;  
             break;
         }
         InstructionsExecuted++;
         pc += Instruction_Size;
         fullInstruction = fetchInstruction(textArrayPointer);
     }
 }

//----------------------------------------Instruction Methods----------------------------------------//

//***Adds an immediate value to a register and stores result in destination register
void addi(int regDest, int regSource, int immVal) {
    int calculation = aluArith(regSource, immVal, "add");
    int load = wb(regDest, calculation, "store");
}

void b(string address) {
    int offset = stoi(hexToDec(address.substr(4)));
    setPC(offset);
    cout << "Branching instruction pointer to code segment at address " + address + "...\n" << endl;
}

bool beqz(string address, int reg) {
    int val = getValueOfReg(reg);
    if (aluComp(val, -1, "equalToZero") == true) {
        int offset = stoi(hexToDec(address.substr(4)));
        setPC(offset);
        cout << "Register $" + to_string(reg) + " is equal to zero - branching instruction pointer to code segment at address " + address + "...\n" << endl;
        return true;
    } else {
        cout << "Register $" + to_string(reg) + " is not equal to zero - continuing with next instruction...\n" << endl;
        return false;
    }
}

bool bge(string address, int regDest, int regSource) {
    int val1 = getValueOfReg(regDest);
    int val2 = getValueOfReg(regSource);
    if (aluComp(val1, val2, "greaterThanEqualTo") == true) {
        int offset = stoi(hexToDec(address.substr(4)));
        setPC(offset);
        cout << "Register $" + to_string(regDest) + " is greater than or equal to register $" + to_string(regSource) + " - branching instruction pointer to code segment at address " + address + "...\n" << endl;
        return true;
    } else {
        cout << "Register $" + to_string(regDest) + " is less than register $" + to_string(regSource) + " - continuing with next instruction...\n" << endl;
        return false;
    }
}

bool bne(string address, int regDest, int regSource) {
    int val1 = getValueOfReg(regDest);
    int val2 = getValueOfReg(regSource);
    if (aluComp(val1, val2, "notEqual") == true) {
        int offset = stoi(hexToDec(address.substr(4)));
        setPC(offset);
        cout << "Register $" + to_string(regDest) + " is not equal to register $" + to_string(regSource) + " - branching instruction pointer to code segment at address " + address + "...\n" << endl;
        return true;
    } else {
        cout << "Register $" + to_string(regDest) + " is equal to register $" + to_string(regSource) + " - continuing with next instruction...\n" << endl;
        return false;
    }
}

void la(string address, int reg) {
    int offset = stoi(hexToDec(address.substr(4)));
    int calculation = wb(reg, offset, "load");
}

void lb(int regDest, int regSource, string *dataArrayPointer) {
    int offset = getValueOfReg(regSource);
    int offsetCalc = aluArith(0, 0, "add"); //Fake instruction to make sure alu clock cycle calculated
    int contentAtOffset = stoi(hexToDec(memoryAccess(offset, dataArrayPointer, 1, "readFromMem", "")));
    int calculation = wb(regDest, contentAtOffset, "load");
}

void li(int regDest, int value) {
    int calculation = wb(regDest, value, "load");
}

void subi(int regDest, int regSource, int immVal) {
    int calculation = aluArith(regSource, immVal, "subtract");
    int load = wb(regDest, calculation, "store");
}

bool syscall(string address, string *dataArrayPointer) {
    string asciiString;
    if (aluComp($2, print_string, "equal") == true) {
        int offset = $4;
        asciiString = memoryAccess(offset, dataArrayPointer, -1, "readStringFromMem", "");
        cout << asciiString << endl;
        return true;
    } else if (aluComp($2, terminate_execution, "equal") == true) {
        return false;
    } else if (aluComp($2, read_string, "equal") == true) {
        cout << "***This is a program that checks if a string is a palindrome.***" << endl;
        cout << "Enter a string: ";
	    cin >> asciiString;
        asciiString = stringToASCII(asciiString);
        memoryAccess($4, dataArrayPointer, -1, "writeStringToMemory", asciiString);
        return true;
    } else {
        cout << "Unknown syscall code.\n" << endl;
        return false;
    }
}

//----------------------------------------Encoding Methods----------------------------------------//

string encodeADDIData(string instruction) {
    string regDestination;
    string regSource1;
    string encodedData;
    string bufferString;
    int immVal;

    //Parse line for relevant data
    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regDestination = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regDestination = trimWhiteSpace(regDestination, whitespace);

    bufferString = bufferString.substr(bufferString.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regSource1 = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regSource1 = trimWhiteSpace(regSource1, whitespace);

    bufferString = bufferString.substr(bufferString.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    immVal = stoi(bufferString.substr(bufferString.find_first_not_of(whitespace)));

    return decToHex(ADDI, 2) + decToHex(No_Size_Involved, 4) + decToHex(determineReg(regDestination), 4) + decToHex(determineReg(regSource1), 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Addr_Involved, 8) + decToHex(immVal, 4);
}

string encodeBData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap) {
    string label;

    //Parse line for relevant data
    label = instruction.substr(instruction.find_first_of(whitespace));
    label = trimWhiteSpace(label, whitespace);

    return decToHex(B, 2) + sizeMap[label] + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + addMap[label] + decToHex(No_Imm_Involved, 4);
}

string encodeBEQZData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap) {
    string bufferString;
    string regSource;
    string label;

    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regSource = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regSource = trimWhiteSpace(regSource, whitespace);

    label = bufferString.substr(bufferString.find_first_of(whitespace));
    label = trimWhiteSpace(label, whitespace);
    string var = addMap[label];

    return decToHex(BEQZ, 2) + sizeMap[label] + decToHex(determineReg(regSource), 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + addMap[label] + decToHex(No_Imm_Involved, 4);
}

string encodeBGEData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap) {
    string regSource1;
    string regSource2;
    string encodedData;
    string label;
    string bufferString;

    //Parse line for relevant data
    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regSource1 = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regSource1 = trimWhiteSpace(regSource1, whitespace);

    bufferString = bufferString.substr(bufferString.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regSource2 = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regSource2 = trimWhiteSpace(regSource2, whitespace);

    label = bufferString.substr(bufferString.find_first_of(whitespace));
    label = trimWhiteSpace(label, whitespace);

    return decToHex(BGE, 2) + sizeMap[label] + decToHex(determineReg(regSource1), 4) + decToHex(determineReg(regSource2), 4) + decToHex(No_Reg_Involved, 4) + addMap[label] + decToHex(No_Imm_Involved, 4);
}

string encodeBNEData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap) {
    string regSource1;
    string regSource2;
    string encodedData;
    string label;
    string bufferString;

    //Parse line for relevant data
    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regSource1 = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regSource1 = trimWhiteSpace(regSource1, whitespace);

    bufferString = bufferString.substr(bufferString.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regSource2 = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regSource2 = trimWhiteSpace(regSource2, whitespace);

    label = bufferString.substr(bufferString.find_first_of(whitespace));
    label = trimWhiteSpace(label, whitespace);

    return decToHex(BNE, 2) + sizeMap[label] + decToHex(determineReg(regSource1), 4) + decToHex(determineReg(regSource2), 4) + decToHex(No_Reg_Involved, 4) + addMap[label] + decToHex(No_Imm_Involved, 4);
}

string encodeLAData(string instruction, map<string, string> &addMap, map<string, string> &sizeMap) {
    string bufferString;
    string regDestination;
    string label;

    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regDestination = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regDestination = trimWhiteSpace(regDestination, whitespace);

    label = bufferString.substr(bufferString.find_first_of(whitespace));
    label = trimWhiteSpace(label, whitespace);

    return decToHex(LA, 2) + sizeMap[label] + decToHex(determineReg(regDestination), 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + addMap[label] + decToHex(No_Imm_Involved, 4);
}

string encodeLBData(string instruction) {
    string bufferString;
    string regDestination;
    string regSource1;

    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regDestination = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regDestination = trimWhiteSpace(regDestination, whitespace);

    regSource1 = bufferString.substr(bufferString.find_first_of("(") + 1, 3);
    regSource1 = trimWhiteSpace(regSource1, whitespace);

    return decToHex(LB, 2) + decToHex(No_Size_Involved, 4) + decToHex(determineReg(regDestination), 4) + decToHex(determineReg(regSource1), 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Addr_Involved, 4) + decToHex(No_Imm_Involved, 4);
}

string encodeLIData(string instruction) {
    string bufferString;
    string regDestination;
    string immVal;

    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regDestination = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regDestination = trimWhiteSpace(regDestination, whitespace);

    immVal = bufferString.substr(bufferString.find_first_of(whitespace));
    immVal = trimWhiteSpace(immVal, whitespace);

    return decToHex(LI, 2) + decToHex(No_Size_Involved, 4) + decToHex(determineReg(regDestination), 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Addr_Involved, 4) + decToHex(stoi(immVal), 4);
}

string encodeSUBIData(string instruction) {
    string regDestination;
    string regSource1;
    string encodedData;
    string bufferString;
    int immVal;

    //Parse line for relevant data
    bufferString = instruction.substr(instruction.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regDestination = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regDestination = trimWhiteSpace(regDestination, whitespace);

    bufferString = bufferString.substr(bufferString.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    regSource1 = bufferString.substr(bufferString.find_first_of("$"), bufferString.find_first_of(","));
    regSource1 = trimWhiteSpace(regSource1, whitespace);

    bufferString = bufferString.substr(bufferString.find_first_of(whitespace));
    bufferString = trimWhiteSpace(bufferString, whitespace);

    immVal = stoi(bufferString.substr(bufferString.find_first_not_of(whitespace)));

    return decToHex(SUBI, 2) + decToHex(No_Size_Involved, 4) + decToHex(determineReg(regDestination), 4) + decToHex(determineReg(regSource1), 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Addr_Involved, 8) + decToHex(immVal, 4);
}

string encodeSYSCALLData(string instruction) {
    return decToHex(SYSCALL, 2) + decToHex(No_Size_Involved, 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Reg_Involved, 4) + decToHex(No_Addr_Involved, 8) + decToHex(No_Imm_Involved, 4);
}

//----------------------------------------Instruction Components----------------------------------------//
string fetchInstruction(string *memArray) {
    ProgramClockCycles++;
    return readFromMem(pc, memArray, Instruction_Size);
}

string memoryAccess(int offset, string *memArray, int sizeInBytes, string action, string value) {
    ProgramClockCycles++;
    if (action.compare("readStringFromMem") == 0) {
       return readStringFromMem(offset, memArray); 
    } else if (action.compare("readFromMem") == 0) {
       return readFromMem(offset, memArray, sizeInBytes);
    } else { //write string to memory
       writeStringToMem(offset, value, memArray);
       return "";
    }
}

void decodeInstructionAndRegFetch() {
    ProgramClockCycles++;
}

int aluArith(int reg, int immVal, string action) {
    ProgramClockCycles++;
    return setReg(reg, immVal, action);
}

bool aluComp(int val1, int val2, string action) {
    ProgramClockCycles++;
    return compareVal(val1, val2, action);
}

int wb(int reg, int immVal, string action) {
    ProgramClockCycles++;
    return setReg(reg, immVal, "load");
}

void setPC(int offset) {
    ProgramClockCycles++;
    pc = offset;
}

//----------------------------------------Helper Methods----------------------------------------//

//***Returns string that has been trimmed of whitespace
 string trimWhiteSpace(string stringToTrim, string charOfChoice) {
     stringToTrim.erase(0, stringToTrim.find_first_not_of(charOfChoice));
     stringToTrim.erase(stringToTrim.find_last_not_of(charOfChoice) + 1);
     return stringToTrim;
 }

 string trimComment(string stringToTrim) {
     int pos = stringToTrim.find_first_of("#");
     if (pos >= 0) {
         stringToTrim = stringToTrim.erase(pos);
     }
     return stringToTrim;
 }

//***Converst hexadecimal value to decimal
 string hexToDec(string hexVal) {
    std::stringstream stream;
    int decValue = 0;
    stream << hexVal;
    stream >> std::hex >> decValue;
    return to_string(decValue);
}

//***Converts decimal value to hexadecimal and prepends zeros based on specified hexValSize
string decToHex(int decVal, int hexValSize) {
    std::stringstream stream;
    stream << std::hex << decVal;
    string rando = stream.str();
    return prependZeros(stream.str(), hexValSize - stream.str().length());
}

//***Prepends zero to a string
string prependZeros(string value, int numToPrepend) {
    string zeros = "";
    for (int i = 0; i < numToPrepend; i++) {
        zeros += "0";
    }
    return value.insert(0, zeros);
}

string stringToASCII(const string& value) {
    string asciiString = "";
    for(unsigned int i = 0; i < value.length(); i++) {
        int c = (int)value[i]; //this is your character
        asciiString += decToHex(c, 2);
    }
    asciiString += "00";
    return asciiString;
}

int determineReg(string reg) {
    if (reg.compare("zero") == 0) {
        return 0;
    } else if (reg.compare("$at") == 0) {
        return 1;
    } else if (reg.compare("$v0") == 0) {
        return 2;
    } else if (reg.compare("$v1") == 0) {
        return 3;
    } else if (reg.compare("$a0") == 0) {
        return 4;
    } else if (reg.compare("$a1") == 0) {
        return 5;
    } else if (reg.compare("$a2") == 0) {
        return 6;
    } else if (reg.compare("$a3") == 0) {
        return 7;
    } else if (reg.compare("$t0") == 0) {
        return 8;
    } else if (reg.compare("$t1") == 0) {
        return 9;
    } else if (reg.compare("$t2") == 0) {
        return 10;
    } else if (reg.compare("$t3") == 0) {
        return 11;
    } else if (reg.compare("$t4") == 0) {
        return 12;
    } else if (reg.compare("$t5") == 0) {
        return 13;
    } else if (reg.compare("$t6") == 0) {
        return 14;
    } else if (reg.compare("$t7") == 0) {
        return 15;
    } else if (reg.compare("$s0") == 0) {
        return 16;
    } else if (reg.compare("$s1") == 0) {
        return 17;
    } else if (reg.compare("$s2") == 0) {
        return 18;
    } else if (reg.compare("$s3") == 0) {
        return 19;
    } else if (reg.compare("$s4") == 0) {
        return 20;
    } else if (reg.compare("$s5") == 0) {
        return 21;
    } else if (reg.compare("$s6") == 0) {
        return 22;
    } else if (reg.compare("$s7") == 0) {
        return 23;
    } else if (reg.compare("$t8") == 0) {
        return 24;
    } else if (reg.compare("$t9") == 0) {
        return 25;
    } else if (reg.compare("$k0") == 0) {
        return 26;
    } else if (reg.compare("$k1") == 0) {
        return 27;
    } else if (reg.compare("$gp") == 0) {
        return 28;
    } else if (reg.compare("$sp") == 0) {
        return 29;
    } else if (reg.compare("$fp") == 0) {
        return 30;
    } else if (reg.compare("$ra") == 0) {
        return 31;
    } else {
        return -1;
    }
}

int getValueOfReg(int reg) {
    switch (reg) {
        case 0:
            return $0;
            break;
        case 1:
            return $1;
            break;
        case 2:
            return $2;
            break;
        case 3:
            return $3;
            break;
        case 4:
            return $4;
            break;
        case 5:
            return $5;
            break;
        case 6:
            return $6;
            break;
        case 7:
            return $7;
            break;
        case 8:
            return $8;
            break;
        case 9:
            return $9;
            break;
        case 10:
            return $10;
            break;
        case 11:
            return $11;
            break;
        case 12:
            return $12;
            break;
        case 13:
            return $13;
            break;
        case 14:
            return $14;
            break;
        case 15:
            return $15;
            break;
        case 16:
            return $16;
            break;
        case 17:
            return $17;
            break;
        case 18:
            return $18;
            break;
        case 19:
            return $19;
            break;
        case 20:
            return $20;
            break;
        case 21:
            return $21;
            break;
        case 22:
            return $22;
            break;
        case 23:
            return $23;
            break;
        case 24:
            return $24;
            break;
        case 25:
            return $25;
            break;
        case 26:
            return $26;
            break;
        case 27:
            return $27;
            break;
        case 28:
            return $28;
            break;
        case 29:
            return $29;
            break;
        case 30:
            return $30;
            break;
        case 31:
            return $31;
            break;
        default:
            cout << "The register '$" + to_string(reg) + "' not defined\n" << endl;
            return -1;
            break;
    }
}

bool compareVal(int val1, int val2, string action) {
    if (action.compare("equalToZero") == 0) {
        return val1 == 0;
    } else if (action.compare("greaterThanEqualTo") == 0) {
        return val1 >= val2;
    } else if (action.compare("equal") == 0) {
        return val1 == val2;
    } else { //"notEqual"
        return val1 != val2;
    }
}

int setReg(int reg, int value, string action) {
    switch (reg) {
        case 0:
            if (action.compare("add") == 0) {
                $0 += value;
                cout << to_string(value) + " added to register 'zero'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $0 -= value;
                cout << to_string(value) + " subtracted from register 'zero'\n" << endl;
            } else { //action = "load"
                $0 = value;
                cout << to_string(value) + " loaded into register 'zero'\n" << endl;
            }
            return $0;
            break;
        case 1:
            if (action.compare("add") == 0) {
                $1 += value;
                cout << to_string(value) + " added to register 'at'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $1 -= value;
                cout << to_string(value) + " subtracted from register 'at'\n" << endl;
            } else { //action = "load"
                $1 = value;
                cout << to_string(value) + " loaded into register 'at'\n" << endl;
            }
            return $1;
            break;
        case 2:
            if (action.compare("add") == 0) {
                $2 += value;
                cout << to_string(value) + " added to register 'v0'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $2 -= value;
                cout << to_string(value) + " subtracted from register 'v0'\n" << endl;
            } else { //action = "load"
                $2 = value;
                cout << to_string(value) + " loaded into register 'v0'\n" << endl;
            }
            return $2;
            break;
        case 3:
            if (action.compare("add") == 0) {
                $3 += value;
                cout << to_string(value) + " added to register 'v1'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $3 -= value;
                cout << to_string(value) + " subtracted from register 'v1'\n" << endl;
            } else { //action = "load"
                $3 = value;
                cout << to_string(value) + " loaded into register 'v1'\n" << endl;
            }
            return $3;
            break;
        case 4:
            if (action.compare("add") == 0) {
                $4 += value;
                cout << to_string(value) + " added to register 'a0'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $4 -= value;
                cout << to_string(value) + " subtracted from register 'a0'\n" << endl;
            } else { //action = "load"
                $4 = value;
                cout << to_string(value) + " loaded into register 'a0'\n" << endl;
            }
            return $4;
            break;
        case 5:
            if (action.compare("add") == 0) {
                $5 += value;
                cout << to_string(value) + " added to register 'a1'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $5 -= value;
                cout << to_string(value) + " subtracted from register 'a1'\n" << endl;
            } else { //action = "load"
                $5 = value;
                cout << to_string(value) + " loaded into register 'a1'\n" << endl;
            }
            return $5;
            break;
        case 6:
            if (action.compare("add") == 0) {
                $6 += value;
                cout << to_string(value) + " added to register 'a2'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $6 -= value;
                cout << to_string(value) + " subtracted from register 'a2'\n" << endl;
            } else { //action = "load"
                $6 = value;
                cout << to_string(value) + " loaded into register 'a2'\n" << endl;
            }
            return $6;
            break;
        case 7:
            if (action.compare("add") == 0) {
                cout << to_string(value) + " added to register 'a3'\n" << endl;
                $7 += value;
            } else if (action.compare("subtract") == 0) {
                $7 -= value;
                cout << to_string(value) + " subtracted from register 'a3'\n" << endl;
            } else { //action = "load"
                $7 = value;
                cout << to_string(value) + " loaded into register 'a3'\n" << endl;
            }
            return $7;
            break;
        case 8:
            if (action.compare("add") == 0) {
                $8 += value;
                cout << to_string(value) + " added to register 't0'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $8 -= value;
                cout << to_string(value) + " subtracted register 't0'\n" << endl;
            } else { //action = "load"
                $8 = value;
                cout << to_string(value) + " loaded into register 't0'\n" << endl;
            }
            return $8;
            break;
        case 9:
            if (action.compare("add") == 0) {
                $9 += value;
                cout << to_string(value) + " added to register 't1'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $9 -= value;
                cout << to_string(value) + " subtracted from register 't1'\n" << endl;
            } else { //action = "load"
                $9 = value;
                cout << to_string(value) + " loaded into register 't1'\n" << endl;
            }
            return $9;
            break;
        case 10:
            if (action.compare("add") == 0) {
                $10 += value;
                cout << to_string(value) + " added to register 't2'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $10 -= value;
                cout << to_string(value) + " subtracted from register 't2'\n" << endl;
            } else { //action = "load"
                $10 = value;
                cout << to_string(value) + " loaded into register 't2'\n" << endl;
            }
            return $10;
            break;
        case 11:
            if (action.compare("add") == 0) {
                $11 += value;
                cout << to_string(value) + " added to register 't3'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $11 -= value;
                cout << to_string(value) + " subtracted from register 't3'\n" << endl;
            } else { //action = "load"
                $11 = value;
                cout << to_string(value) + " loaded into register 't3'\n" << endl;
            }
            return $11;
            break;
        case 12:
            if (action.compare("add") == 0) {
                $12 += value;
                cout << to_string(value) + " added to register 't4'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $12 -= value;
                cout << to_string(value) + " subtracted from register 't4'\n" << endl;
            } else { //action = "load"
                $12 = value;
                cout << to_string(value) + " loaded into register 't4'\n" << endl;
            }
            return $12;
            break;
        case 13:
            if (action.compare("add") == 0) {
                $13 += value;
                cout << to_string(value) + " added to register 't5'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $13 -= value;
                cout << to_string(value) + " subtracted from register 't5'\n" << endl;
            } else { //action = "load"
                $13 = value;
                cout << to_string(value) + " loaded into register 't5'\n" << endl;
            }
            return $13;
            break;
        case 14:
            if (action.compare("add") == 0) {
                $14 += value;
                cout << to_string(value) + " added to register 't6'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $14 -= value;
                cout << to_string(value) + " subtracted from register 't6'\n" << endl;
            } else { //action = "load"
                $14 = value;
                cout << to_string(value) + " loaded into register 't6'\n" << endl;
            }
            return $14;
            break;
        case 15:
            if (action.compare("add") == 0) {
                $15 += value;
                cout << to_string(value) + " added to register 't7'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $15 -= value;
                cout << to_string(value) + " subtracted from register 't7'\n" << endl;
            } else { //action = "load"
                $15 = value;
                cout << to_string(value) + " loaded into register 't7'\n" << endl;
            }
            return $15;
            break;
        case 16:
            if (action.compare("add") == 0) {
                $16 += value;
                cout << to_string(value) + " added to register 's0'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $16 -= value;
                cout << to_string(value) + " subtracted from register 's0'\n" << endl;
            } else { //action = "load"
                $16 = value;
                cout << to_string(value) + " loaded into register 's0'\n" << endl;
            }
            return $16;
            break;
        case 17:
            if (action.compare("add") == 0) {
                $17 += value;
                cout << to_string(value) + " added to register 's1'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $17 -= value;
                cout << to_string(value) + " subtracted from register 's1'\n" << endl;
            } else { //action = "load"
                $17 = value;
                cout << to_string(value) + " loaded into register 's1'\n" << endl;
            }
            return $17;
            break;
        case 18:
            if (action.compare("add") == 0) {
                $18 += value;
                cout << to_string(value) + " added to register 's2'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $18 -= value;
                cout << to_string(value) + " subtracted from register 's2'\n" << endl;
            } else { //action = "load"
                $18 = value;
                cout << to_string(value) + " loaded into register 's2'\n" << endl;
            }
            return $18;
            break;
        case 19:
            if (action.compare("add") == 0) {
                $19 += value;
                cout << to_string(value) + " added to register 's3'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $19 -= value;
                cout << to_string(value) + " subtracted from register 's3'\n" << endl;
            } else { //action = "load"
                $19 = value;
                cout << to_string(value) + " loaded into register 's3'\n" << endl;
            }
            return $19;
            break;
        case 20:
            if (action.compare("add") == 0) {
                $20 += value;
                cout << to_string(value) + " added to register 's4'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $20 -= value;
                cout << to_string(value) + " subtracted register 's4'\n" << endl;
            } else { //action = "load"
                $20 = value;
                cout << to_string(value) + " loaded into register 's4'\n" << endl;
            }
            return $20;
            break;
        case 21:
            if (action.compare("add") == 0) {
                $21 += value;
                cout << to_string(value) + " added to register 's5'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $21 -= value;
                cout << to_string(value) + " subtracted from register 's5'\n" << endl;
            } else { //action = "load"
                $21 = value;
                cout << to_string(value) + " loaded into register 's5'\n" << endl;
            }
            return $21;
            break;
        case 22:
            if (action.compare("add") == 0) {
                $22 += value;
                cout << to_string(value) + " added to register 's6'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $22 -= value;
                cout << to_string(value) + " subtracted from register 's6'\n" << endl;
            } else { //action = "load"
                $22 = value;
                cout << to_string(value) + " loaded into register 's6'\n" << endl;
            }
            return $22;
            break;
        case 23:
            if (action.compare("add") == 0) {
                $23 += value;
                cout << to_string(value) + " added to register 's7'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $23 -= value;
                cout << to_string(value) + " subtracted from register 's7'\n" << endl;
            } else { //action = "load"
                $23 = value;
                cout << to_string(value) + " loaded into register 's7'\n" << endl;
            }
            return $23;
            break;
        case 24:
            if (action.compare("add") == 0) {
                $24 += value;
                cout << to_string(value) + " added to register 't8'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $24 -= value;
                cout << to_string(value) + " subtracted from register 't8'\n" << endl;
            } else { //action = "load"
                $24 = value;
                cout << to_string(value) + " loaded into register 't8'\n" << endl;
            }
            return $24;
            break;
        case 25:
            if (action.compare("add") == 0) {
                $25 += value;
                cout << to_string(value) + " added to register 't9'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $25 -= value;
                cout << to_string(value) + " subtracted from register 't8'\n" << endl;
            } else { //action = "load"
                $25 = value;
                cout << to_string(value) + " loaded into register 't8'\n" << endl;
            }
            return $25;
            break;
        case 26:
            if (action.compare("add") == 0) {
                $26 += value;
                cout << to_string(value) + " added to register 'k0'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $26 -= value;
                cout << to_string(value) + " subtracted from register 'k0'\n" << endl;
            } else { //action = "load"
                $26 = value;
                cout << to_string(value) + " loaded into register 'k0'\n" << endl;
            }
            return $26;
            break;
        case 27:
            if (action.compare("add") == 0) {
                $27 += value;
                cout << to_string(value) + " added to register 'k1'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $27 -= value;
                cout << to_string(value) + " subtracted from register 'k1'\n" << endl;
            } else { //action = "load"
                $27 = value;
                cout << to_string(value) + " loaded into register 'k1'\n" << endl;
            }
            return $27;
            break;
        case 28:
            if (action.compare("add") == 0) {
                $28 += value;
                cout << to_string(value) + " added to register 'gp'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $28 -= value;
                cout << to_string(value) + " subtracted from register 'gp'\n" << endl;
            } else { //action = "load"
                $28 = value;
                cout << to_string(value) + " loaded into register 'gp'\n" << endl;
            }
            return $28;
            break;
        case 29:
            if (action.compare("add") == 0) {
                $29 += value;
                cout << to_string(value) + " added to register 'sp'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $29 -= value;
                cout << to_string(value) + " subtracted from register 'sp'\n" << endl;
            } else { //action = "load"
                $29 = value;
                cout << to_string(value) + " loaded into register 'sp'\n" << endl;
            }
            return $29;
            break;
        case 30:
            if (action.compare("add") == 0) {
                $30 += value;
                cout << to_string(value) + " added to register 'fp'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $30 -= value;
                cout << to_string(value) + " subtracted from register 'fp'\n" << endl;
            } else { //action = "load"
                $30 = value;
                cout << to_string(value) + " loaded into register 'fp'\n" << endl;
            }
            return $30;
            break;
        case 31:
            if (action.compare("add") == 0) {
                $31 += value;
                cout << to_string(value) + " added to register 'ra'\n" << endl;
            } else if (action.compare("subtract") == 0) {
                $31 -= value;
                cout << to_string(value) + " subtracted from register 'ra'\n" << endl;
            } else { //action = "load"
                $31 = value;
                cout << to_string(value) + " loaded into register 'ra'\n" << endl;
            }
            return $31;
            break;
        default:
            cout << "The register '$" + to_string(reg) + "' not defined\n" << endl;
            return -1;
            break;
    }
}