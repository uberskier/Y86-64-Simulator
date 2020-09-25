/**
 * Names: Zack Noble & Baylor Matney
 * Team: matney-noble
*/
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>

#include "Loader.h"
#include "Memory.h"

//first column in file is assumed to be 0
#define ADDRBEGIN 2   //starting column of 3 digit hex address 
#define ADDREND 4     //ending column of 3 digit hext address
#define DATABEGIN 7   //starting column of data bytes
#define COMMENT 28    //location of the '|' character 

Memory * mem = Memory::getInstance();

/**
 * Loader constructor
 * Opens the .yo file named in the command line arguments, reads the contents of the file
 * line by line and loads the program into memory.  If no file is given or the file doesn't
 * exist or the file doesn't end with a .yo extension or the .yo file contains errors then
 * loaded is set to false.  Otherwise loaded is set to true.
 *
 * @param argc is the number of command line arguments passed to the main; should
 *        be 2
 * @param argv[0] is the name of the executable
 *        argv[1] is the name of the .yo file
 */
Loader::Loader(int argc, char * argv[])
{
   loaded = false;
   

   //Start by writing a method that opens the file (checks whether it ends 
   //with a .yo and whether the file successfully opens; if not, return without 
   //loading)
   if (argc != 2) {
      return;
   }

   if (!Loader::fileCheck(argv[1])) {
      return;
   }

   std::string str(argv[1]);
   inf.open(str); 


   std::string line;
   std::getline (inf, line);
   int lineNumber = 1;

   while (inf.good()) {
      //std::cout << line << "\n";
      if (line.substr(0,1) != " ") {
         if (hasErrors(line)) {
            std::cout << "Error on line " << std::dec << lineNumber
                     << ": " << line << std::endl;
            return;
         }
      }
      if (line.substr(DATABEGIN,1) != " ") {
         Loader::loadline(line);
      }
      std::getline (inf, line);
      lineNumber++;
   }
   inf.close(); 
   
   //The file handle is declared in Loader.h.  You should use that and
   //not declare another one in this file.
   
   //Next write a simple loop that reads the file line by line and prints it out
   
   //Next, add a method that will write the data in the line to memory 
   //(call that from within your loop)

   //Finally, add code to check for errors in the input line.
   //When your code finds an error, you need to print an error message and return.
   //Since your output has to be identical to your instructor's, use this cout to print the
   //error message.  Change the variable names if you use different ones.
   //  std::cout << "Error on line " << std::dec << lineNumber
   //       << ": " << line << std::endl;


   //If control reaches here then no error was found and the program
   //was loaded into memory.
   loaded = true;  
  
}

/**
 * isLoaded
 * returns the value of the loaded data member; loaded is set by the constructor
 *
 * @return value of loaded (true or false)
 */
bool Loader::isLoaded()
{
   return loaded;
}


//You'll need to add more helper methods to this file.  Don't put all of your code in the
//Loader constructor.  When you add a method here, add the prototype to Loader.h in the private
//section.

/**
 * fileCheck
 * returns the flag if file ends with .yo 
 *
 * @param file 
 *
 * @return value if .yo file (true or false)
 */
bool Loader::fileCheck(char * file) 
{
   std::string str(file);
   if (str.substr(str.find_last_of(".") + 1) == "yo") {
      inf.open(str);
      if (inf.is_open()){
        inf.close();
        return true; 
      }
      else {
         return false;
      }
   }
   else {
      return false;
   }
}

void Loader::loadline(std::string line)
{
   uint32_t address = Loader::convert(line, ADDRBEGIN, 3);

   uint8_t tempval = 0; 

   bool error;

   for (int i = 7; i < 28; i+=2) {
      tempval = 0; 
      if (line.substr(i,1) != " ") {
        tempval = Loader::convert8(line, 2, i);
        mem->putByte(tempval, address, error); 
      }
      //printf("%x ------- %x\n", test, tempval);      
      address++;
   }
}

uint32_t Loader::convert(std::string line, int b, int e)
{
   std::string add;
   add.append(line, b, e);
   return std::stoul(add, NULL, 16);
}

uint8_t Loader::convert8(std::string line, int be, int en)
{
   std::string val;
   val.append(line, en, be);
   return std::stoul(val, NULL, 16);
}

bool Loader::hasErrors(std::string line) {
   if(hasColon(line) || hasBar(line) || hasMultTwo(line) || hasData(line) || hasX(line) || hasZero(line) || hasNoSpace(line) || hasExtraSpace(line)) {
      return true;
   }
   return false;
}

bool Loader::hasColon(std::string line) {
   if (line.substr(5,1) != ":") {
      return true;
   }
   return false;
}

bool Loader::hasBar(std::string line) {
   if (line.substr(COMMENT,1) != "|") {
      return true;
   }
   return false;
}

bool Loader::hasMultTwo(std::string line) {
   int length = 0;
   for (int i = 7; i < 28; i++) { 
      if (line.substr(i,1) != " ") { 
         length++;
      }   
   }
   if (length % 2 != 0) {
      return true;
   }
   return false;
}


bool Loader::hasData(std::string line)
{
    for (int x = DATABEGIN; x < 27; x++) {
       int y = line.at(x);
        if(!(isxdigit(y)) && line.substr(x, 1) != " ") {
            return true;
        }
    }
    for (int x = ADDRBEGIN; x <= ADDREND; x++) {
       int z = line.at(x);
        if(!(isxdigit(z))) {
            return true;
        }
    }
    return false;
}

bool Loader::hasX(std::string line) 
{
    if (line.substr(1, 1) != "x") {
        return true;
    }
    return false;
}

bool Loader::hasZero(std::string line) 
{
    if (line.substr(0, 1) != "0") {
        return true;
    }
    return false;
}

bool Loader::hasNoSpace(std::string line) 
{
    if (line.substr(6, 1) != " " ) {
        return true;
    }
    return false;
}

bool Loader::hasExtraSpace(std::string line) 
{
    if (line.substr(6, 1) != " " && !(isxdigit(line.at(7)))) {
        return true;
    }
    return false;
}

