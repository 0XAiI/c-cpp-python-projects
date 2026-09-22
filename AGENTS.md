# AGENTS.md

Personal collection of 23 independent projects under `C/` (14), `C++/` (7), `Python/` (2). No shared code, no tests, no CI, no root build — each project is self-contained. Scope changes to a single project folder, then build/run that project as its own verification.

## C projects (`C/`)

- Build per project, never from the root: `cd C/<Project>` then `make` (build), `make run` (build + run), `make clean`. Compilers are `clang` (TypeGame uses `gcc`) with `-Wall -Wextra`.
- Each Makefile links only that project's deps (`-lcs50`, `-lraylib`, `-lSDL2`, `-lcurl`, `-ljson-c`, ...); 6 projects use the CS50 library as an input helper. Don't assume a project has a dep just because another does.
- Quirks: `C/Weather/` reads `OPENWEATHER_API_KEY` from `C/Weather/.env` (gitignored). TypeGame must be built from `C/TypeGame/Src/` (its Makefile pulls sources from `../Include/`). Games like OMORI load assets from local `Image/`, `Sound/`, `Font/` dirs — keep folder structure intact.

## C++ projects (`C++/`)

- All 7 projects are subdirectories of the root `C++/CMakeLists.txt` and build together; deps come from the vcpkg manifest `C++/vcpkg.json` (raylib, sfml, sdl2, sdl2-image, sdl2-ttf, miniaudio) and need the vcpkg toolchain:
  `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake && cmake --build build`
- Binaries land in `C++/build/bin`. There is no pre-existing `build/`; a fresh configure downloads/installs vcpkg deps first.
- To add a project you need both a `C++/<Project>/CMakeLists.txt` and an `add_subdirectory(<Project>)` in the root file.
- Pacman is C++20 + SFML; the rest are mostly C++17 + Raylib or SFML. Some projects keep a legacy Makefile (e.g. `MPlayer/src/Makefile`) that predates CMake — prefer the CMake build.

## Python projects (`Python/`)

- `AiChatBot`: managed with uv (`pyproject.toml` + `uv.lock`, Python 3.13). Run `uv sync` then `python main.py`. Note `main.py` only actually imports `openai` and `python-dotenv`, while `pyproject.toml` also declares langchain/langgraph — pip-install only what you use if going that route. Requires a `.env` with `GROQ_API_KEY` (gitignored).
- `DigitRecognition`: plain pip project (tensorflow, opencv-python, numpy, matplotlib). The trained `model_cnn.h5` is gitignored and absent from the repo — without it, `main.py` retrains MNIST from scratch on first run (slow). Predictions read 28×28 grayscale images from `digits/`.

## Git / verification

- No CI, tests, or pre-commit; verification is build + run of the affected project (graphical ones need a display).
- API projects need an untracked `.env` (copy `.env.example`, add a real key): Weather `OPENWEATHER_API_KEY`, AiChatBot `GROQ_API_KEY`.
- Media and generated binaries are gitignored (`*.mp3`, `*.mp4`, `*.h5`, `output.png`, `stb_image.h`), but some files predate those rules and remain tracked — e.g. `C/ImageViewer/stb_image.h`, `C++/Quiz/Music/music.mp3`. Don't remove them or re-add new ones.