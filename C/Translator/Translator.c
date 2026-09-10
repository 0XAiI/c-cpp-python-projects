#include <cs50.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_URL_LEN 512
#define MAX_LANGUAGES 50

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

typedef struct {
  char *memory;
  size_t size;
} MemoryStruct;

typedef struct {
  const char *name;
  const char *code;
} LanguageMapping;

const LanguageMapping languages[] = {
    {"arabic", "ar"},   {"spanish", "es"},   {"french", "fr"},
    {"german", "de"},   {"italian", "it"},   {"portuguese", "pt"},
    {"russian", "ru"},  {"chinese", "zh"},   {"japanese", "ja"},
    {"korean", "ko"},   {"hindi", "hi"},     {"bengali", "bn"},
    {"punjabi", "pa"},  {"tamil", "ta"},     {"telugu", "te"},
    {"urdu", "ur"},     {"malayalam", "ml"}, {"kannada", "kn"},
    {"gujarati", "gu"}, {"oriya", "or"},     {"marathi", "mr"},
    {"assamese", "as"}, {"dutch", "nl"},     {"english", "en"},
    {"catalan", "ca"},  {"burmese", "my"},   {"estonian", "et"},
    {"finnish", "fi"},  {"galician", "gl"},  {"georgian", "ka"}};

const size_t language_count = sizeof(languages) / sizeof(languages[0]);

const char *get_language_code(const char *code) {
  if (code == NULL) {
    return NULL;
  }

  if (strlen(code) == 2) {
    for (size_t i = 0; i < language_count; i++) {
      if (strcasecmp(code, languages[i].code) == 0) {
        return languages[i].code;
      }
    }
    return NULL;
  }

  for (size_t i = 0; i < language_count; i++) {
    if (strcasecmp(code, languages[i].name) == 0) {
      return languages[i].code;
    }
  }
  return NULL;
}

void print_supported_languages(void) {
  printf("Supported languages (name or code):\n");
  for (size_t i = 0; i < language_count; i++) {
    printf("- %s (%s)\n", languages[i].name, languages[i].code);
  }
}

bool init_chunk(MemoryStruct *mem) {
  if (mem == NULL) {
    return false;
  }

  mem->memory = (char *)malloc(1);
  if (mem->memory == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return false;
  }
  mem->memory[0] = '\0';
  mem->size = 0;
  return true;
}

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb,
                                    void *userp) {
  size_t new_size = size * nmemb;
  MemoryStruct *mem = (MemoryStruct *)userp;

  char *ptr = (char *)realloc(mem->memory, mem->size + new_size + 1);
  if (ptr == NULL) {
    fprintf(stderr, "Memory reallocation failed\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, new_size);
  mem->size += new_size;
  mem->memory[mem->size] = '\0';

  return new_size;
}

char *url_encode(const char *str) {
  if (str == NULL) {
    return NULL;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "CURL initialization failed\n");
    return NULL;
  }

  char *output = curl_easy_escape(curl, str, 0);
  curl_easy_cleanup(curl);
  return output;
}

void save_translation_to_file(const char *input_text,
                              const char *translated_text) {
  if (input_text == NULL || translated_text == NULL) {
    return;
  }

  FILE *fp = fopen("Translation.txt", "a");
  if (fp == NULL) {
    fprintf(stderr, "Failed to open translation file\n");
    return;
  }

  fprintf(fp, "input text: %s\n", input_text);
  fprintf(fp, "output text: %s\n\n", translated_text);
  fclose(fp);
}

bool translate_text(const char *text, const char *source, const char *output) {
  if (text == NULL || source == NULL || output == NULL) {
    fprintf(stderr, "Invalid parameters for translation\n");
    return false;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "CURL initialization failed\n");
    return false;
  }

  MemoryStruct chunk;
  if (!init_chunk(&chunk)) {
    curl_easy_cleanup(curl);
    return false;
  }

  char *encoded_text = url_encode(text);
  if (encoded_text == NULL) {
    fprintf(stderr, "URL encoding failed\n");
    free(chunk.memory);
    curl_easy_cleanup(curl);
    return false;
  }

  char url[MAX_URL_LEN];
  snprintf(url, sizeof(url),
           "https://translate.googleapis.com/translate_a/"
           "single?client=gtx&sl=%s&tl=%s&dt=t&q=%s",
           source, output, encoded_text);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_memory_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  CURLcode result = curl_easy_perform(curl);
  if (result != CURLE_OK) {
    fprintf(stderr, "Translation request failed: %s\n",
            curl_easy_strerror(result));
    free(chunk.memory);
    curl_free(encoded_text);
    curl_easy_cleanup(curl);
    return false;
  }

  char *translated_text = strstr(chunk.memory, "\"");
  if (translated_text != NULL) {
    translated_text++;
    char *end_text = strchr(translated_text, '\"');
    if (end_text != NULL) {
      *end_text = '\0';
      printf("Translation: %s\n", translated_text);
      save_translation_to_file(text, translated_text);
    }
  }

  free(chunk.memory);
  curl_free(encoded_text);
  curl_easy_cleanup(curl);
  return true;
}

int main(void) {
  curl_global_init(CURL_GLOBAL_ALL);

  char *input_text = get_string("Enter text to translate: ");
  if (input_text == NULL) {
    fprintf(stderr, "Failed to get input text\n");
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

  char *source_input = get_string("Enter source language: ");
  const char *source = get_language_code(source_input);
  if (source == NULL) {
    fprintf(stderr, "Unsupported source language: %s\n", source_input);
    print_supported_languages();
    free(input_text);
    free(source_input);
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

  char *output_input = get_string("Enter output language: ");
  const char *output = get_language_code(output_input);
  if (output == NULL) {
    fprintf(stderr, "Unsupported output language: %s\n", output_input);
    print_supported_languages();
    free(input_text);
    free(source_input);
    free(output_input);
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

  bool success = translate_text(input_text, source, output);

  free(input_text);
  free(source_input);
  free(output_input);
  curl_global_cleanup();

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
