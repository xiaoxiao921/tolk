/**
 *  Product:        Tolk
 *  File:           ScreenReaderDriverZDSR.cpp
 *  Description:    Driver for the ZDSR screen reader.
 *  Copyright:      (c) 2022, qt06<qt06.com@gmail.com>
 *  License:        LGPLv3
 */

// The ZDSR Project provides a header and libraries,
// but we don't use these in order to support running even if the DLL is missing.

#include "ScreenReaderDriverZDSR.h"

ScreenReaderDriverZDSR::ScreenReaderDriverZDSR() :
  ScreenReaderDriver(L"ZDSR", true, false),
  #ifdef _WIN64
  controller(LoadLibrary(L"ZDSRAPI_x64.dll")),
  #else
  controller(LoadLibrary(L"ZDSRAPI.dll")),
  #endif
  zdsrSpeak(NULL),
  zdsrStopSpeak(NULL),
  zdsrInitTTS(NULL),
  zdsrGetSpeakState(NULL)
{
  if (controller) {
    zdsrInitTTS = (ZDSRInitTTS)GetProcAddress(controller, "InitTTS");
    zdsrGetSpeakState = (ZDSRGetSpeakState)GetProcAddress(controller, "GetSpeakState");
    zdsrSpeak = (ZDSRSpeak)GetProcAddress(controller, "Speak");
    zdsrStopSpeak = (ZDSRStopSpeak)GetProcAddress(controller, "StopSpeak");
	zdsrInitTTS(0, NULL);
  }
}

ScreenReaderDriverZDSR::~ScreenReaderDriverZDSR() {
  if (controller) FreeLibrary(controller);
}

bool ScreenReaderDriverZDSR::Speak(const wchar_t *str, bool interrupt) {
  return (zdsrSpeak(str, interrupt) == 0);
}

bool ScreenReaderDriverZDSR::Silence() {
  zdsrStopSpeak();
  return true;
}

bool ScreenReaderDriverZDSR::IsSpeaking() {
  return (zdsrGetSpeakState() == 3);
}

bool ScreenReaderDriverZDSR::IsActive() {
  if (zdsrGetSpeakState) return (zdsrGetSpeakState() >= 3);
  return false;
}

bool ScreenReaderDriverZDSR::Output(const wchar_t *str, bool interrupt) {
  // Beware short-circuiting.
  const bool speak = zdsrSpeak(str, interrupt);
  const bool braille = Braille(str);
  return (speak || braille);
}
