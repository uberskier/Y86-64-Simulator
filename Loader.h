
class Loader
{
   private:
      bool loaded; //set to true if a file is successfully loaded into memory    
      bool fileCheck(char * file); 
      uint32_t convert(std::string line, int b, int e);
      void loadline(std::string line);
      uint8_t convert8(std::string line, int b, int e);
      bool hasErrors(std::string line);
      bool hasColon(std::string line);
      bool hasBar(std::string line);
      bool hasMultTwo(std::string line);
      bool hasData(std::string line);
      bool hasX(std::string line);
      bool hasZero(std::string line);
      bool hasNoSpace(std::string line);
      bool hasExtraSpace(std::string line);
      bool hasCons(std::string line);
      //bool hasBadAdd(std::string line);
      bool hasNoCom(std::string line);
      bool validAddress(std::string line);
      bool outsideMem(std::string line);
      std::ifstream inf;  //input file handle
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
};
