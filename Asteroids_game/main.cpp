#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <random>
#include <cmath>

constexpr int windowH = 800;
constexpr int windowW = 1000;

int getRandomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

class Entity {
protected:
    float x, y;
    float vx, vy;
    float angle;
    float radius;
    bool active;

public:
    Entity(float _x, float _y, float _radius);
    virtual ~Entity() = default;

    float getX() const { return x; }
    float getY() const { return y; }
    float getRadius() const { return radius; }
    bool isActive() const { return active; }

    void destroy();

    void wrapScreen(int screenWidth, int screenHeight);

    bool checkCollision(const Entity& other) const;

    virtual void Update(float dt) = 0;
    virtual void Draw(SDL_Renderer* renderer) const = 0;
};

class Ship : public Entity {
private:
    float rotationSpeed;
    float thrustPower;
    float maxSpeed;
    float shootCooldown;
    float currentCooldown;

public:
    Ship(float _x, float _y);

    void RotateLeft(float dt);
    void RotateRight(float dt);
    void Thrust(float dt);

    bool CanShoot();

    void Update(float dt) override;
    void Draw(SDL_Renderer* renderer) const override;
};

class Bullet : public Entity {
private:
    float lifeTime;

public:
    Bullet(float _x, float _y, float _angle);

    void Update(float dt) override;
    void Draw(SDL_Renderer* renderer) const override;
};

class Asteroid : public Entity {
private:
    int size;
    float rotationSpeed;

public:
    Asteroid(float _x, float _y, int _size);

    void Update(float dt) override;
    void Draw(SDL_Renderer* renderer) const override;
};


int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!SDL_CreateWindowAndRenderer("SDL3 Asteroids", windowW, windowH, 0, &window, &renderer)) {
        return 1;
    }
    SDL_SetRenderVSync(renderer, 1);

    Ship player(windowW / 2.0f, windowH / 2.0f);
    std::vector<Bullet> bullets;
    std::vector<Asteroid> asteroids;

    for (int i = 0; i < 5; ++i) {
        asteroids.emplace_back(getRandomInt(0, windowW), getRandomInt(0, windowH), 3);
    }

    bool isRunning = true;
    Uint64 lastTime = SDL_GetTicks();
    SDL_Event event;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }
        }

        const bool* keys = SDL_GetKeyboardState(nullptr);

        Uint64 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        if (keys[SDL_SCANCODE_LEFT])  player.RotateLeft(dt);
        if (keys[SDL_SCANCODE_RIGHT]) player.RotateRight(dt);
        if (keys[SDL_SCANCODE_UP])    player.Thrust(dt);

        if (keys[SDL_SCANCODE_SPACE] && player.CanShoot()) {
            bullets.emplace_back(player.getX(), player.getY(), 0.0f);
        }

        player.Update(dt);
        player.wrapScreen(windowW, windowH);

        for (auto& b : bullets) {
            b.Update(dt);
            b.wrapScreen(windowW, windowH);
        }

        for (auto& a : asteroids) {
            a.Update(dt);
            a.wrapScreen(windowW, windowH);
        }

        //видалення куль та поділ астероїдів

        std::erase_if(bullets, [](const Bullet& b) { return !b.isActive(); });
        std::erase_if(asteroids, [](const Asteroid& a) { return !a.isActive(); });

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        player.Draw(renderer);
        for (const auto& b : bullets) b.Draw(renderer);
        for (const auto& a : asteroids) a.Draw(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
