#ifndef GENERATOR_H_ 
#define GENERATOR_H_

#define M 2147483647.0 
#define Q 127773 
#define R 2836
#define A 16807
class Generator
{
public:
  Generator();
  ~Generator();
  int RozkladRownomierny(int dol_zakresu, int gora_zakresu);
private:
  double ziarno_generatora;
};
#endif
