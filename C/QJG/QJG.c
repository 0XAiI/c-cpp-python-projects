#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUOTE_URL "https://api.quotable.io/random"
#define JOKE_URL "https://official-joke-api.appspot.com/jokes/random"

typedef struct {
  bool is_joke;
  bool is_quote;
  char *memory;
  size_t size;
} MemoryChunk;

static size_t write_callback(void *contents, size_t size, size_t nmemb,
                             void *userp) {
  size_t total_size = size * nmemb;
  MemoryChunk *mem = (MemoryChunk *)userp;

  char *ptr = (char *)realloc(mem->memory, total_size + mem->size + 1);
  if (ptr == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, total_size);
  mem->size += total_size;
  mem->memory[mem->size] = '\0';

  return total_size;
}

bool init_memory_chunk(MemoryChunk *chunk) {
  if (chunk == NULL) {
    return false;
  }

  chunk->memory = (char *)malloc(1);
  if (chunk->memory == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return false;
  }

  chunk->memory[0] = '\0';
  chunk->size = 0;
  chunk->is_joke = false;
  chunk->is_quote = false;

  return true;
}

const char *get_user_choice(MemoryChunk *chunk) {
  if (chunk == NULL) {
    return NULL;
  }

  char choice;
  do {
    printf("\n1. Quote\n2. Joke\n3. Exit\n");
    printf("Enter choice: ");

    if (scanf(" %c", &choice) != 1) {
      fprintf(stderr, "Error reading input\n");
      return NULL;
    }

    switch (choice) {
    case '1':
      chunk->is_quote = true;
      chunk->is_joke = false;
      return QUOTE_URL;
    case '2':
      chunk->is_quote = false;
      chunk->is_joke = true;
      return JOKE_URL;
    case '3':
      printf("Exiting...\n");
      return NULL;
    default:
      printf("Invalid choice. Please enter 1, 2, or 3.\n");
      break;
    }
  } while (choice != '3');

  return NULL;
}

void display_quote(const cJSON *json) {
  if (json == NULL) {
    printf("Invalid JSON received\n");
    return;
  }

  cJSON *content = cJSON_GetObjectItemCaseSensitive(json, "content");
  cJSON *author = cJSON_GetObjectItemCaseSensitive(json, "author");

  if (cJSON_IsString(content) && cJSON_IsString(author)) {
    printf("\n\"%s\"\n- %s\n", content->valuestring, author->valuestring);
  } else {
    printf("Could not parse quote\n");
  }
}

void display_joke(const cJSON *json) {
  if (json == NULL) {
    printf("Invalid JSON received\n");
    return;
  }

  cJSON *joke = cJSON_GetObjectItemCaseSensitive(json, "setup");
  cJSON *punchline = cJSON_GetObjectItemCaseSensitive(json, "punchline");

  if (cJSON_IsString(joke) && cJSON_IsString(punchline)) {
    printf("\nSetup: %s\nPunchline: %s\n", joke->valuestring,
           punchline->valuestring);
  } else {
    printf("Could not parse joke\n");
  }
}

void process_response(const MemoryChunk *chunk) {
  if (chunk == NULL || chunk->memory == NULL) {
    printf("No data received\n");
    return;
  }

  cJSON *json = cJSON_Parse(chunk->memory);
  if (json == NULL) {
    printf("Invalid JSON received\n");
    return;
  }

  if (chunk->is_quote) {
    display_quote(json);
  } else if (chunk->is_joke) {
    display_joke(json);
  }

  cJSON_Delete(json);
}

int main(void) {
  curl_global_init(CURL_GLOBAL_DEFAULT);

  MemoryChunk chunk;
  if (!init_memory_chunk(&chunk)) {
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

  const char *url = get_user_choice(&chunk);
  if (url == NULL) {
    free(chunk.memory);
    curl_global_cleanup();
    return EXIT_SUCCESS;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "CURL initialization failed\n");
    free(chunk.memory);
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  CURLcode result = curl_easy_perform(curl);

  if (result != CURLE_OK) {
    fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(result));
    free(chunk.memory);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

  process_response(&chunk);

  free(chunk.memory);
  curl_easy_cleanup(curl);
  curl_global_cleanup();

  return EXIT_SUCCESS;
}
