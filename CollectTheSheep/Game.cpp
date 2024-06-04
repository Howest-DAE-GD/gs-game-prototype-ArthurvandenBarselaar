#include "pch.h"
#include "Game.h"

#include "SVGParser.h"
#include "Texture.h"
#include "utils.h"

Vector2f Game::m_WindowSize{};
std::vector<Sheep*> Game::m_Sheep;
float Game::m_TimeYouHaveLeft{40.0f};

Game::Game( const Window& window ) 
	:BaseGame{ window },
	m_Collector(window.width/2, window.height/2)
{
	m_WindowSize = {window.width, window.height};
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	Restart();

}


void Game::Cleanup( )
{
}

void Game::Update( float elapsedSec )
{
	// Check keyboard state

	float speed = 100;

	Vector2f inputKeyboard;

	m_TimeYouHaveLeft -= elapsedSec;
	timer2 -= elapsedSec;

	if(m_TimeYouHaveLeft < 0 && deadScreen == false)
	{
		timer2 = 5;
		deadScreen = true;
		
		sheepAlive = 0;

		for (int i{}; i < m_Sheep.size(); ++i)
		{
			if(m_Sheep[i]->m_IsSave)
				++sheepAlive;
		}

		if(sheepAlive <= 0 || sheepAlive >= m_Sheep.size())
		{
			GameOver = true;
			GameOverText = new Texture("HighScore: " + std::to_string(sheepCurrentlyAlive), "arial.ttf", 64, Color4f{1,1,1,1});
		}
	}

	if(deadScreen && timer2 < 0)
	{
		if(GameOver) return;
		Restart();
	}

	if(!deadScreen)
	{
		const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
		if ( pStates[SDL_SCANCODE_RIGHT] )
		{
			inputKeyboard.x += 1;
		}
		if ( pStates[SDL_SCANCODE_LEFT])
		{
			inputKeyboard.x += -1;
		}
		if ( pStates[SDL_SCANCODE_UP] )
		{
			inputKeyboard.y += 1;
		}
		if ( pStates[SDL_SCANCODE_DOWN])
		{
			inputKeyboard.y += -1;
		}
		inputKeyboard = inputKeyboard.Normalized();
		inputKeyboard = inputKeyboard * speed * elapsedSec;
		
		m_PlayerPosition += inputKeyboard;
	}

	sheepCurrentlyAlive = 0;
	
	for (int i{}; i < m_Sheep.size(); ++i)
	{
		m_Sheep[i]->Update(elapsedSec);
		
		if((m_PlayerPosition - m_Sheep[i]->m_Position).Length() < 40)
		{
			m_Sheep[i]->m_Position -= (m_PlayerPosition - m_Sheep[i]->m_Position).Normalized();
		}

		if(m_Sheep[i]->m_IsSave) ++sheepCurrentlyAlive;
	}
	
}

void Game::Draw( ) const
{
	ClearBackground( );

	utils::SetColor({1,0,0,1});
	utils::DrawEllipse(m_PlayerPosition, 30, 30);

	for (int i{}; i < m_Sheep.size(); ++i)
	{
		m_Sheep[i]->Draw();
	}

	utils::SetColor({0,0,1,1});
	utils::DrawEllipse(m_Collector, 50, 50);

	
	utils::SetColor({1,0,0,1});
	utils::FillRect(10, m_WindowSize.y- 30, m_TimeYouHaveLeft / 40 * m_WindowSize.x - 10, 30);

	if(GameOver)
	{
		GameOverText->Draw(Vector2f{30, 30});
		// Something
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::ClearBackground( ) const
{
	glClearColor( 30/255.0f, 30/255.0f, 30/255.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::Restart()
{
	m_Sheep.clear();
	
	m_PlayerPosition = Vector2f{100,100};
	m_TimeYouHaveLeft = 40.0f;
	
	timer2 = 0;
	deadScreen = false;
	
	for (int i{}; i < sheepAlive; ++i)
	{
		m_Sheep.push_back(new Sheep{{utils::Random(0.0f, m_WindowSize.x), utils::Random(0.0f, m_WindowSize.y)}, &m_Collector, &m_PlayerPosition, &m_Sheep});
	}
}
