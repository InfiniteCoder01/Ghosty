#pragma once

#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/html5.h>

using emscripten::val;
thread_local const val JSdocument = val::global("document");
thread_local const val JSwindow = val::global("window");
int __audioVolume = 120;
#endif

struct Color {
  Color() = default;
#ifdef __EMSCRIPTEN__
  char color[10];

  Color(int red, int green, int blue, int alpha = 255) { sprintf(color, "#%02x%02x%02x%02x", red, green, blue, alpha); }
#endif
};

struct Image {
  int width, height;

  Image() = default;
#ifdef __EMSCRIPTEN__
  char* image;
  val JSimage;

  Image(const std::string& filename) {
    image = emscripten_get_preloaded_image_data(filename.c_str(), &width, &height);
    val imageData = val::global("ImageData").new_(val::global("Uint8ClampedArray").new_(emscripten::typed_memory_view<uint8_t>(width * height * 4, (uint8_t*)image)), width, height);
    val canvas = JSdocument.call<val>("getElementById", val("canvas"));
    val ctx = canvas.call<val>("getContext", val("2d"));
    int w = canvas["width"].as<int>(), h = canvas["height"].as<int>();
    canvas.set("width", width);
    canvas.set("height", height);
    ctx.call<void>("putImageData", imageData, 0, 0);
    JSimage = val::global("Image").new_();
    JSimage.set("src", canvas.call<val>("toDataURL"));
    canvas.set("width", w);
    canvas.set("height", h);
  }

  ~Image() { free(image); }
#endif
};

struct Window {
  Window() = default;

#ifdef __EMSCRIPTEN__
  val ctx;
  Window(val ctx) : ctx(ctx) {}
#endif
};

struct Audio {
  val audio;

  Audio() = default;
  Audio(val audio) : audio(audio) {}

  void stop() { audio.call<void>("pause"); }
};

Audio playSound(const std::string& filename, bool looped = false) {
#ifdef __EMSCRIPTEN__
  val audio = val::global("Audio").new_(filename);
  audio.set("volume", __audioVolume / 255.f);
  audio.set("loop", looped);
  audio.call<void>("play");
  return Audio(audio);
#endif
  return Audio();
}

void setAudioVolume(int volume) { __audioVolume = volume; }

int getAudioVolume() { return __audioVolume; }

Window* createWindow(const std::string& name) {
#ifdef __EMSCRIPTEN__
  emscripten_set_window_title(name.c_str());
  // clang-format off
  EM_ASM({
    keys = {};
    window.addEventListener("keydown", function(event) {
      keys[event.key.toLowerCase()] = { pressed: true, released: false, held: true };
      event.preventDefault();
    });
    window.addEventListener("keyup", function(event) {
      keys[event.key.toLowerCase()] = { pressed: false, released: true, held: false };
      event.preventDefault();
    });
  });
  // clang-format on
  val canvas = JSdocument.call<val>("getElementById", val("canvas"));
  val ctx = canvas.call<val>("getContext", val("2d"));
  return new Window(ctx);
#endif
  return nullptr;
}

void antialiasing(Window* window, bool enabled) { window->ctx.set("imageSmoothingEnabled", enabled); }

void destroyWindow(Window* window) { delete window; }

int windowWidth(Window* window) { return window->ctx["canvas"]["width"].as<int>(); }
int windowHeight(Window* window) { return window->ctx["canvas"]["height"].as<int>(); }

void fillRect(Window* window, int x, int y, int w, int h, Color color) {
  window->ctx.set("fillStyle", color.color);
  window->ctx.call<void>("fillRect", x, y, w, h);
}

void clear(Window* window, Color color = Color(0, 0, 0)) { fillRect(window, 0, 0, windowWidth(window), windowHeight(window), color); }

void drawPixel(Window* window, int x, int y, Color color) { fillRect(window, x, y, 1, 1, color); }

void drawImage(Window* window, Image* image, int x, int y, int w = -1, int h = -1, bool flip = false, int srcX = 0, int srcY = 0, int srcW = -1, int srcH = -1) {
  if (w == -1) w = image->width;
  if (h == -1) h = image->height;
  if (srcW == -1) srcW = image->width;
  if (srcH == -1) srcH = image->height;
  if (flip) {
    window->ctx.call<void>("save");
    window->ctx.call<void>("translate", window->ctx["canvas"]["width"].as<int>(), 0);
    window->ctx.call<void>("scale", -1, 1);
  }
  window->ctx.call<void>("drawImage", image->JSimage, srcX, srcY, srcW, srcH, (flip ? window->ctx["canvas"]["width"].as<int>() - x : x), y, w * (flip ? -1 : 1), h);
  if (flip) window->ctx.call<void>("restore");
}

void setFont(Window* window, std::string font) { window->ctx.set("font", font); }

void drawText(Window* window, int x, int y, std::string text, Color color = Color(255, 255, 255)) {
  window->ctx.set("fillStyle", color.color);
  window->ctx.call<void>("fillText", text, x, y);
}

int textWidth(Window* window, std::string text) { return window->ctx.call<val>("measureText", text)["width"].as<int>(); }

int textHeight(Window* window, std::string text) { return window->ctx.call<val>("measureText", text)["height"].as<int>(); }

bool isKeyPressed(std::string key) {
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  if (val::global("keys")[key].isUndefined()) return false;
  return val::global("keys")[key]["pressed"].isTrue();
}

bool isKeyReleased(std::string key) {
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  if (val::global("keys")[key].isUndefined()) return false;
  return val::global("keys")[key]["released"].isTrue();
}

bool isKeyHeld(std::string key) {
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  if (val::global("keys")[key].isUndefined()) return false;
  return val::global("keys")[key]["held"].isTrue();
}

void updateKeys() {
#ifdef __EMSCRIPTEN__
  // clang-format off
  EM_ASM(
    for (const [key, value] of Object.entries(keys)) {
      keys[key].pressed = false;
      keys[key].released = false;
    }
  );
  // clang-format on
#endif
}
