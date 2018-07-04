#ifndef ZAPIS_H_ 
#define ZAPIS_H_ 
#include <fstream>  


using namespace std; 
class Zapis
{
public:
  Zapis();
  ~Zapis();
  static  const int liczbaBadanychF = 27;
  void ZapiszWynikiDoPlikuTxt(bool kanal,float tab[liczbaBadanychF][2]);
  void WyborKanalu(bool kanal);
private:
  ofstream logfile_;
  bool kanal;

};
#endif // !NETWORK_SIMULATION_LOGGER_H