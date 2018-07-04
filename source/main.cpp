#include <Windows.h>
#include "resource.h"
#include <stdio.h>
#include <cstdlib>
#include <process.h>
#include <ctime>
#include <math.h>
#include "Commctrl.h"
#include "generator.h"
#include "badanie.h"
#include "zapis.h"
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <mmreg.h>
#include <tchar.h>
#include <complex>

#pragma comment(lib,"winmm.lib")
#define czas 1
#define Pi 3.14159265358979323846

#define FADEIN(t) (t<3000?t/3000.0:1.0)
#define FADEOUT(t,d) (t>d-3000?(d-t)/3000.0:1.0)

SHORT *pBufferForAudio=NULL;
WAVEFORMATEX pcmWaveFormat = { 0 };
MMRESULT mmResult = 0;
HWAVEOUT hwo = 0;
UINT devId;
CHAR Text[20];
WAVEHDR whdr;
Generator *generator=nullptr;
HWND hwndMainWindow;
const int liczbaBadanychF = 28;
Badanie *Fptr[liczbaBadanychF] = { nullptr };
Badanie *Fptr2[liczbaBadanychF] = { nullptr };
bool skonczonoBadanie = false;
int liczbaOszustw;
int  AktualnieBadanaF;
float czest;
int q = 0;
unsigned int PoziomDzwieku = 0x00000001;
const int liczbaOszustwMax = 3;
float BadaneCzestotliwosci[liczbaBadanychF];
float TablicaWynikow[liczbaBadanychF-1][2];
float TablicaWynikow2[liczbaBadanychF-1][2];
bool KanalBadany1 = true;        //ktory kanal badany
HWND hwndSlider;

void muzyka(SHORT *pBufferForAudio, WAVEFORMATEX pcmWaveFormat, double czestotliwosc);
void Note(SHORT* pBufferForAudio, int iStart, int iDuration, float fNote, float fDiv);
void odtworzDzwiek(double czestotliwosc);
void WarunkiStartowe();
void WyswietlWyniki();
bool SprawdzKoniec();
void __cdecl ThreadProc(void* Arg);
bool ChangeVolume(double nVolume, bool bScalar);
float pure(float timeInSeconds, unsigned short channel, void *context);
MMRESULT play(float nSeconds, float signal(float timeInSeconds, unsigned short channel, void *context), void *context = NULL, unsigned long samplesPerSecond = 48000);


INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_INITDIALOG:
  {
    WarunkiStartowe();
    HWND hwndStatic = GetDlgItem(hwndDlg, IDC_STATIC4);
    wsprintf(Text, "%d", q++);
    SetWindowText(hwndStatic, Text);
    return TRUE;
  }
  return FALSE;

  case WM_COMMAND:
    switch (HIWORD(wParam))
    {
    case BN_CLICKED:
      switch (LOWORD(wParam))
      {
      case IDC_BUTTON1:      // Dziek interpretowany jako s³yszany
      {
        HWND hwndStatic = GetDlgItem(hwndDlg, IDC_STATIC4);
        wsprintf(Text, "%d", q++);
        SetWindowText(hwndStatic, Text);

        if (AktualnieBadanaF == 27)
        {
          liczbaOszustw++;
          if (liczbaOszustw == liczbaOszustwMax)
          {
            KillTimer(hwndMainWindow, 1);
            KillTimer(hwndMainWindow, 0);
            WarunkiStartowe();
            MessageBox(hwndDlg, "Nie oszukuj nie mog³eœ s³yszeæ dzwiêku - POWTÓRZ BADANIE", "OSZUST", MB_OK);
          }
        }
        else
        {
          if (KanalBadany1 == true)
            Fptr[AktualnieBadanaF]->UstawS³yszane(PoziomDzwieku);
          else
            Fptr2[AktualnieBadanaF]->UstawS³yszane(PoziomDzwieku >> 16);
        }
        return TRUE;
      }

      case IDC_BUTTON2:          // Rozpoczeto badanie
      {
        WarunkiStartowe();
        SetTimer(hwndDlg, 0, czas * 1000, nullptr);
        return TRUE;
      }

      }
      return TRUE;
    }
    return FALSE;

  case WM_TIMER:
  {
    bool znaleziono;
    HANDLE hThread;
    switch (LOWORD(wParam))
    {
    case 0:
      skonczonoBadanie = SprawdzKoniec();

      if (skonczonoBadanie == true)
      {
        KillTimer(hwndDlg, 0);
        KillTimer(hwndDlg, 1);
        WyswietlWyniki();
        WarunkiStartowe();
        return TRUE;
      }
      if (KanalBadany1)
      {
        znaleziono = false;
        while (!znaleziono)
        {
          AktualnieBadanaF = generator->RozkladRownomierny(0, liczbaBadanychF);
          if (Fptr[AktualnieBadanaF]->Przebadana() == false)
            znaleziono = true;
        }
        czest = Fptr[AktualnieBadanaF]->ZwrocF();
        PoziomDzwieku = Fptr[AktualnieBadanaF]->ZwrocAmp();
        Fptr[AktualnieBadanaF]->PrzestawIndeks();
        hThread = (HANDLE)_beginthread(ThreadProc, 0, NULL);
        SetTimer(hwndDlg, 1, czas * 1500, nullptr);
        KillTimer(hwndDlg, 0);
      }
      else
      {
        znaleziono = false;
        while (!znaleziono)
        {
          AktualnieBadanaF = generator->RozkladRownomierny(0, liczbaBadanychF);
          if (Fptr2[AktualnieBadanaF]->Przebadana() == true)
            znaleziono = false;
          else
            znaleziono = true;
        }
        czest = Fptr2[AktualnieBadanaF]->ZwrocF();
        PoziomDzwieku = Fptr2[AktualnieBadanaF]->ZwrocAmp();
        Fptr2[AktualnieBadanaF]->PrzestawIndeks();
        hThread = (HANDLE)_beginthread(ThreadProc, 0, NULL);
        SetTimer(hwndDlg, 1, czas * 1500, nullptr);
        KillTimer(hwndDlg, 0);
      }
      return FALSE;
    case 1:
      SetTimer(hwndDlg, 0, czas * 1000, nullptr);
      KillTimer(hwndDlg, 1);
      return FALSE;
    }
  }
  return FALSE;

  case WM_CLOSE:
  {
    DestroyWindow(hwndDlg);
    PostQuitMessage(0);
    return TRUE;
  }
  return FALSE;

  }
  return FALSE;
}

void WarunkiStartowe()
{
  ChangeVolume(1.0, true);

  for (int i = liczbaBadanychF - 1; i >= 0; i--)
  {
    if (Fptr[i] != nullptr)
      delete Fptr[i];
    if (Fptr2[i] != nullptr)
      delete Fptr2[i];
  }
  for (int i = 0; i < liczbaBadanychF - 1; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      TablicaWynikow[i][j] = 0;
      TablicaWynikow2[i][j] = 0;
    }
  }
  srand(time(NULL));
  generator = new Generator;
  skonczonoBadanie = false;
  AktualnieBadanaF = 0;
  czest = 0;
  liczbaOszustw = 0;
  KanalBadany1 = true;

  for (int i = 0; i < 4; i++)
  {
    BadaneCzestotliwosci[i] = 20.0 + (50 * i);//Zakres 20 do 220 co 50Hz 4 proby
  }
  for (int i = 4; i < 18; i++)
  {
    BadaneCzestotliwosci[i] = 220.0 + ((i - 3) * 627); //Zakres od 220 do 9k co 627Hz 14 prob 
  }
  for (int i = 18; i < 27; i++)
  {
    BadaneCzestotliwosci[i] = 10000.0 + ((i - 17) * 1111);//Zakres 10k do 20k co 1111 Hz 9 prob
  }

  BadaneCzestotliwosci[27] = 0.0;

  for (int i = 0; i < liczbaBadanychF; i++)
  {
    Fptr[i] = new Badanie(BadaneCzestotliwosci[i]);
    Fptr2[i] = new Badanie(BadaneCzestotliwosci[i]);
  }
  for (int i = 0; i < liczbaBadanychF - 1; i++)
  {
    TablicaWynikow[i][0] = BadaneCzestotliwosci[i];
    TablicaWynikow2[i][0] = BadaneCzestotliwosci[i];
  }
}

void __cdecl ThreadProc(void* Arg)
{
  odtworzDzwiek(czest);
  _endthread();
}

