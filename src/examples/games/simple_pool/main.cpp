#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/ObjectConstructTools.h>

using namespace gamebase;
using namespace std;

const int N = 2;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        design = deserialize<CanvasLayout>("simple_pool\\Design.json");
        m_view->addObject(design);
        
        ball[0] = design->getChild<ObjectConstruct>("#ball");
        ball[0]->setCallback(bind(&MyApp::enterStrikeMode, this));

        auto ball1 = deserialize<ObjectConstruct>("simple_pool\\Ball.json");
        design->getChild<CanvasLayout>("#canvas")->addObject(ball1);
        ball[1] = ball1.get();

        cue = design->getChild<ObjectConstruct>("#cue");
        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
    }

    void postload()
    {
        for (int i = 0; i < 6; ++i)
            holes[i] = design->getChild<StaticTextureRect>("#hole" + toString(i))->getOffset();
        holeRadius = design->getChild<StaticTextureRect>("#hole0")->width() / 2;

        for (int i = 0; i < 2; ++i)
        {
            left[i] = design->getChild<StaticTextureRect>("#left" + toString(i))->movedBox();
            right[i] = design->getChild<StaticTextureRect>("#right" + toString(i))->movedBox();
        }
        top = design->getChild<StaticTextureRect>("#top")->movedBox();
        bottom = design->getChild<StaticTextureRect>("#bottom")->movedBox();

        ballRadius = ball[0]->width() / 2;

        restart();
    }

    void restart()
    {
        for (int i = 0; i < N; ++i)
        {
            ball[i]->resetAllChannels();
            ball[i]->runAnimation("appear");
            ball[i]->enable();
            velocity[i] = Vec2(0, 0);
            inGame[i] = true;
            ball[i]->setAngle(0);
        }
        ball[0]->setOffset(Vec2(0, 150));
        ball[1]->setOffset(Vec2(0, 200));

        strikeMode = false;
        cue->setVisible(false);
    }

    void enterStrikeMode()
    {
        for (int i = 0; i < N; ++i)
            if (inGame[i] && !velocity[i].isZero())
                return;
        
        strikeMode = true;
        force = 0;
    }

    void move()
    {
        float time = timeDelta();

        for (int i = 0; i < N; ++i)
        {
            Vec2 ballPos = ball[i]->getOffset();
            auto change = velocity[i] * time;
            ballPos += change;
            ball[i]->setAngle(ball[i]->angle() + change.length() / 40);
            velocity[i] *= (1.0 - time * 0.3);
            if (velocity[i].length() < 5)
                velocity[i] = Vec2(0, 0);
            ball[i]->setOffset(ballPos);

            if (strikeMode && i == 0)
            {
                cue->setVisible(true);
                Vec2 mousePos = m_inputRegister.mousePosition();
                Vec2 delta = ball[i]->fullTransform().offset - mousePos;

                float angle = 3.14 + delta.angle();
                Vec2 offset((ball[i]->width() + cue->width()) / 2 + force / 10, 0);
                offset.setAngle(angle);
                offset += ballPos;
                cue->setOffset(offset);
                cue->setAngle(angle);

                if (m_inputRegister.mouseButtons.isPressed(MouseButton::Right))
                {
                    force += time * 300;
                    if (force > 500)
                        force = 500;
                }
                if (m_inputRegister.mouseButtons.isJustOutpressed(MouseButton::Right))
                {
                    strikeMode = false;
                    cue->setVisible(false);
                    velocity[i] = Vec2(force, 0);
                    velocity[i].rotate(delta.angle());
                }
            }

            for (int j = 0; j < 6; ++j)
            {
                Vec2 distance = holes[j] - ballPos;
                if (distance.length() < holeRadius)
                {
                    ball[i]->disable();
                    ball[i]->runAnimation("disappear");
                    inGame[i] = false;
                    velocity[i] = distance;
                }
            }

            if (isTouching(ballPos, top) && velocity[i].y > 0)
                velocity[i].y = -velocity[i].y;
            if (isTouching(ballPos, bottom) && velocity[i].y < 0)
                velocity[i].y = -velocity[i].y;

            for (int j = 0; j < 2; ++j)
            {
                if (isTouching(ballPos, left[j]) && velocity[i].x < 0)
                    velocity[i].x = -velocity[i].x;
                if (isTouching(ballPos, right[j]) && velocity[i].x > 0)
                    velocity[i].x = -velocity[i].x;
            }
        }

        if (inGame[0] && inGame[1])
        {
            auto ballPos0 = ball[0]->getOffset();
            auto ballPos1 = ball[1]->getOffset();
            auto posDelta = ballPos0 - ballPos1;
            if (posDelta.length() < 2 * ballRadius)
            {
                auto velDelta = velocity[0] - velocity[1];
                posDelta.normalize();
                auto relativeVelocity = dot(posDelta, velDelta);
                if (relativeVelocity < 0)
                {
                    velocity[0] -= relativeVelocity * posDelta;
                    velocity[1] += relativeVelocity * posDelta;
                }
            }
        }
    }

    bool isTouching(const Vec2& ballPos, const BoundingBox& box)
    {
        if (box.contains(Vec2(ballPos.x - ballRadius, ballPos.y)))
            return true;
        if (box.contains(Vec2(ballPos.x + ballRadius, ballPos.y)))
            return true;
        if (box.contains(Vec2(ballPos.x, ballPos.y - ballRadius)))
            return true;
        if (box.contains(Vec2(ballPos.x, ballPos.y + ballRadius)))
            return true;
        if (dist(box.bottomLeft, ballPos) < ballRadius)
            return true;
        if (dist(box.topRight, ballPos) < ballRadius)
            return true;
        if (dist(Vec2(box.bottomLeft.x, box.topRight.y), ballPos) < ballRadius)
            return true;
        if (dist(Vec2(box.topRight.x, box.bottomLeft.y), ballPos) < ballRadius)
            return true;
        return false;
    }

    shared_ptr<CanvasLayout> design;

    ObjectConstruct* ball[2];
    Vec2 velocity[2];
    bool inGame[2];

    ObjectConstruct* cue;

    BoundingBox top;
    BoundingBox bottom;
    BoundingBox left[2];
    BoundingBox right[2];

    Vec2 holes[6];
    float holeRadius;

    float ballRadius;

    BoundingBox gameBox;
    float force;
    bool strikeMode;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
