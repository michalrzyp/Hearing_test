#include "zapis.h"
#include <iostream>
using namespace std;

Zapis::Zapis()
{
  kanal = true;

}

Zapis::~Zapis()
{
}

void Zapis::WyborKanalu(bool kanal)
{
  if (kanal == true)
  {
    logfile_.open("prawy1.txt", ios::out | ios::trunc);
    logfile_ << flush;
    logfile_.close();
  }
  else
  {
    logfile_.open("lewy1.txt", ios::out | ios::trunc);
    logfile_ << flush;
    logfile_.close();
  }
}

void Zapis::ZapiszWynikiDoPlikuTxt(bool kanal, float tab[liczbaBadanychF][2])
{
  WyborKanalu(kanal);
  kanal == true ? logfile_.open("prawy1.txt", ios::out | ios::app) : logfile_.open("lewy1.txt", ios::out | ios::app);
  for (int i = 0; i < liczbaBadanychF; i++)
  {
      logfile_ << tab[i][0];
      logfile_ <<" "<<tab[i][1]<< endl;
  }
  logfile_.close();
}
