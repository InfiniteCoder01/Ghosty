// clang-format off
std::string levels[] = {
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "                                                                                       F"
  "     T                                             #                                   F"
  "                                                   #                                   F"
  "                                                   #                                   F"
  "                                                   #                                   F"
  "                                                   #                                   F"
  "                                              ######                                   F"
  "                                                 S #                                   F"
  "                                                   #                                   F"
  "########################################################################################",
  /**************************************************************************************************************/
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P                                                                              #                             F"
  "P            #                                         #                       #                             F"
  "P            #  A                                      #                       #                             F"
  "P            # A                                       #                       #                             F"
  "P            #  A                                      #               B       #                             F"
  "P            #                                         #                       #              C              F"
  "P    ##################################################################################################---####"
  "P                                                                                                            F"
  "P                                                                                                            F"
  "######################                 ####################                                                  F"
  "######################LLLLLLLLLLLLLLLLL#####################                                                 F"
  "######################LLLLLLLLLLLLLLLLL######################                                           #    F"
  "######################LLLLLLLLLLLLLLLLL#######################                                         ###   F"
  "######################LLLLLLLLLLLLLLLLL########################                                       #####  F"
  "######################LLLLLLLLLLLLLLLLL#######################################################################",
  /**************************************************************************************************************/
  "P                                                                                      F"
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                   H                   "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                                                                                       "
  "P                       h                                                               "
  "P              S S            S                                                         "
  "########################################################################################",
};
// clang-format on

int tileSize = 32, cameraX = 0, cameraY = 0, levelIndex = 0, nextLevelIndex = -1;
bool particlesEnabled = true, playerEnabled = true, renderingEnabled = true;
int levelWidth, levelHeight;
const int nLevels = 3;

void resizeLevel() {
  levelWidth = levels[levelIndex].find_first_of("F") + 1;
  levelHeight = levels[levelIndex].length() / levelWidth;
}

char getTile(int x, int y) {
  if (x < 0 || x > levelWidth || y < 0 || y > levelHeight) return ' ';
  return levels[levelIndex][y * levelWidth + x];
}

void setTile(int x, int y, char tile) {
  if (x < 0 || x > levelWidth || y < 0 || y > levelHeight) return;
  levels[levelIndex][y * levelWidth + x] = tile;
}

float random(float min, float max) { return rand() % 1000 * (max - min) / 1000 + min; }

struct Particle {
  float x, y, velX, velY, time;
  int size = 2;
  bool gravity;
  Color color;

  Particle() = default;
  Particle(int x, int y, float velX, float velY, bool gravity, float time, Color color) : x(x), y(y), velX(velX), velY(velY), gravity(gravity), time(time), color(color) {}

  void update(float deltaTime) {
    time -= deltaTime;
    if (gravity) velY += deltaTime * 100;
    x += velX * deltaTime;
    y += velY * deltaTime;
  }

  void draw(Window* window) { fillRect(window, x - cameraX - size / 2, y - cameraY - size / 2, size, size, color); }
};

std::vector<Particle> particles;

void explode(int x, int y, Color color, int count = 100) {
  if(!particlesEnabled) return;
  for (int i = 0; i < count; i++) {
    float angle = random(0, 3.1415926 * 2);
    float velocity = random(0, tileSize * 1.5);
    particles.push_back(Particle(x, y, sin(angle) * velocity, cos(angle) * velocity, true, random(1.5, 2.5), color));
  }
}
