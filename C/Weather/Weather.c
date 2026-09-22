#include <cjson/cJSON.h>
#include <cs50.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE_URL "https://api.openweathermap.org/data/2.5/weather"
#define MAX_URL_LEN 512
#define MAX_BUFFER_LEN 256
#define NUM_ALGERIAN_CITIES 58

#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif
typedef struct {
  char *buffer;
  size_t len;
} ResponseBuffer;

const char *algerian_cities[NUM_ALGERIAN_CITIES] = {
    "Adrar",         "Chlef",
    "Laghouat",      "Oum El Bouaghi",
    "Batna",         "Bejaia",
    "Biskra",        "Bechar",
    "Blida",         "Bouira",
    "Tamanrasset",   "Tebessa",
    "Tlemcen",       "Tiaret",
    "Tizi Ouzou",    "Algiers",
    "Djelfa",        "Jijel",
    "Setif",         "Saida",
    "Skikda",        "Sidi Bel Abbes",
    "Annaba",        "Guelma",
    "Constantine",   "Medea",
    "Mostaganem",    "Msila",
    "Mascara",       "Ouargla",
    "Oran",          "El Bayadh",
    "Illizi",        "Bordj Bou Arreridj",
    "Boumerdes",     "El Tarf",
    "Tindouf",       "Tissemsilt",
    "El Oued",       "Khenchela",
    "Souk Ahras",    "Tipaza",
    "Mila",          "Ain Defla",
    "Naama",         "Ain Temouchent",
    "Ghardaia",      "Relizane",
    "Timimoun",      "Bordj Badji Mokhtar",
    "Ouled Djellal", "Beni Abbes",
    "In Salah",      "In Guezzam",
    "Touggourt",     "Djanet",
    "El Mghair",     "El Meniaa"};

bool init_response_buffer(ResponseBuffer *s) {
  if (s == NULL) {
    return false;
  }

  s->len = 0;
  s->buffer = (char *)malloc(1);

  if (s->buffer == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return false;
  }

  s->buffer[0] = '\0';
  return true;
}

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t new_len = ((ResponseBuffer *)userp)->len + size * nmemb;
  ResponseBuffer *s = (ResponseBuffer *)userp;

  char *tmp = (char *)realloc(s->buffer, new_len + 1);
  if (tmp == NULL) {
    fprintf(stderr, "Memory reallocation failed\n");
    return 0;
  }

  s->buffer = tmp;
  memcpy(s->buffer + s->len, contents, size * nmemb);
  s->buffer[new_len] = '\0';
  s->len = new_len;

  return size * nmemb;
}

bool is_valid_algerian_city(const char *city_name) {
  if (city_name == NULL) {
    return false;
  }

  for (int i = 0; i < NUM_ALGERIAN_CITIES; i++) {
    if (strcasecmp(city_name, algerian_cities[i]) == 0) {
      return true;
    }
  }
  return false;
}

