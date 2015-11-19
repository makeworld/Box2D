#include "b2thread.h"

B2Thread::B2Thread(QObject* parent)
  : QThread(parent)
  , m_b2world(b2Vec2(0.0f, -10.0f))
  , m_playing(true)
{
}

B2Thread::~B2Thread()
{
  quit();
  wait();
}

void B2Thread::init()
{
  QMutexLocker locker(&m_mutex);

  m_b2world.SetDebugDraw(this);
  SetFlags(e_shapeBit);

  //
  // graound
  //

  b2BodyDef bodyDef;
  bodyDef.type = b2_staticBody;
  bodyDef.position.Set(5.0f, 1.0f);
  b2Body* body = m_b2world.CreateBody(&bodyDef);

  b2PolygonShape polygonShape;
  polygonShape.SetAsBox(4.0f, 0.1f);

  body->CreateFixture(&polygonShape, 0.0f);
}

void B2Thread::add(float x, float y)
{
  QMutexLocker locker(&m_mutex);

  //
  // triangle
  //

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(x, y);
  b2Body* body = m_b2world.CreateBody(&bodyDef);

  const float radius = 0.5f;
  b2Vec2 vertices[3];
  vertices[0].Set(radius * qCos(M_PI/2.0f), radius * qSin(M_PI/2.0f));
  vertices[1].Set(radius * qCos(M_PI*7.0f/6.0f), radius * qSin(M_PI*7.0f/6.0f));
  vertices[2].Set(radius * qCos(M_PI*11.0f/6.0f), radius * qSin(M_PI*11.0f/6.0f));

  b2PolygonShape polygonShape;
  polygonShape.Set(vertices, sizeof(vertices)/sizeof(vertices[0]));

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &polygonShape;
  fixtureDef.friction = 0.3f;
  fixtureDef.restitution = 0.7f;
  fixtureDef.density = 1.0f;

  body->CreateFixture(&fixtureDef);
}

void B2Thread::run()
{
  init();

  float32 timeStep = 1.0f / 60.0f;
  int32 velocityIterations = 8;
  int32 positionIterations = 3;

  QElapsedTimer timer;
  timer.start();

  for(int step = 0; m_playing; ++step){
    {
      QMutexLocker locker(&m_mutex);
      m_b2world.Step(timeStep, velocityIterations, positionIterations);
    }

    emit preDraw();

    {
      QMutexLocker locker(&m_mutex);
      m_b2world.DrawDebugData();
    }

    qint64 remain = static_cast<qint64>(step * timeStep * 1000.0f - timer.elapsed());
    if(remain > 0){
      msleep(remain);
    }

    emit postDraw();
  }
}
