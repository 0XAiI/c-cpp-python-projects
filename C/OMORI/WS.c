#include <raylib.h>
#include <raymath.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define ROOM_X 250.0f
#define ROOM_Y 270.0f
#define ROOM_W 250.0f
#define ROOM_H 200.0f

#define OMORI_SCALE 2.5f
#define CAT_SCALE 0.025f
#define DOOR_SCALE 0.1f

static Image icon = {0};

typedef struct {
  Image image;
  Texture2D texture;
  Vector2 position;
  bool isCollision;
  Rectangle rec;
} Cat;

typedef struct {
  Image image;
  Texture2D texture;
  Vector2 position;
  bool isCollision;
  Rectangle rec;
} Door;

typedef struct {
  int frame;
  int maxFrames;
  float timer;
  float speed;
} Animation;

typedef enum {
  DIRECTION_DOWN,
  DIRECTION_UP,
  DIRECTION_LEFT,
  DIRECTION_RIGHT
} Direction;

typedef struct {
  Vector2 position;
  Vector2 oldPosition;
  bool isFrozen;
  bool stopped;
  Image image;
  Texture2D currentTexture;
  Texture2D textureLeft;
  Texture2D textureRight;
  Texture2D textureUp;
  Texture2D textureDown;
  Direction direction;
  float speed;
  Music music;
  Sound sound;
} Omori;

static void InitObjects(Cat *cat, Door *door) {
  cat->isCollision = false;
  door->isCollision = false;

  cat->image = LoadImage("./Image/Cat.png");
  door->image = LoadImage("./Image/Door.png");

  if (cat->image.data == NULL || door->image.data == NULL) {
    TraceLog(LOG_ERROR, "Could not load images");
    exit(EXIT_FAILURE);
  }

  cat->texture = LoadTextureFromImage(cat->image);
  door->texture = LoadTextureFromImage(door->image);

  cat->position =
      (Vector2){ROOM_X + 15.0f,
                ROOM_Y + ROOM_H - (cat->texture.height * CAT_SCALE) - 10.0f};

  door->position = (Vector2){ROOM_X + ROOM_W / 2.0f -
                                 (door->texture.width * DOOR_SCALE) / 2.0f,
                             ROOM_Y};

  cat->rec = (Rectangle){cat->position.x, cat->position.y,
                         cat->texture.width * CAT_SCALE,
                         cat->texture.height * CAT_SCALE};

  door->rec = (Rectangle){door->position.x, door->position.y,
                          door->texture.width * DOOR_SCALE,
                          door->texture.height * DOOR_SCALE};

  UnloadImage(cat->image);
  UnloadImage(door->image);
}

static void InitAnimation(Animation *ani) {
  ani->frame = 0;
  ani->maxFrames = 4;
  ani->timer = 0.0f;
  ani->speed = 0.5f;
}

static void InitOmori(Omori *omori) {
  omori->speed = 2.575f;
  omori->isFrozen = false;
  omori->stopped = false;

  omori->position.x = (float)GetScreenWidth() / 2.0f - 20.0f;

  omori->position.y = (float)GetScreenHeight() / 2.0f - 50.0f;

  omori->direction = DIRECTION_DOWN;

  Image imageDown = LoadImage("./Image/OMORI.png");
  Image imageUp = LoadImage("./Image/up.png");
  Image imageLeft = LoadImage("./Image/left.png");
  Image imageRight = LoadImage("./Image/right.png");

  icon = LoadImage("./Image/OMORI.png");

  if (icon.data == NULL) {
    TraceLog(LOG_ERROR, "Could not load window icon");
    exit(EXIT_FAILURE);
  }

  omori->textureDown = LoadTextureFromImage(imageDown);
  omori->textureUp = LoadTextureFromImage(imageUp);
  omori->textureLeft = LoadTextureFromImage(imageLeft);
  omori->textureRight = LoadTextureFromImage(imageRight);

  if (omori->textureDown.id == 0 || omori->textureUp.id == 0 ||
      omori->textureLeft.id == 0 || omori->textureRight.id == 0) {
    fprintf(stderr, "Could not load textures\n");
    exit(EXIT_FAILURE);
  }

  SetTextureFilter(omori->textureDown, TEXTURE_FILTER_POINT);

  SetTextureFilter(omori->textureUp, TEXTURE_FILTER_POINT);

  SetTextureFilter(omori->textureLeft, TEXTURE_FILTER_POINT);

  SetTextureFilter(omori->textureRight, TEXTURE_FILTER_POINT);

  omori->currentTexture = omori->textureDown;

  UnloadImage(imageDown);
  UnloadImage(imageUp);
  UnloadImage(imageLeft);
  UnloadImage(imageRight);
}

static void MoveOmori(Omori *omori) {
  if (omori->stopped)
    return;

  Vector2 velocity = {0};

  if (IsKeyDown(KEY_RIGHT)) {
    velocity.x += 1.0f;
    omori->currentTexture = omori->textureRight;
    omori->direction = DIRECTION_RIGHT;
  }

  if (IsKeyDown(KEY_LEFT)) {
    velocity.x -= 1.0f;
    omori->currentTexture = omori->textureLeft;
    omori->direction = DIRECTION_LEFT;
  }

  if (IsKeyDown(KEY_UP)) {
    velocity.y -= 1.0f;
    omori->currentTexture = omori->textureUp;
    omori->direction = DIRECTION_UP;
  }

  if (IsKeyDown(KEY_DOWN)) {
    velocity.y += 1.0f;
    omori->currentTexture = omori->textureDown;
    omori->direction = DIRECTION_DOWN;
  }

  if (Vector2Length(velocity) > 0.0f) {
    velocity = Vector2Scale(Vector2Normalize(velocity), omori->speed);

    omori->position = Vector2Add(omori->position, velocity);
  }
}

