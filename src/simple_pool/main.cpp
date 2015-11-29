#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/ObjectConstructTools.h>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        design = deserialize<CanvasLayout>("simple_pool\\Design.json");
        m_view->addObject(design);
        
        ball = design->getChild<ObjectConstruct>("#ball");
        ball->setCallback(bind(&MyApp::enterStrikeMode, this));
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

        ballRadius = ball->width() / 2;

        restart();
    }

    void restart()
    {
        ball->setOffset(Vec2(0, 0));
        ball->resetAllChannels();
        ball->runAnimation("appear");
        ball->enable();
        strikeMode = false;
        cue->setVisible(false);
        velocity = Vec2(0, 0);
        topTouch = false;
        bottomTouch = false;
        for (int i = 0; i < 2; ++i)
        {
            leftTouch[i] = false;
            rightTouch[i] = false;
        }
    }

    void enterStrikeMode()
    {
        if (velocity.isZero())
        {
            strikeMode = true;
            force = 0;
        }
    }

    void move()
    {
        float time = timeDelta();

        Vec2 ballPos = ball->getOffset();
        ballPos += velocity * time;
        velocity *= (1.0 - time * 0.3);
        if (velocity.length() < 5)
            velocity = Vec2(0, 0);
        ball->setOffset(ballPos);

        if (strikeMode)
        {
            cue->setVisible(true);
            Vec2 mousePos = m_inputRegister.mousePosition();
            Vec2 delta = mousePos - ball->fullTransform().offset;

            float angle = 3.14 + delta.angle();
            Vec2 offset((ball->width() + cue->width()) / 2 + force / 10, 0);
            offset.setAngle(angle);
            offset += ballPos;
            cue->setOffset(offset);
            cue->setAngle(angle);

            if (m_inputRegister.mouseButtons.isPressed(MouseButton::Right))
            {
                force += time * 200;
                if (force > 500)
                    force = 500;
            }
            if (m_inputRegister.mouseButtons.isJustOutpressed(MouseButton::Right))
            {
                strikeMode = false;
                cue->setVisible(false);
                velocity = Vec2(force, 0);
                velocity.rotate(delta.angle());
            }
        }

        for (int i = 0; i < 6; ++i)
        {
            Vec2 distance = holes[i] - ballPos;
            if (distance.length() < holeRadius)
            {
                ball->disable();
                ball->runAnimation("disappear");
                velocity = distance;
            }
        }

        {
            bool touch = isTouching(top);
            if (touch && !topTouch)
                velocity.y = -velocity.y;
            topTouch = touch;
        }
        {
            bool touch = isTouching(bottom);
            if (touch && !bottomTouch)
                velocity.y = -velocity.y;
            bottomTouch = touch;
        }

        for (int i = 0; i < 2; ++i)
        {
            {
                bool touch = isTouching(left[i]);
                if (touch && !leftTouch[i])
                    velocity.x = -velocity.x;
                leftTouch[i] = touch;
            }
            {
                bool touch = isTouching(right[i]);
                if (touch && !rightTouch[i])
                    velocity.x = -velocity.x;
                rightTouch[i] = touch;
            }
        }
    }

    bool isTouching(const BoundingBox& box)
    {
        auto ballPos = ball->getOffset();
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

    ObjectConstruct* ball;
    ObjectConstruct* cue;

    BoundingBox top;
    bool topTouch;

    BoundingBox bottom;
    bool bottomTouch;

    BoundingBox left[2];
    bool leftTouch[2];

    BoundingBox right[2];
    bool rightTouch[2];

    Vec2 holes[6];
    float holeRadius;

    float ballRadius;

    BoundingBox gameBox;
    Vec2 velocity;
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
