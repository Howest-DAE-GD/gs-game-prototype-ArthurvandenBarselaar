#pragma once
#include <vector>

#include "BaseGame.h"
#include "Sheep.h"
#include "Vector2f.h"

class Texture;

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

	static Vector2f m_WindowSize;
	static float m_TimeYouHaveLeft;

	int sheepAlive{50};
	int sheepCurrentlyAlive{};

	float timer2{};
	bool deadScreen{};
	bool GameOver{};
	
private:

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;
	void Restart();

	Vector2f m_PlayerPosition;

	Vector2f m_Collector;
	Texture* GameOverText;

	static std::vector<Sheep*> m_Sheep;
};