static void UpdateOmoriAnimation(Animation *ani) {
  ani->timer += GetFrameTime();

  if (ani->timer >= ani->speed) {
    ani->frame = (ani->frame + 1) % ani->maxFrames;

    ani->timer = 0.0f;
  }
}

static bool CheckOmoriBoundaryCollision(const Omori *omori) {

  const float width = omori->currentTexture.width * OMORI_SCALE;

  const float height = omori->currentTexture.height * OMORI_SCALE;

  return (omori->position.x < ROOM_X || omori->position.y < ROOM_Y ||
          omori->position.x + width > ROOM_X + ROOM_W ||
          omori->position.y + height > ROOM_Y + ROOM_H);
}

static void DrawGameWindow(void) {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "WHITE SPACE");

  InitAudioDevice();

  SetTargetFPS(60);

  Cat cat = {0};
  Door door = {0};

  Omori *omori = (Omori *)calloc(1, sizeof(Omori));

  Animation *ani = (Animation *)calloc(1, sizeof(Animation));

  if (omori == NULL || ani == NULL) {
    TraceLog(LOG_ERROR, "Could not allocate memory");

    free(omori);
    free(ani);

    CloseAudioDevice();
    CloseWindow();

    exit(EXIT_FAILURE);
  }

  InitObjects(&cat, &door);
  InitOmori(omori);
  InitAnimation(ani);

  SetWindowIcon(icon);

  const Rectangle room = {ROOM_X, ROOM_Y, ROOM_W, ROOM_H};

  const Vector2 resetPosition = {(float)GetScreenWidth() / 2.0f - 20.0f,
                                 (float)GetScreenHeight() / 2.0f - 50.0f};

  const float freezeDuration = 1.0f;

  float freezeTime = 0.0f;
  bool playingFreezeSound = false;

  omori->music = LoadMusicStream("./Sound/OMORI.mp3");

  if (omori->music.ctxData == NULL) {
    TraceLog(LOG_ERROR, "Could not load music stream");

    goto cleanup;
  }

  omori->sound = LoadSound("./Sound/red-hands.mp3");

  if (omori->sound.frameCount == 0) {
    TraceLog(LOG_ERROR, "Could not load sound");

    UnloadMusicStream(omori->music);

    goto cleanup;
  }

  SetSoundVolume(omori->sound, 0.5f);
  SetMusicVolume(omori->music, 0.5f);

  PlayMusicStream(omori->music);

  while (!WindowShouldClose()) {
    UpdateMusicStream(omori->music);

    UpdateOmoriAnimation(ani);

    if (!omori->isFrozen) {
      omori->oldPosition = omori->position;

      if (!omori->stopped) {
        MoveOmori(omori);
      }
    } else {
      omori->currentTexture = omori->textureDown;

      freezeTime -= GetFrameTime();

      if (freezeTime <= 0.0f) {
        omori->isFrozen = false;
        playingFreezeSound = false;
      }
    }

    if (CheckOmoriBoundaryCollision(omori)) {
      omori->position = resetPosition;

      omori->oldPosition = omori->position;

      omori->isFrozen = true;

      freezeTime = freezeDuration;

      if (!playingFreezeSound) {
        PlaySound(omori->sound);
        playingFreezeSound = true;
      }
    }

    Rectangle omoriRec = {omori->position.x, omori->position.y,
                          omori->currentTexture.width * OMORI_SCALE,
                          omori->currentTexture.height * OMORI_SCALE};

    cat.isCollision = CheckCollisionRecs(cat.rec, omoriRec);

    door.isCollision = CheckCollisionRecs(door.rec, omoriRec);

    if (cat.isCollision || door.isCollision) {
      omori->position = omori->oldPosition;
    }

    BeginDrawing();

    ClearBackground(WHITE);

    DrawRectangleLinesEx(room, 4, BLACK);

    DrawTextureEx(cat.texture, cat.position, 0.0f, CAT_SCALE, WHITE);

    DrawTextureEx(door.texture, door.position, 0.0f, DOOR_SCALE, WHITE);

    const Rectangle source = {0.0f, 0.0f, (float)omori->currentTexture.width,
                              (float)omori->currentTexture.height};

    const Rectangle dest = {omori->position.x, omori->position.y,
                            omori->currentTexture.width * OMORI_SCALE,
                            omori->currentTexture.height * OMORI_SCALE};

    DrawTexturePro(omori->currentTexture, source, dest, (Vector2){0, 0}, 0.0f,
                   WHITE);

    EndDrawing();
  }

  UnloadTexture(omori->textureDown);
  UnloadTexture(omori->textureUp);
  UnloadTexture(omori->textureLeft);
  UnloadTexture(omori->textureRight);

  UnloadSound(omori->sound);
  UnloadMusicStream(omori->music);

  UnloadImage(icon);

  UnloadTexture(cat.texture);
  UnloadTexture(door.texture);

  CloseAudioDevice();
  CloseWindow();

  free(omori);
  free(ani);

  return;

cleanup:
  UnloadTexture(omori->textureDown);
  UnloadTexture(omori->textureUp);
  UnloadTexture(omori->textureLeft);
  UnloadTexture(omori->textureRight);

  UnloadImage(icon);

  UnloadTexture(cat.texture);
  UnloadTexture(door.texture);

  CloseAudioDevice();
  CloseWindow();

  free(omori);
  free(ani);
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  DrawGameWindow();

  return EXIT_SUCCESS;
}
