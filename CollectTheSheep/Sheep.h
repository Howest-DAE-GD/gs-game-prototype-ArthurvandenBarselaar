#pragma once
#include <vector>

#include "GameState.h"

class Sheep
{
public:
    Sheep(const Vector2f& pos, Vector2f* collectorPtr, Vector2f* playerPos, std::vector<Sheep*>* allSheep, GameState* currentGameState);
    void Update(float deltaTime);
    void Draw() const;

    bool m_IsSave{};
    Vector2f m_Position;
    Vector2f m_Velocity{};
    
private:
    
    float m_MaxSpeed{30};
    float m_RelaxSpeed{1};
    float m_MaxForce{2.0f};

    float m_DeadTimer{};

    Vector2f* m_Collector;
    Vector2f* m_PlayerPos;
    GameState* m_CurrentGameState;

    Vector2f RunAwayFromPlayer() const;

    Vector2f Separation() const;
    Vector2f Alignment() const;
    Vector2f Cohesion() const;
    Vector2f SeekPoint(const Vector2f& target) const;
    
    Vector2f MoveToCollector() const;
    Vector2f Boundaries() const;
    Vector2f Wandering() const;

    void GetRandomPosition();
    Vector2f m_RandomPickedTarget{};

    std::vector<Sheep*>* m_SheepCollection;

    void Explode(float timeDead) const;
    
    bool m_DoingWhatWolfSays{false};
    // float m_TimeUntilNotDoingWhatWolfSays{0};
    
};
