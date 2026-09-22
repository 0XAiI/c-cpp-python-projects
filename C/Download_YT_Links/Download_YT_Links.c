#include <cs50.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 1024
#define MAX_COMMAND 4096

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

bool is_valid_link(const char *link) {
  if (link == NULL) {
    return false;
  }
  return (strstr(link, "youtube.com/watch?v=") != NULL) ||
         (strstr(link, "youtu.be/") != NULL);
}

void get_unique_output(char *buffer, const char *extension) {
  const char *dir_name = "Music";
  MKDIR(dir_name);

  for (int i = 0; i < 100; i++) {
    snprintf(buffer, MAX_PATH, "Music/video%d.%s", i, extension);
  }

  snprintf(buffer, MAX_PATH, "./Music/default_output.%s", extension);
}

void play_file(const char *path) {
  if (path == NULL) {
    fprintf(stderr, "Error: NULL path provided\n");
    return;
  }

  printf("Playing: %s\n", path);
  char command[MAX_COMMAND];
  snprintf(command, sizeof(command), "mpv \"%s\"", path);

  if (system(command) != 0) {
    fprintf(stderr, "Failed to play file: %s\n", path);
  }
}

bool download_media(const char *link, const char *format, const char *options) {
  if (link == NULL || format == NULL || options == NULL) {
    fprintf(stderr, "Error: Invalid parameters\n");
    return false;
  }

  char output[MAX_PATH];
  get_unique_output(output, format);

  char command[MAX_COMMAND];
  snprintf(command, sizeof(command), "yt-dlp %s -o \"%s\" \"%s\"", options,
           output, link);
  printf("Downloading %s...\n", format);
  int result = system(command);

  if (result != 0) {
    fprintf(stderr, "Download failed.\n");
    return false;
  }

  printf("Downloaded to %s\n", output);

  printf("Play it now? (y/n): ");
  char choice;
  if (scanf(" %c", &choice) == 1 && (choice == 'y' || choice == 'Y')) {
    play_file(output);
  }

  return true;
}

void download_media_menu(const char *prompt, const char *format,
                         const char *options) {
  if (!prompt)
    return;
  string link = get_string("%s", prompt);

  if (link == NULL) {
    return;
  }

  if (!is_valid_link(link)) {
    fprintf(stderr, "Invalid YouTube link.\n");
    free(link);
    return;
  }

  download_media(link, format, options);
  free(link);
}

void download_mp4(void) {
  download_media_menu(
      "Enter MP4 YouTube link: ", "mp4",
      "-f mp4/bestvideo+bestaudio/best --merge-output-format mp4");
}

void download_mp3(void) {
  download_media_menu("Enter MP3 YouTube link: ", "mp3",
                      "-x --audio-format mp3");
}

void download_wav(void) {
  download_media_menu("Enter WAV YouTube link: ", "wav",
                      "-x --audio-format wav");
}

#ifdef __linux__
bool install_dependencies_for_linux(void) {
  if (system("which yt-dlp > /dev/null") != 0 ||
      system("which mpv > /dev/null") != 0) {
    printf("\nRequired tools not found. Install yt-dlp and mpv? (y/n): ");
    string resp = get_string("Enter your choice: ");

    if (resp == NULL) {
      return false;
    }

    if (resp[0] == 'y' || resp[0] == 'Y') {
      if (system("sudo apt install -y yt-dlp mpv") != 0) {
        fprintf(stderr, "Installation failed. Please install manually.\n");
        free(resp);
        return false;
      }
      printf("Dependencies installed successfully.\n");
    }

    free(resp);
  }
  return true;
}
#endif

#ifdef _WIN32
bool install_dependencies_for_windows(void) {
  if (system("where yt-dlp > nul") != 0 || system("where mpv > nul") != 0) {
    printf("Required tools not found. Install yt-dlp and mpv? (y/n): ");
    string resp = get_string("Enter your choice: ");

    if (resp == NULL) {
      return false;
    }

    if (resp[0] == 'y' || resp[0] == 'Y') {
      if (system("winget install yt-dlp.yt-dlp > nul") != 0 ||
          system("winget install -e --id mpv.net > nul") != 0) {
        fprintf(stderr, "Installation failed. Please install manually.\n");
        free(resp);
        return false;
      }
      printf("Dependencies installed successfully.\n");
    }

    free(resp);
  }
  return true;
}
#endif

void menu(void) {
#ifdef __linux__
  if (!install_dependencies_for_linux()) {
    return;
  }
#endif

#ifdef _WIN32
  if (!install_dependencies_for_windows()) {
    return;
  }
#endif

  while (true) {
    printf("\n==== YouTube Media Downloader ====\n");
    printf("1. Download MP4 Video\n");
    printf("2. Download MP3 Audio\n");
    printf("3. Download WAV Audio\n");
    printf("4. Exit\n");

    string choice = get_string("Choose an option: ");
    if (choice == NULL) {
      continue;
    }

    switch (choice[0]) {
    case '1':
      download_mp4();
      break;
    case '2':
      download_mp3();
      break;
    case '3':
      download_wav();
      break;
    case '4':
      printf("Goodbye!\n");
    default:
      printf("Invalid choice. Try again.\n");
    }
    free(choice);
  }
}

int main(void) {
  menu();
  return 0;
}
