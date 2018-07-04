#ifndef BADANIE_H_ 
#define BADANIE_H_

class Badanie
{
public:
  Badanie(float czest);
  ~Badanie();
  float ZwrocF();
  unsigned int ZwrocAmp();
  void UstawS³yszane(int AktualnieBadanaAMP);
  bool Przebadana();
  unsigned int ZwrocWynik();
  void PrzestawIndeks();
private:
  static const int liczba_progow_natezenia_ = 16;   // co 3dB
  float czestotliwosc_;
  unsigned int tabNatezen_[liczba_progow_natezenia_];
  bool dzwieki_slyszane_[liczba_progow_natezenia_];  //tablica slyszanych natezen dla danej f
  int indeks_dolny_;
  int indek_gorny_;
  bool przebadana_; // f przebadana lub nie 
};
#endif 