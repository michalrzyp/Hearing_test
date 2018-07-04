#include "badanie.h"


Badanie::Badanie(float czest)
{
  czestotliwosc_ = czest;
  indeks_dolny_ = 0;
  indek_gorny_ = liczba_progow_natezenia_ - 1;
  przebadana_ = false;

  for (int i = 0; i < liczba_progow_natezenia_; i++)
  {
    dzwieki_slyszane_[i] = false;
    tabNatezen_[i] = (0x0001 << (i + 1));
  }
  tabNatezen_[liczba_progow_natezenia_ - 1] = tabNatezen_[liczba_progow_natezenia_ - 1]--;
}

Badanie::~Badanie()
{
}

float Badanie::ZwrocF()
{
  return czestotliwosc_;
}

unsigned int Badanie::ZwrocAmp()
{
  return tabNatezen_[indeks_dolny_];
}

void Badanie::PrzestawIndeks()
{
  if (indeks_dolny_ == indek_gorny_)
    przebadana_ = false;

  indeks_dolny_++;
}

void Badanie::UstawS³yszane(int AktualnieBadanaAMP)
{
  przebadana_ = true;
  for (int i = 0; i < liczba_progow_natezenia_; i++)
  {
    if (tabNatezen_[i] == AktualnieBadanaAMP)
    {
      dzwieki_slyszane_[i] = true;
    }
  }
}

bool Badanie::Przebadana()
{
  return przebadana_;
}

unsigned int Badanie::ZwrocWynik()
{
  for (int i = 0; i < liczba_progow_natezenia_; i++)
  {
    if (dzwieki_slyszane_[i] == true)
      return tabNatezen_[i];
  }
  return 0x00000000;   // w ogole ne slyszane f
}