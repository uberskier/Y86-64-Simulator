
class Loader
{
   private:
      bool loaded; //set to true if a file is successfully loaded into memory    
      bool fileCheck(char * file); 
      uint32_t convert(std::string line, int b, int e);
      void loadline(std::string line);
      uint8_t convert8(std::string line, int b, int e);
      std::ifstream inf;  //input file handle
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
};