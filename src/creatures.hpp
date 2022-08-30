Image playerTex, slimeTex, arrowTex, abilityBallTex, hammerTex;

const int itemAnimationFrames[] = {0, 1, 8, 9};
struct Item {
  float x, y, speedX, speedY, animator = 0;
  int w, h, frame = 0, animation = 0;
  int nFrames = 1;
  bool locked;
  Image* image;

  Item() = default;
  Item(int x, int y, int speedX, int speedY, Image* image) : x(x), y(y), speedX(speedX), speedY(speedY), image(image) { resize(); }

  void resize() {
    if(image == &abilityBallTex) {
      nFrames = 9;
    }
    w = tileSize;
    h = w * image->height * nFrames / image->width;
  }

  bool collides() {
    for (int x1 = (x + 1) / tileSize; x1 < (x + w - 2) / tileSize; x1++) {
      for (int y1 = (y + 1) / tileSize; y1 < (y + h - 2) / tileSize; y1++) {
        if (getTile(x1, y1) == '#') {
          return true;
        } else if (getTile(x1, y1) == 'L') {
          return true;
        }
      }
    }
    return x < 0 || x + w > levelWidth * tileSize || y < 0 || y + h > levelHeight * tileSize;
  }

  void update(float deltaTime) {
    if (locked) return;
    animator += deltaTime;
    if (animator > 0.1) {
      animator = 0;
      if (animation > 0) {
        frame++;
        if (frame < itemAnimationFrames[animation] || frame >= itemAnimationFrames[animation + 1]) {
          frame = itemAnimationFrames[animation];
        }
      }
    }
    speedX -= speedX * deltaTime * 0.5;
    speedY += deltaTime * tileSize * 64;

    if (collides()) return;
    x += speedX * deltaTime;
    if (collides()) {
      while (collides()) {
        x -= speedX > 0 ? 1 : -1;
      }
      speedX = 0;
    }
    y += speedY * deltaTime;
    if (collides()) {
      while (collides()) {
        y -= speedY > 0 ? 1 : -1;
      }
      if (speedX != 0) {
        playSound("Sound/drop.wav");
      }
      speedX = 0;
      speedY = 0;
    }
  }

  void draw(Window* window) { drawImage(window, image, x - cameraX, y - cameraY, image->width / nFrames * frame, 0, image->width / nFrames, image->height, w, h); }
};

std::vector<Item> items[nLevels];

int jumpsPossible = 1, dashPossible = 0, canBreak = 0, abilityBalls = 1;
bool completed = false;

struct Player {
  int animationFrames[5][3] = {{1, 4, true}, {5, 8, true}, {9, 12, false}, {13, 16, false}, {17, 20, true}};
  float x, y, speedX, speedY, animator = 0, dash = 0;
  bool ghost = false, respawn = false, flip;
  int jumps = 0, nFrames = 20, speed = 16;
  int w, h, frame, animation;

  Player() = default;
  Player(int x, int y) : x(x * tileSize), y(y * tileSize) {}

  void resize() {
    h = tileSize * 2;
    w = h * playerTex.width / playerTex.height / nFrames;
  }

  void ghostify() {
    animation = 3;
    ghost = true;
    playSound("Sound/ghostify.wav");
  }

  bool collides() {
    if (!ghost) {
      for (int x1 = (x + 1) / tileSize; x1 < (x + w - 2) / tileSize; x1++) {
        for (int y1 = (y + 1) / tileSize; y1 < (y + h - 2) / tileSize; y1++) {
          if (getTile(x1, y1) == '#') {
            return true;
          } else if (getTile(x1, y1) == 'L') {
            ghostify();
            return true;
          } else if (getTile(x1, y1) == 'F') {
            nextLevelIndex = levelIndex + 1;
            return true;
          } else if (getTile(x1, y1) == 'P') {
            nextLevelIndex = levelIndex - 1;
            return true;
          } else if (getTile(x1, y1) == '-') {
            if(canBreak) {
              setTile(x1, y1, ' ');
            }
            return true;
          } else if (getTile(x1, y1) == 'C') {
            respawn = true;
            completed = true;
            return true;
          }
        }
      }
    }
    return x < 0 || x + w > levelWidth * tileSize || y < 0 || y + h > levelHeight * tileSize;
  }

