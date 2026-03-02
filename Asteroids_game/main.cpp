#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <random>

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
	float w, h;

public:
	Entity(float _x, float _y, float _w, float _h)
		: x(_x), y(_y), w(_w), h(_h) {
	}

	virtual ~Entity() = default;

	float getX() const { return x; }
	float getY() const { return y; }
	float getW() const { return w; }
	float getH() const { return h; }
	float getCenterY() const {
		return y + h / 2.0f;
	}

	void setX(float _x) { x = _x; }
	void setY(float _y) { y = _y; }

	SDL_FRect getRect() const {
		return { x, y, w, h };
	}

	virtual void Draw(SDL_Renderer* renderer) const = 0;
};


int main(int argc, char* argv[]) {

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL error: %s", SDL_GetError());
		return 1;
	}

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	if (!SDL_CreateWindowAndRenderer("SDL3 Pingpong", windowW, windowH, 0, &window, &renderer)) {
		return 1;
	}
	SDL_SetRenderVSync(renderer, 1);

	bool isRunning = true;
	Uint64 lastTime = SDL_GetTicks();
	SDL_Event event;

	while (isRunning) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				isRunning = false;
			}
		}

		Uint64 currentTime = SDL_GetTicks();
		float DeltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;

		const bool* keys = SDL_GetKeyboardState(nullptr);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		//myBall.Draw(renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}