void parse_weather_data(const char *json) {
  if (json == NULL) {
    fprintf(stderr, "No weather data to parse\n");
    return;
  }

  float temperature = 0, humidity = 0, pressure = 0;
  float wind_speed = 0, cloudiness = 0, latitude = 0, longitude = 0;
  char description[MAX_BUFFER_LEN] = {0};
  char city[MAX_BUFFER_LEN] = {0};

  char *temp_ptr = (char *)strstr(json, "\"temp\":");
  if (temp_ptr != NULL) {
    sscanf(temp_ptr, "\"temp\":%f", &temperature);
  }

  char *coord_ptr = (char *)strstr(json, "\"coord\"");
  if (coord_ptr != NULL) {
    char *lon_ptr = strstr(coord_ptr, "\"lon\":");
    char *lat_ptr = strstr(coord_ptr, "\"lat\":");

    if (lon_ptr != NULL) {
      sscanf(lon_ptr, "\"lon\":%f", &longitude);
    }
    if (lat_ptr != NULL) {
      sscanf(lat_ptr, "\"lat\":%f", &latitude);
    }
  }

  char *hum_ptr = (char *)strstr(json, "\"humidity\":");
  if (hum_ptr != NULL) {
    sscanf(hum_ptr, "\"humidity\":%f", &humidity);
  }

  char *pres_ptr = (char *)strstr(json, "\"pressure\":");
  if (pres_ptr != NULL) {
    sscanf(pres_ptr, "\"pressure\":%f", &pressure);
  }

  char *wind_ptr = (char *)strstr(json, "\"speed\":");
  if (wind_ptr != NULL) {
    sscanf(wind_ptr, "\"speed\":%f", &wind_speed);
  }

  char *desc_ptr = (char *)strstr(json, "\"description\":\"");
  if (desc_ptr != NULL) {
    sscanf(desc_ptr, "\"description\":\"%255[^\"]\"", description);
  }

  char *cloud_ptr = (char *)strstr(json, "\"all\":");
  if (cloud_ptr != NULL) {
    sscanf(cloud_ptr, "\"all\":%f", &cloudiness);
  }

  char *city_ptr = (char *)strstr(json, "\"name\":\"");
  if (city_ptr != NULL) {
    sscanf(city_ptr, "\"name\":\"%255[^\"]\"", city);
  }

  printf("\nWeather Information:\n");
  printf("City: %s\n", city);
  printf("Temperature: %.2f°C\n", temperature);
  printf("Humidity: %.2f%%\n", humidity);
  printf("Pressure: %.2f hPa\n", pressure);
  printf("Wind Speed: %.2f m/s\n", wind_speed);
  printf("Coordinates: %.2f, %.2f\n", latitude, longitude);
  printf("Cloudiness: %.2f%%\n", cloudiness);
  printf("Description: %s\n", description);
}

void send_weather_request(const char *city_name) {
  if (city_name == NULL) {
    fprintf(stderr, "Invalid city name\n");
    return;
  }

  CURL *curl = curl_easy_init();
  if (curl == NULL) {
    fprintf(stderr, "CURL initialization failed\n");
    return;
  }

  ResponseBuffer response;
  if (!init_response_buffer(&response)) {
    curl_easy_cleanup(curl);
    return;
  }

  char *api_key = getenv("OPENWEATHER_API_KEY");
  if (api_key == NULL) {
    fprintf(stderr, "OPENWEATHER_API_KEY environment variable not set\n");
    free(response.buffer);
    curl_easy_cleanup(curl);
    return;
  }

  char *encoded_city = curl_easy_escape(curl, city_name, 0);
  if (encoded_city == NULL) {
    fprintf(stderr, "URL encoding failed\n");
    free(response.buffer);
    curl_easy_cleanup(curl);
    return;
  }

  char url[MAX_URL_LEN];
  snprintf(url, sizeof(url),
           "https://api.openweathermap.org/data/2.5/"
           "weather?q=%s,DZ&appid=%s&units=metric",
           encoded_city, api_key);

  curl_free(encoded_city);

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "WeatherApp/1.0");

  CURLcode result = curl_easy_perform(curl);

  if (result != CURLE_OK) {
    fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(result));
  } else {
    parse_weather_data(response.buffer);
  }

  free(response.buffer);
  curl_easy_cleanup(curl);
}

int main(void) {
  curl_global_init(CURL_GLOBAL_ALL);

  char *city_name = get_string("Enter the city name: ");

  if (city_name == NULL) {
    fprintf(stderr, "Failed to read city name\n");
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif

  if (is_valid_algerian_city(city_name)) {
    send_weather_request(city_name);
  } else {
    printf("Invalid city name. Please enter a valid Algerian city.\n");
    curl_global_cleanup();
    return EXIT_FAILURE;
  }

  curl_global_cleanup();

  return EXIT_SUCCESS;
}
