#include "pch.h"
#include "Sheep.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <valarray>

#include "Game.h"
#include "Texture.h"
#include "utils.h"


Sheep::Sheep(const Vector2f& pos, Vector2f* collectorPtr, Vector2f* playerPos, std::vector<Sheep*>* allSheep, GameState* currentGameState):
    m_Position(pos),
    m_Collector(collectorPtr),
    m_PlayerPos(playerPos),
    m_CurrentGameState(currentGameState),
    m_SheepCollection(allSheep)
{
    if(m_FrontTexture == nullptr)
    {
        m_FrontTexture = new Texture{"sheep_down.png"};
        m_BackTexture = new Texture{"sheep_up.png"};
        m_SideTexture = new Texture{"sheep_right_left.png"};
    }
    
    GetRandomPosition();
}
Sheep::~Sheep()
{
    delete m_FrontTexture;
    delete m_BackTexture;
    delete m_SideTexture;
}

Vector2f Sheep::SeekPoint(const Vector2f& target) const
{
    Vector2f desired = target - m_Position;
    desired = desired.Normalized() * m_MaxSpeed;
    
    Vector2f steer = desired - m_Velocity;
    steer = steer.Limit(m_MaxForce);

    return steer;
    
}
Vector2f Sheep::MoveToCollector() const
{
    return SeekPoint(*m_Collector);
}
Vector2f Sheep::Boundaries() const
{
    constexpr float offset = 50;

    Vector2f desired{};
    
    if (m_Position.x < offset) {
        desired = Vector2f(m_MaxSpeed, m_Velocity.y);
    } else if (m_Position.x > Game::m_WindowSize.x - offset) {
        desired = Vector2f(-m_MaxSpeed, m_Velocity.y);
    }

    if (m_Position.y < offset) {
        desired = Vector2f(m_Velocity.x, m_MaxSpeed);
    } else if (m_Position.y > Game::m_WindowSize.y - offset) {
        desired = Vector2f(m_Velocity.x, -m_MaxSpeed);
    }

    if(desired != Vector2f{})
    {
        desired = desired.Normalized() * m_MaxSpeed;
        return (desired - m_Velocity).Normalized() * m_MaxForce;
    }
    
    return {};
}
Vector2f Sheep::Wandering() const
{
    return {};
}
Vector2f Sheep::RunAwayFromPlayer() const
{
    return SeekPoint(m_Position + m_Position - *m_PlayerPos);
}
Vector2f Sheep::Separation() const
{
    const float distanceOfSepartion = 20;

    Vector2f sum{};
    int count{};
    
    for (const Sheep* sheep : *m_SheepCollection)
    {
        if(sheep == this) continue;

        Vector2f diff = (m_Position - sheep->m_Position);
        const float distance =  diff.Length();
        if(distance < distanceOfSepartion)
        {
            sum += diff.Normalized() * (1 / distance);
            ++count;
        }
    }

    if(count > 0)
    {
        sum = sum.Normalized() * m_MaxSpeed;

        const Vector2f steer = (sum - m_Velocity).Limit(m_MaxForce);
        return steer;
    }

    return Vector2f{};
}
Vector2f Sheep::Alignment() const
{
    const float distanceOfAlign = 50;
    Vector2f sum{};
    int count{};

    for (const Sheep* sheep : *m_SheepCollection)
    {
        if(sheep == this) continue;

        Vector2f diff = (m_Position - sheep->m_Position);
        const float distance =  diff.Length();
        if(distance < distanceOfAlign)
        {
            sum += sheep->m_Velocity;
            ++count;
        }
    }

    if(count > 0)
    {
        sum = sum.Normalized() * m_MaxSpeed;

        const Vector2f steer = (sum - m_Velocity).Limit(m_MaxForce);
        return steer;
    }
    
    return {};
}
Vector2f Sheep::Cohesion() const
{
    const float distanceOfCohesion = 50;
    Vector2f sum{};
    int count{};

    for (const Sheep* sheep : *m_SheepCollection)
    {
        if(sheep == this) continue;

        Vector2f diff = (m_Position - sheep->m_Position);
        const float distance =  diff.Length();
        if(distance < distanceOfCohesion)
        {
            sum += sheep->m_Position;
            ++count;
        }
    }

    if(count > 0)
    {
        sum = sum / static_cast<float>(count);
        return SeekPoint(sum);
    }
    
    return {};
}

