#include "generator.h"
#include <stdlib.h> 
Generator::Generator()
{
  ziarno_generatora = 214743647;
}


Generator::~Generator()
{
}

int Generator::RozkladRownomierny(int dol_zakresu, int gora_zakresu)
{
  double x = ziarno_generatora;
  int h = int(x / Q);
  x = A * (x - Q * h) - R * h;
  ziarno_generatora = x;
  if (x < 0) x += M;
  double c = dol_zakresu, d = gora_zakresu;
  return (x / M) * (d - c) + c;
}