  void update(Window* window, float deltaTime) {
    int prevAnim = animation;
    if (!animationFrames[animation][2] || frame == animationFrames[animation][0] - 1) {
      animation = 0;
    }
    if (isKeyPressed("E") && ghost && abilityBalls > 0) {
      abilityBalls--;
      items[levelIndex].push_back(Item(x + (flip ? 0 : w), y + h / 2, (flip ? -tileSize * 10 : tileSize * 10), 0, &abilityBallTex));
      if (flip) {
        items[levelIndex][items[levelIndex].size() - 1].x -= items[levelIndex][items[levelIndex].size() - 1].w;
      }
    }
    if (isKeyPressed("R")) {
      respawn = true;
    }
    speedX += ((isKeyHeld("D") - isKeyHeld("A")) * tileSize * speed - speedX) * deltaTime * 5;
    if (abs(speedX) < 1 && !isKeyHeld("D") && !isKeyHeld("A")) speedX = 0;
    if (speedX > 0) flip = false;
    if (speedX < 0) flip = true;
    if (animation == 0 && speedX != 0) {
      animation = 1;
    }
    if (ghost) {
      speedY += ((isKeyHeld("S") - isKeyHeld("W")) * tileSize * speed - speedY) * deltaTime * 5;
      if (abs(speedY) < 1 && !isKeyHeld("S") && !isKeyHeld("W")) speedY = 0;
      animation = 4;
    } else {
      static bool dashed = false;
      if (isKeyPressed(" ") && jumps > 0) {
        speedY = -tileSize * 20;
        jumps--;
        playSound("Sound/jump.wav");
        explode(x + w / 2, y + h, Color(255, 255, 255));
        animation = 2;
      }
      if (isKeyHeld("tab") && dash > 0) {
        if (!dashed) {
          playSound("Sound/dash.wav");
        }
        dash -= deltaTime;
        speedY = 0;
        speedX = tileSize * speed * (flip ? -1 : 1);
        explode(x + (flip ? w : 0), y + h / 2, Color(255, 255, 255), 10);
        dashed = true;
      } else {
        if (dashed) {
          dash = -5;
          dashed = false;
        }
        dash = std::min(dash + deltaTime, (float)dashPossible);
        speedY += deltaTime * tileSize * 64;
      }
    }

    if (animation != prevAnim) {
      frame = animationFrames[animation][0] - 1;
    }
    animator += deltaTime;
    if (animator > 0.1) {
      animator = 0;
      frame++;
      if (frame >= animationFrames[animation][1]) {
        frame = animationFrames[animation][0] - 1;
      }
    }

    while (collides()) y--;
    x += speedX * deltaTime;
    if (collides()) {
      while (collides()) {
        x -= speedX > 0 ? 1 : -1;
      }
      speedX = 0;
    }
    y += speedY * deltaTime;
    if (collides()) {
      while (collides()) {
        y -= speedY > 0 ? 1 : -1;
      }
      if (speedY > 0) {
        jumps = jumpsPossible;
      }
      speedY = 0;
    }

    cameraX = std::max(0, std::min(levelWidth * tileSize - windowWidth(window), (int)x - windowWidth(window) / 2));
    cameraY = std::max(0, std::min(levelHeight * tileSize - windowHeight(window), (int)y - windowHeight(window) / 2));
  }

  void draw(Window* window) { drawImage(window, &playerTex, x - cameraX, y - cameraY, playerTex.width / nFrames * frame, 0, playerTex.width / nFrames, playerTex.height, w, h, flip); }
};

Player player;

struct Creature {
  float x, y, speedX, speedY, targetX, targetY, animator = 0;
  int nFrames, speed = 80;
  bool flip = false;
  int w, h, frame;
  Image* image;

  Creature() = default;
  Creature(int x, int y, Image* image) : x(x), y(y), image(image) { resize(); }

  void resize() {
    if (image == &slimeTex)
      nFrames = 13;
    else
      nFrames = 2;
    w = tileSize * 2;
    h = w * image->height * nFrames / image->width;
  }

  bool collides() {
    for (int x1 = (x + 1) / tileSize; x1 < (x + w - 2) / tileSize; x1++) {
      for (int y1 = (y + 1) / tileSize; y1 < (y + h - 2) / tileSize; y1++) {
        if (getTile(x1, y1) == '#') {
          return true;
        } else if (getTile(x1, y1) == 'L') {
          return true;
        }
      }
    }
    return x < 0 || x + w > levelWidth * tileSize || y < 0 || y + h > levelHeight * tileSize;
  }

  void update(float deltaTime) {
    if (targetX != 0 && targetY != 0) {  // Catching creatures into ability balls
      frame = 0;
      w -= w * deltaTime * 0.005;
      h -= h * deltaTime * 0.005;
      w = std::max(w, 1);
      h = std::max(h, 1);
      speedX = (targetX - (x + w / 2)) * 10;
      speedY = (targetY - (y + h / 2)) * 10;
    } else {
      animator += deltaTime;
      if (animator > 0.1) {
        animator = 0;
        frame++;
        frame %= nFrames;
      }

      if (image == &slimeTex) {  // Slime AI
        speedX = 0;
        if (frame > 2 && frame < 8) {
          speedX = player.x + player.w / 2 > x + w / 2 ? speed : -speed;
        }
        if (frame == 2) {
          speedY = -tileSize * 8 * 0.25;
        } else if (frame == 8) {
          explode(x + w / 2, y + h, Color(0, 255, 64), 10);
        }
        speedY += deltaTime * tileSize * 8;
      } else {  // Arrow AI
        if (speedX == 0) speedX = tileSize * 5;
        flip = speedX < 0;
      }
    }

    x += speedX * deltaTime;
    if (collides() && (targetX == 0 || targetY == 0)) {
      while (collides()) {
        x -= speedX > 0 ? 1 : -1;
      }
      if (image == &arrowTex) {
        speedX = -speedX;
        playSound("Sound/flip.wav");
      } else {
        speedX = 0;
      }
    }
    y += speedY * deltaTime;
    if (collides() && (targetX == 0 || targetY == 0)) {
      while (collides()) {
        y -= speedY > 0 ? 1 : -1;
      }
      speedY = 0;
    }
  }

  void draw(Window* window) { drawImage(window, image, x - cameraX, y - cameraY, image->width / nFrames * frame, 0, image->width / nFrames, image->height, w, h, flip); }
};

std::vector<Creature> creatures[nLevels];