void WyswietlWyniki()
{
  Zapis *ZapisDoPliku = new Zapis();
  for (int i = 0; i < liczbaBadanychF - 1; i++)
  {
    TablicaWynikow[i][1] = Fptr[i]->ZwrocWynik();

    TablicaWynikow2[i][1] = Fptr2[i]->ZwrocWynik();
  }
  ZapisDoPliku->ZapiszWynikiDoPlikuTxt(!KanalBadany1, TablicaWynikow);
  ZapisDoPliku->ZapiszWynikiDoPlikuTxt(KanalBadany1, TablicaWynikow2);
  delete ZapisDoPliku;
  MessageBox(hwndMainWindow, "Wyniki zosta³y zapisane do pliku tekstowego", "KONIEC", MB_OK);
}

bool SprawdzKoniec()
{
  if (KanalBadany1 == true)
  {
    for (int i = 0; i < (liczbaBadanychF - 1); i++)
    {
      if (Fptr[i]->Przebadana() == false)
        return false;
    }
    KanalBadany1 = false;
    return false;
  }
  else
  {
    for (int i = 0; i < (liczbaBadanychF - 1); i++)
    {
      if (Fptr2[i]->Przebadana() == false)
        return false;
    }
    return true;
  }
}

void muzyka(SHORT *pBufferForAudio, WAVEFORMATEX pcmWaveFormat, double czestotliwosc)
{
  float S = pcmWaveFormat.nAvgBytesPerSec;
  Note(pBufferForAudio, 0, czas*S, czestotliwosc, pcmWaveFormat.nSamplesPerSec);
}

void  Note(SHORT* pBufferForAudio, int iStart, int iDuration, float fNote, float fDiv)
{
  for (int i = iStart; i < iStart + iDuration; i++)
  {
    float t = i / fDiv;
    //pBufferForAudio[i] = SHORT(FADEIN(i)*FADEOUT(i, iDuration) * 32768 * sin(2 * Pi*fNote*t) + 32768);
    pBufferForAudio[i] = 32768.0 * sin(2.0 * Pi*fNote*t);
  }
}

void odtworzDzwiek(double czestotliwosc)
{
  if (czestotliwosc != 0)
  {
    whdr.dwBufferLength = czas * pcmWaveFormat.nAvgBytesPerSec;
    {
      muzyka(pBufferForAudio, pcmWaveFormat, czestotliwosc);
      mmResult = waveOutPrepareHeader(hwo, &whdr, sizeof(WAVEHDR));
      mmResult = waveOutWrite(hwo, &whdr, sizeof(WAVEHDR));
      DWORD glosnosc;

      if (KanalBadany1 == true)
      {
        glosnosc = (DWORD)PoziomDzwieku;
      }
      else
      {
        PoziomDzwieku = (PoziomDzwieku) << 17;
        glosnosc = (DWORD)PoziomDzwieku;
      }
      while ((whdr.dwFlags & WHDR_DONE) != WHDR_DONE) Sleep(100);
      mmResult = waveOutSetVolume(hwo, glosnosc);
      mmResult = waveOutUnprepareHeader(hwo, &whdr, sizeof(WAVEHDR));
    }
  }
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, PSTR szCmdLIne, int iCmdShow)
{
  hwndMainWindow = CreateDialog(hinstance, MAKEINTRESOURCE(IDD_MAINVIEW), NULL, DialogProc);
  ShowWindow(hwndMainWindow, iCmdShow);
  MSG msg = {};
  WarunkiStartowe();

  pcmWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
  pcmWaveFormat.nChannels = 2;
  pcmWaveFormat.nSamplesPerSec = 44100L;
  pcmWaveFormat.wBitsPerSample = 16;
  pcmWaveFormat.nAvgBytesPerSec = 176400L;
  pcmWaveFormat.nBlockAlign = 4; //2 jak na 16 bitow
  pcmWaveFormat.cbSize = 0;

  for (devId = 0; devId < waveOutGetNumDevs(); devId++)
  {
    mmResult = waveOutOpen(&hwo, devId, &pcmWaveFormat, 0, 0, CALLBACK_NULL);

    if (mmResult == MMSYSERR_NOERROR) break;
  }

  if (mmResult != MMSYSERR_NOERROR)//gdy sie nie udalo
  {
    MessageBox(hwndMainWindow, TEXT("Nie znaleziono karty"), TEXT("Error"), MB_OK);
    return mmResult;
  }

  pBufferForAudio = new SHORT[pcmWaveFormat.nAvgBytesPerSec * czas];

  ZeroMemory(&whdr, sizeof(WAVEHDR));
  whdr.lpData = reinterpret_cast<LPSTR>(pBufferForAudio);

  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  mmResult = waveOutClose(hwo);
  delete generator;
  for (int i = liczbaBadanychF - 1; i >= 0; i--)
  {
    if (Fptr[i] != nullptr)
      delete Fptr[i];
    if (Fptr2[i] != nullptr)
      delete Fptr2[i];
  }
  delete[] pBufferForAudio;
  return 0;
}

