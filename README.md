# 💻 c-cpp-python-projects

> 🎮🛠️ A collection of **23 projects** in **C, C++, and Python** — games, tools, and utilities.

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Platform](https://img.shields.io/badge/Windows%20%7C%20Linux-0078D4?style=for-the-badge)
![PRs](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=for-the-badge)

---

## ✨ Highlights

- 🎮 **9 games** — from Snake and Pac-Man to Tetris, Pong, and an OMORI-inspired exploration game
- 🌐 **API-powered tools** — weather, translation, quotes & jokes, AI chatbot
- 🧠 **Machine learning** — CNN handwritten-digit recognition with TensorFlow/Keras
- 🧰 **Everyday utilities** — QR generator, YouTube downloader, paint app, music player, to-do list

---

## 📁 Projects Overview

### 🔵 C — 14 projects

| Project | Description |
|---------|-------------|
| 🌦️ [Weather](./C/Weather/) | Weather info for Algerian cities using OpenWeatherMap API |
| ⌨️ [TypeGame](./C/TypeGame/) | Typing speed game with difficulty levels |
| 📈 [Functions](./C/Functions/) | Mathematical function plotter (sin, cos, tan, etc.) |
| 🖼️ [ImageViewer](./C/ImageViewer/) | Image viewer supporting multiple formats via SDL2 |
| 💎 [MatchGame](./C/MatchGame/) | Match-3 puzzle game with Raylib |
| 💬 [QJG](./C/QJG/) | Quote and joke generator from public APIs |
| 🔳 [QrCode](./C/QrCode/) | QR code generator outputting PNG |
| 🐍 [SnakeGame](./C/SnakeGame/) | Classic snake game |
| 🔢 [Sudoku](./C/Sudoku/) | Sudoku generator and solver |
| ✅ [TaskManager](./C/TaskManager/) | Graphical to-do list application |
| 🌍 [Translator](./C/Translator/) | Translation using Google Translate API |
| 🧮 [Interpreter](./C/Interpreter/) | Simple arithmetic expression interpreter |
| ⬇️ [Download_YT_Links](./C/Download_YT_Links/) | YouTube video/audio downloader |
| 🌌 [OMORI](./C/OMORI/) | OMORI-inspired 2D exploration game |

### 🟣 C++ — 7 projects

| Project | Description |
|---------|-------------|
| 🏓 [PING-PONG](./C++/PING-PONG/) | Pong game with AI opponent |
| 👾 [Game](./C++/Game/) | Space shooter with aliens and lasers |
| 🎨 [Paint](./C++/Paint/) | Drawing application with brush tools |
| ❓ [Quiz](./C++/Quiz/) | Quiz game with heart character |
| 🎵 [MPlayer](./C++/MPlayer/) | Music player with audio visualizer |
| 🟡 [Pacman](./C++/Pacman/) | Classic Pac-Man game with C++20 and SFML |
| 🧱 [Tetris](./C++/Tetris/) | Classic Tetris game with Raylib |

### 🟢 Python — 2 projects

| Project | Description |
|---------|-------------|
| ✍️ [DigitRecognition](./Python/DigitRecognition/) | CNN-based handwritten digit recognition |
| 🤖 [AiChatBot](./Python/AiChatBot/) | CLI chatbot powered by Groq's Llama 3.3 API |

---

## 🚀 Getting Started

```bash
git clone https://github.com/<your-username>/c-cpp-python-projects.git
cd c-cpp-python-projects
```

🔑 Copy the samples before running API-based projects:

```bash
# AiChatBot
cp Python/AiChatBot/.env.example Python/AiChatBot/.env
# then edit Python/AiChatBot/.env and set GROQ_API_KEY

# Weather (C)
# create C/Weather/.env (or export the variable) with:
# OPENWEATHER_API_KEY=your_key_here
```

---

## 🔨 Building

### 🔵 C Projects

Each C project has a Makefile. Navigate to the project folder and run:

```bash
make        # Build
make run    # Build and run
make clean  # Remove executable
```

Works on Windows and Linux.

### 🟣 C++ Projects

All C++ projects build together from the root CMake file
(`C++/CMakeLists.txt`, dependencies via `C++/vcpkg.json`):

```bash
cd C++
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

(Some projects also keep a legacy per-project Makefile.)

### 🟢 Python Projects

Python projects use pip or uv for dependencies:

```bash
pip install -r requirements.txt
python main.py
```

---

## 📁 Directory Structure

```
c-cpp-python-projects/
├── C/
│   ├── Weather/
│   ├── TypeGame/
│   ├── Functions/
│   ├── ImageViewer/
│   ├── MatchGame/
│   ├── QJG/
│   ├── QrCode/
│   ├── SnakeGame/
│   ├── Sudoku/
│   ├── TaskManager/
│   ├── Translator/
│   ├── Interpreter/
│   ├── Download_YT_Links/
│   └── OMORI/
├── C++/
│   ├── CMakeLists.txt
│   ├── vcpkg.json
│   ├── Game/
│   ├── MPlayer/
│   ├── Pacman/
│   ├── Paint/
│   ├── PING-PONG/
│   ├── Quiz/
│   └── Tetris/
└── Python/
    ├── DigitRecognition/
    │   ├── main.py
    │   └── digits/        # sample 28×28 digit images
    └── AiChatBot/
        ├── main.py
        └── .env.example   # copy to .env and add your key
```

---

## 📚 Libraries Used

| Library | Used By |
|---------|---------|
| **Raylib** | C games & graphics (Functions, MatchGame, SnakeGame, TaskManager, TypeGame, OMORI), C++ (Game, Quiz, Tetris) |
| **SDL2** | C++ PING-PONG, Paint; C ImageViewer |
| **SFML** | C++ MPlayer, Pacman |
| **libcurl** | C API clients (QJG, Translator, Weather) |
| **cJSON / json-c** | C JSON parsing (QJG, Weather) |
| **stb_image** | C ImageViewer image loading |
| **qrcodegen** | C QR code generation |
| **TensorFlow/Keras** | Python DigitRecognition CNN |
| **OpenCV** | Python DigitRecognition image preprocessing |
| **OpenAI SDK** | Python AiChatBot (Groq API) |
| **CS50 Library** | Various C projects (input helpers) |

---

## ⚠️ Requirements & Notes

- 🌦️ **Weather** needs `OPENWEATHER_API_KEY` environment variable (or `.env` file)
- ⬇️ **Download_YT_Links** requires `yt-dlp` and `mpv` installed on your system
- 🤖 **AiChatBot** needs a Groq API key in a `.env` file (`GROQ_API_KEY=...`)
- 🖼️ Some games need assets (fonts, images, sounds) in specific subdirectories — keep the folder structure intact
- ✍️ **DigitRecognition** expects 28×28 grayscale images in `digits/` folder
- 📥 C projects use the CS50 library (`libcs50`) for simple user input

---

## 🤝 Contributing

Contributions are welcome! Feel free to open an issue or submit a pull request — whether it's a bug fix, a new project, or an improvement to an existing one.
