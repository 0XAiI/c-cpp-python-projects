#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32
#include <commdlg.h>
#include <windows.h>
#else
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/wait.h>
#endif

#include <iostream>
#include <string>

#include "UI.hpp"

std::string UI::Full_Path;

#ifdef _WIN32
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
#else

static bool fzf_available() {
  return std::system("command -v fzf >/dev/null 2>&1") == 0;
}

static bool music_dir_exists(const std::string &dir) {
  struct stat st;
  return stat(dir.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

static std::string pick_track_via_fzf(const std::string &dir) {
  const std::string cmd =
      "ls -1 \"" + dir +
      "\" | "
      "grep -Ei '\\.(mp3|wav|flac|ogg|mp4|webm|m4a|aac|opus|aiff)$' | "
      "fzf --header=\"Pick a track from ~/Music (Esc -> type a path)\"";

  FILE *pipe = popen(cmd.c_str(), "r");
  if (pipe == NULL) {
    return "";
  }

  char buf[4096];
  std::string name;
  if (fgets(buf, sizeof(buf), pipe) != NULL) {
    name = buf;
    while (!name.empty() && (name.back() == '\n' || name.back() == '\r')) {
      name.pop_back();
    }
  }

  int rc = pclose(pipe);
  if (!name.empty() && WIFEXITED(rc) && WEXITSTATUS(rc) == 0) {
    return dir + "/" + name;
  }
  return "";
}

std::string UI::Menu() {
  const char *home = std::getenv("HOME");
  const std::string musicDir = home ? std::string(home) + "/Music" : "";

  if (fzf_available() && music_dir_exists(musicDir)) {
    Full_Path = pick_track_via_fzf(musicDir);
    if (!Full_Path.empty()) {
      std::cout << "Selected: " << Full_Path << std::endl;
      return GetSelectedFile();
    }
    std::cout << "No track selected; falling back to manual entry."
              << std::endl;
  }

  std::cout << "Enter path to an audio file (mp3, wav, flac, ogg, mp4, webm): ";
  std::getline(std::cin, Full_Path);

  if (Full_Path.empty()) {
    std::cerr << "No file selected" << std::endl;
    return "";
  }

  std::cout << "Selected: " << Full_Path << std::endl;
  return GetSelectedFile();
}
#endif
