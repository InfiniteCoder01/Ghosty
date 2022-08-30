#include <chrono>
#include <vector>
#include "gui.h"
#include "level.hpp"
#include "creatures.hpp"

void centerText(Window* window, int& y, std::string text) {
  drawText(window, (windowWidth(window) - textWidth(window, text)) / 2, y, text);
  y += tileSize;
}

Image tiles;

void drawQuater(Window* window, int x, int y, int tx, int ty, int x1, int y1) {  // Thanks Kofybreak (Youtube) for this idea
  if (getTile(x + x1 * 2 - 1, y) != getTile(x, y) && getTile(x, y + y1 * 2 - 1) != getTile(x, y)) {
    drawImage(window, &tiles, x * tileSize + x1 * tileSize / 2 - cameraX, y * tileSize + y1 * tileSize / 2 - cameraY, tx * 16 + x1 * 8, ty * 16 + y1 * 8, 8, 8, tileSize / 2, tileSize / 2);
  } else if (getTile(x + x1 * 2 - 1, y) != getTile(x, y) && getTile(x, y + y1 * 2 - 1) == getTile(x, y)) {
    drawImage(window, &tiles, x * tileSize + x1 * tileSize / 2 - cameraX, y * tileSize + y1 * tileSize / 2 - cameraY, tx * 16 + x1 * 8, ty * 16 + 16 + y1 * 8, 8, 8, tileSize / 2, tileSize / 2);
  } else if (getTile(x + x1 * 2 - 1, y) == getTile(x, y) && getTile(x, y + y1 * 2 - 1) != getTile(x, y)) {
    drawImage(window, &tiles, x * tileSize + x1 * tileSize / 2 - cameraX, y * tileSize + y1 * tileSize / 2 - cameraY, tx * 16 + 16 + x1 * 8, ty * 16 + 16 + y1 * 8, 8, 8, tileSize / 2, tileSize / 2);
  } else if (getTile(x + x1 * 2 - 1, y) == getTile(x, y) && getTile(x, y + y1 * 2 - 1) == getTile(x, y)) {
    drawImage(window, &tiles, x * tileSize + x1 * tileSize / 2 - cameraX, y * tileSize + y1 * tileSize / 2 - cameraY, tx * 16 + 16 + x1 * 8, ty * 16 + y1 * 8, 8, 8, tileSize / 2, tileSize / 2);
  }
}

void drawPatch(Window* window, int x, int y, int tx, int ty) {
  drawQuater(window, x, y, tx, ty, 0, 0);
  drawQuater(window, x, y, tx, ty, 1, 0);
  drawQuater(window, x, y, tx, ty, 0, 1);
  drawQuater(window, x, y, tx, ty, 1, 1);
}