bool ChangeVolume(double nVolume, bool bScalar)
{
  HRESULT hr = NULL;
  bool decibels = false;
  bool scalar = false;
  double newVolume = nVolume;

  CoInitialize(NULL);
  IMMDeviceEnumerator *deviceEnumerator = NULL;
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
    __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
  IMMDevice *defaultDevice = NULL;

  hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
  deviceEnumerator->Release();
  deviceEnumerator = NULL;

  IAudioEndpointVolume *endpointVolume = NULL;
  hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume),
    CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
  defaultDevice->Release();
  defaultDevice = NULL;

  // -------------------------
  float currentVolume = 0;
  endpointVolume->GetMasterVolumeLevel(&currentVolume);
  //printf("Current volume in dB is: %f\n", currentVolume);

  hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
  //CString strCur=L"";
  //strCur.Format(L"%f",currentVolume);
  //AfxMessageBox(strCur);

  // printf("Current volume as a scalar is: %f\n", currentVolume);
  if (bScalar == false)
  {
    hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
  }
  else if (bScalar == true)
  {
    hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
  }
  endpointVolume->Release();

  CoUninitialize();

  return FALSE;
}

MMRESULT play(float nSeconds, float signal(float timeInSeconds, unsigned short channel, void *context), void *context, unsigned long samplesPerSecond)
{
  UINT timePeriod = 1;
  MMRESULT mmresult = MMSYSERR_NOERROR;
  WAVEFORMATEX waveFormat = { 0 };
  waveFormat.cbSize = 0;
  waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
  waveFormat.nChannels = 2;
  waveFormat.nSamplesPerSec = samplesPerSecond;
  const size_t nBuffer = (size_t)(nSeconds * waveFormat.nChannels * waveFormat.nSamplesPerSec);
  float *buffer;
  waveFormat.wBitsPerSample = CHAR_BIT * sizeof(buffer[0]);
  waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / CHAR_BIT;
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

  buffer = (float *)calloc(nBuffer, sizeof(*buffer));
  __try
  {
    for (size_t i = 0; i < nBuffer; i += waveFormat.nChannels)
      for (unsigned short j = 0; j < waveFormat.nChannels; j++)
        buffer[i + j] = signal((i + j) * nSeconds / nBuffer, j, context);
    HWAVEOUT hWavOut = NULL;
    mmresult = waveOutOpen(&hWavOut, WAVE_MAPPER,
      &waveFormat, NULL, NULL, CALLBACK_NULL);
    if (mmresult == MMSYSERR_NOERROR)
    {
      __try
      {
        timeBeginPeriod(timePeriod);
        __try
        {
          WAVEHDR hdr = { 0 };
          hdr.dwBufferLength = (ULONG)(nBuffer * sizeof(buffer[0]));
          hdr.lpData = (LPSTR)&buffer[0];
          mmresult = waveOutPrepareHeader(hWavOut, &hdr, sizeof(hdr));
          if (mmresult == MMSYSERR_NOERROR)
          {
            __try
            {
              ULONG start = GetTickCount();
              mmresult = waveOutWrite(hWavOut, &hdr, sizeof(hdr));
              Sleep((ULONG)(1000 * nSeconds - (GetTickCount() - start)));
            }
            __finally
            {
              //waveOutSetVolume(hwo, 0x0FFF);
              waveOutUnprepareHeader(hWavOut, &hdr, sizeof(hdr));
            }
          }
        }
        __finally { timeEndPeriod(timePeriod); }
      }
      __finally { waveOutClose(hWavOut); }
    }
  }
  __finally { free(buffer); }
  return mmresult;
}

float pure(float timeInSeconds, unsigned short channel, void *context)
{
  const float frequency = *(const float *)context;
  const float angle = (float)(frequency * 2 * Pi * timeInSeconds);
  switch (channel)
  {
  case  0: return (float)sin(angle + 0 * Pi / 2);
  default: return (float)sin(angle + 1 * Pi / 2);
  }
}