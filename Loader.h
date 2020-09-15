
class Loader
{
   private:
      bool loaded; //set to true if a file is successfully loaded into memory    
      bool fileCheck(char * file); 
      std::ifstream inf;  //input file handle
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
};