void Sheep::GetRandomPosition()
{
    m_RandomPickedTarget = Vector2f{utils::Random(50.0f, Game::m_WindowSize.x - 50.0f), utils::Random(50.0f, Game::m_WindowSize.y- 50.0f)};
}

void Sheep::Update(const float deltaTime)
{
    if(*m_CurrentGameState == GameState::running)
    {
        if(!m_IsSave)
        {
            if((*m_Collector - m_Position).Length() < 50)
            {
                m_IsSave = true;
            }
        
            if((*m_PlayerPos - m_Position).Length() < 60)
            {
                m_DoingWhatWolfSays = true;
            }
            else
            {
                m_DoingWhatWolfSays = false;
            }
        
            if(m_DoingWhatWolfSays)
            {
                m_Velocity += RunAwayFromPlayer();
            }
            else
            {
                m_Velocity += Wandering();
            }

            m_Velocity += Separation() * 2;
            m_Velocity += Alignment() * 2;
            m_Velocity += Cohesion();
        
        }
        else if(m_IsSave)
        {
            m_Velocity += Separation() * 2;
            m_Velocity += MoveToCollector();
        }
    
        m_Velocity += Boundaries() * 50;
        m_Velocity = m_Velocity.Limit(m_MaxSpeed);
        m_Position += m_Velocity * deltaTime;
    }
    else if(*m_CurrentGameState == GameState::sheepDead)
    {
        m_DeadTimer += deltaTime;
        GetRandomPosition();
    }
    else if(*m_CurrentGameState == GameState::sheepGoingToField)
    {
        m_IsSave = false;
        m_DeadTimer = 0;
        m_Position = Vector2f::Lerp(m_Position, m_RandomPickedTarget, 5 * deltaTime); // haha bad
    }
    

    
}


void Sheep::Draw() const
{
    if(*m_CurrentGameState == GameState::sheepDead && !m_IsSave)
    {
        Explode(-m_DeadTimer);
    }
    else
    {
        utils::SetColor({1,1,1,1});
        if(m_IsSave)
        {
            utils::SetColor({0,1,0,1});
        }
        else if(m_DoingWhatWolfSays)
        {
            utils::SetColor({0,1,1,1});
            utils::DrawLine(m_Position, m_Position + m_Velocity.Normalized()*10);
        }
        utils::DrawEllipse(m_Position, 10, 10);
        
        glPushMatrix();
        glTranslatef(m_Position.x, m_Position.y, 0);
        glScalef(0.3f, 0.3f,0.3f);
        const float rotation = std::atan2(m_Velocity.y, m_Velocity.x);;
        if(rotation > 0.0f)
        {
            if(rotation > 1.0/4*M_PI && rotation < 3.0/4*M_PI)
            {
                m_BackTexture->Draw({-50,-50});
            }
            else
            {
                if(rotation > 1/2*M_PI)
                {
                    glScalef(-1,1,1);
                    m_SideTexture->Draw({-50,-50});
                }else
                {
                    m_SideTexture->Draw({-50,-50});
                }
            }
        }
        else
        {
            if(rotation < -1.0/4*M_PI && rotation > -3.0/4*M_PI)
            {
                m_FrontTexture->Draw({-50,-50});
            }
            else
            {
                if(rotation > -1/2*M_PI)
                {
                    glScalef(-1,1,1);
                    m_SideTexture->Draw({-50,-50});
                }else
                {
                    m_SideTexture->Draw({-50,-50});
                }
            }
        }

        glPopMatrix();


        

    }

}
void Sheep::Explode(float timeDead) const
{
    if(timeDead > 1) timeDead = 1;
    utils::SetColor({1,0,0,1});
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{50, 0}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{-50, 0}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{0, 50}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{0, -50}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
}