int main(int argc, char** argv) {
  EM_ASM(document.getElementsByTagName("head")[0].innerHTML += "<link href='https://fonts.googleapis.com/css?family=Press Start 2P' rel='stylesheet'>");
  Window* window = createWindow("Hi! I'm Ghosty!");
  tiles = Image("Assets/Tiles.png");
  slimeTex = Image("Assets/Slime.png");
  arrowTex = Image("Assets/Arrow.png");
  playerTex = Image("Assets/Mag.png");
  abilityBallTex = Image("Assets/AbilityBall.png");
  hammerTex = Image("Assets/Hammer.png");
  Audio menuTheme = playSound("Sound/MenuTheme.mp3", true);
  while (!isKeyPressed(" ")) {
    antialiasing(window, false);
    setFont(window, std::to_string((int)(tileSize * 0.6)) + "px 'Press Start 2P', cursive");
    tileSize = windowWidth(window) / 40;

    if (isKeyPressed("E")) {
      particlesEnabled = !particlesEnabled;
      playSound("Sound/blip.wav");
    }
    if (isKeyPressed("P")) {
      playerEnabled = !playerEnabled;
      playSound("Sound/blip.wav");
    }
    if (isKeyPressed("R")) {
      renderingEnabled = !renderingEnabled;
      playSound("Sound/blip.wav");
    }
    if (isKeyPressed("Y")) {
      val::global("navigator")["clipboard"].call<void>("writeText", val("https://youtu.be/YTMWMFIyQFM?t=1064"));
      playSound("Sound/blip.wav");
    }
    if (isKeyPressed("X")) {
      setAudioVolume(std::min(getAudioVolume() + 5, 255));
      playSound("Sound/blip.wav");
    }
    if (isKeyPressed("Z")) {
      setAudioVolume(std::max(getAudioVolume() - 5, 0));
      playSound("Sound/blip.wav");
    }

    updateKeys();
    clear(window);
    int y = (windowHeight(window) - tileSize * 11) / 2;
    centerText(window, y, "Ghosty");
    centerText(window, y, "by InfiniteCoder");
    centerText(window, y, "");
    centerText(window, y, "Press E to " + std::string(particlesEnabled ? "disable" : "enable") + " particles");
    centerText(window, y, "Press P to " + std::string(playerEnabled ? "disable" : "enable") + " player");
    centerText(window, y, "Press R to " + std::string(renderingEnabled ? "disable" : "enable") + " rendering");
    centerText(window, y, "Press Z/X to change volume (" + std::to_string(getAudioVolume()) + "/255)");
    centerText(window, y, "");
    centerText(window, y, "Press space to start");
    centerText(window, y, "");
    centerText(window, y, "Original disable Idea: https://youtu.be/YTMWMFIyQFM?t=1064");
    centerText(window, y, "Press Y to copy this ^ URL");
    emscripten_sleep(1);
  }
  menuTheme.stop();
  playSound("Sound/MainTheme.mp3", true);
  resizeLevel();
  float animationTimer = 0;
  auto t = std::chrono::steady_clock::now();
  while (!completed) {
    if (nextLevelIndex != -1) {
      levelIndex = nextLevelIndex;
      nextLevelIndex = -1;
      resizeLevel();
    }
    if (player.respawn) {
      playSound("Sound/respawn.wav");
      explode(player.x + player.w / 2, player.y + player.h / 2, Color(255, 255, 255), 300);
    }
    player = Player(2, levelHeight - 3);
    player.resize();
    while (!player.respawn && nextLevelIndex == -1) {
      antialiasing(window, false);
      float deltaTime = (std::chrono::steady_clock::now() - t).count() / 1000000000.0;
      t = std::chrono::steady_clock::now();
      animationTimer += deltaTime;

      for (int x = 0; x < levelWidth; x++) {
        for (int y = 0; y < levelHeight; y++) {
          if (getTile(x, y) == 'S') {
            setTile(x, y, ' ');
            creatures[levelIndex].push_back(Creature(x * tileSize, y * tileSize, &slimeTex));
          } else if (getTile(x, y) == 'A') {
            setTile(x, y, ' ');
            creatures[levelIndex].push_back(Creature(x * tileSize, y * tileSize, &arrowTex));
          } else if (getTile(x, y) == 'B') {
            setTile(x, y, ' ');
            items[levelIndex].push_back(Item(x * tileSize, y * tileSize, 0, 0, &abilityBallTex));
          } else if (getTile(x, y) == 'h') {
            setTile(x, y, ' ');
            items[levelIndex].push_back(Item(x * tileSize, y * tileSize, 0, 0, &hammerTex));
          }
        }
      }

      player.update(window, deltaTime);

      for (Item& item : items[levelIndex]) {
        item.update(deltaTime);
        item.locked = false;
        if (item.x < player.x + player.w && item.x + item.w > player.x && item.y < player.y + player.h && item.y + item.h > player.y) {
          if (item.image == &abilityBallTex) {
            if (item.animation == 0) {
              abilityBalls++;
              playSound("Sound/pickup.wav");
            } else {
              playSound("Sound/powerup.wav");
              if (item.animation == 1)
                jumpsPossible = 2;
              else if (item.animation == 2)
                dashPossible = 2;
            }
          } else {
            playSound("Sound/powerup.wav");
            canBreak = 1;
          }
          item.frame = -1;
        }
      }
      items[levelIndex].erase(std::remove_if(items[levelIndex].begin(), items[levelIndex].end(), [](const Item& item) { return item.frame < 0; }), items[levelIndex].end());

      for (Creature& creature : creatures[levelIndex]) {
        creature.update(deltaTime);
        if (creature.x < player.x + player.w && creature.x + creature.w > player.x && creature.y < player.y + player.h && creature.y + creature.h > player.y) {
          if (!player.ghost) {
            player.ghostify();
            explode(player.x + player.w / 2, player.y + player.h / 2, Color(230, 25, 25));
          }
        }
        for (Item& item : items[levelIndex]) {
          if (item.image == &abilityBallTex && item.x < creature.x + creature.w && item.x + item.w > creature.x && item.y < creature.y + creature.h && item.y + item.h > creature.y) {  // Catch creature
            creature.targetX = item.x + item.w / 2;
            creature.targetY = item.y + item.h / 2;
            item.locked = true;
            item.speedX = 0;
            item.speedY = 0;
            if (item.animation == 0) playSound("Sound/catch.wav");
            if (creature.image == &slimeTex) {
              item.animation = 1;
            } else {
              item.animation = 2;
            }
          }
        }
      }
      creatures[levelIndex].erase(std::remove_if(creatures[levelIndex].begin(), creatures[levelIndex].end(), [](const Creature& creature) { return creature.w == 1 && creature.h == 1; }), creatures[levelIndex].end());

      for (Particle& particle : particles) {
        particle.update(deltaTime);
      }
      particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& particle) { return particle.time < 0; }), particles.end());

      updateKeys();

      clear(window, Color(32, 32, 32));
      if (renderingEnabled) {
        for (int x = 0; x < levelWidth; x++) {
          for (int y = 0; y < levelHeight; y++) {
            if (getTile(x, y) == '#') {
              drawPatch(window, x, y, 0, 0);
            } else if (getTile(x, y) == 'L') {
              drawImage(window, &tiles, x * tileSize - cameraX, y * tileSize - cameraY, ((int)(animationTimer / 0.2) % 2 == 0 ? 32 : 48), (getTile(x, y - 1) == 'L' ? 16 : 0), 16, 16, tileSize, tileSize);
            } else if (getTile(x, y) == 'C') {
              drawImage(window, &tiles, x * tileSize - cameraX, y * tileSize - cameraY, 64, 0, 16, 16, tileSize, tileSize);
            } else if (getTile(x, y) == '-') {
              if (getTile(x - 1, y) != '-')
                drawImage(window, &tiles, x * tileSize - cameraX, y * tileSize - cameraY, 64, 16, 8, 16, tileSize / 2, tileSize);
              else
                drawImage(window, &tiles, x * tileSize - cameraX, y * tileSize - cameraY, 66, 16, 8, 16, tileSize / 2, tileSize);
              if (getTile(x + 1, y) != '-')
                drawImage(window, &tiles, x * tileSize + tileSize / 2 - cameraX, y * tileSize - cameraY, 72, 16, 8, 16, tileSize / 2, tileSize);
              else
                drawImage(window, &tiles, x * tileSize - cameraX + tileSize / 2, y * tileSize - cameraY, 66, 16, 8, 16, tileSize / 2, tileSize);
            } else if (getTile(x, y) == 'T') {
              drawText(window, x * tileSize - cameraX, y * tileSize - cameraY, "English keyboard only!");
              drawText(window, x * tileSize - cameraX, y * tileSize - cameraY + tileSize * 2, "WASD + Space");
              drawText(window, x * tileSize - cameraX, y * tileSize - cameraY + tileSize * 3, "R - respawn (NOT RESET!)");
              drawText(window, x * tileSize - cameraX, y * tileSize - cameraY + tileSize * 4, "E (ghost) - throw ability ball");
              drawText(window, x * tileSize - cameraX, y * tileSize - cameraY + tileSize * 5, "Tab (ability unlocked) - dash");
            } else if (getTile(x, y) == 'H') {
              drawText(window, x * tileSize - cameraX, y * tileSize - cameraY, "Try going back");
            }
          }
        }

        for (Creature& creature : creatures[levelIndex]) {
          creature.draw(window);
        }

        if (playerEnabled) {
          player.draw(window);
        }

        for (Item& item : items[levelIndex]) {
          item.draw(window);
        }

        for (Particle& particle : particles) {
          particle.draw(window);
        }
      }

      drawText(window, 0, tileSize, std::to_string((int)(1 / deltaTime)) + " FPS");
      emscripten_sleep(1);
    }
  }
  while(!isKeyPressed("esc")) {
    if (isKeyPressed("Y")) {
      val::global("navigator")["clipboard"].call<void>("writeText", val("https://thumbs.dreamstime.com/b/pixel-treasure-chest-art-isolated-white-background-169701203.jpg"));
      playSound("Sound/blip.wav");
    }
    updateKeys();
    clear(window);
    int y = (windowHeight(window) - tileSize * 13) / 2;
    centerText(window, y, "Ghosty");
    centerText(window, y, "by InfiniteCoder");
    centerText(window, y, "");
    centerText(window, y, "Congratulayions, you finished the game!");
    centerText(window, y, "Code by InfiniteCoder");
    centerText(window, y, "Assets by InfiniteCoder");
    centerText(window, y, "Sound by InfiniteCoder");
    centerText(window, y, "Tools used:");
    centerText(window, y, "Aseprite, SFXR,");
    centerText(window, y, "BeepBox");
    centerText(window, y, "");
    centerText(window, y, "Press Y to copy URL to chest texture");
    centerText(window, y, "Press Escape to exit the game");
    emscripten_sleep(1);
  }
  destroyWindow(window);
  return 0;
}
