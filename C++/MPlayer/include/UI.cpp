#define _CRT_SECURE_NO_WARNINGS

#include <commdlg.h>
#include <windows.h>

#include <iostream>
#include <string>

#include "UI.hpp"

std::string UI::Full_Path;

std::string UI::Menu() {
  OPENFILENAMEA ofn = {};
  char fileName[MAX_PATH] = {};

  const char *userProfile = std::getenv("USERPROFILE");
  std::string initialDir =
      userProfile ? std::string(userProfile) + "\\Music" : "";

  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFilter =
      "Audio Files\0*.mp3;*.wav;*.flac;*.ogg;*.mp4\0All Files\0*.*\0";
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrInitialDir = initialDir.empty() ? NULL : initialDir.c_str();
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;

  if (!GetOpenFileNameA(&ofn)) {
    std::cerr << "No file selected" << std::endl;
    return "";
  }

  Full_Path = fileName;
  std::cout << "Selected: " << Full_Path << std::endl;
  return GetSelectedFile();
}
