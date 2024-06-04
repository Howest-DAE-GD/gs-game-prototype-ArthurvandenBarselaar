#include "pch.h"
#include "Sheep.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Game.h"
#include "utils.h"


Sheep::Sheep(const Vector2f& pos, Vector2f* collectorPtr, Vector2f* playerPos, std::vector<Sheep*>* allSheep):
    m_Position(pos),
    m_Collector(collectorPtr),
    m_PlayerPos(playerPos),
    m_SheepCollection(allSheep)
{
    m_RandomPickedTarget = Vector2f{utils::Random(50.0f, Game::m_WindowSize.x - 50.0f), utils::Random(50.0f, Game::m_WindowSize.y- 50.0f)};
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

Vector2f Sheep::Boundaries()
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

Vector2f Sheep::Wandering()
{
    if ((m_RandomPickedTarget - m_Position).Length() < 10)
    {
        m_RandomPickedTarget = Vector2f{utils::Random(50.0f, Game::m_WindowSize.x - 50.0f), utils::Random(50.0f, Game::m_WindowSize.y- 50.0f)};
    }

    Vector2f desired = m_RandomPickedTarget - m_Position;
    desired = desired.Normalized() * m_RelaxSpeed;
    Vector2f steer = desired - m_Velocity;
    steer = steer.Normalized() * m_MaxForce;

    return steer;
}

void Sheep::Update(const float deltaTime)
{
    if(!m_IsSave)
    {
        if((*m_Collector - m_Position).Length() < 50)
        {
            m_IsSave = true;
        }
    }
    else if(m_IsSave)
    {
        m_Velocity += MoveToCollector();
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
    m_Velocity += Aligment() * 2;
    m_Velocity += Cohesion();
    
    m_Velocity += Boundaries() * 50;

    m_Velocity = m_Velocity.Limit(m_MaxSpeed);
    
    m_Position += m_Velocity * deltaTime;
}

void Sheep::Draw() const
{
    if(Game::m_TimeYouHaveLeft < 0 && !m_IsSave)
    {
        Explode(-Game::m_TimeYouHaveLeft);
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
    }

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


Vector2f Sheep::Aligment() const
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

void Sheep::Explode(float timeDead) const
{
    if(timeDead > 1) timeDead = 1;
    utils::SetColor({1,0,0,1});
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{50, 0}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{-50, 0}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{0, 50}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
    utils::DrawEllipse(Vector2f::Lerp(m_Position, m_Position + Vector2f{0, -50}, sin((timeDead * M_PI) / 2.0f)), 10 , 10);
}
