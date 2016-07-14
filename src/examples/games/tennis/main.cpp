#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

int ballSpeed = 200;
int platformSpeed = 300;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        randomize();
        design = deserialize<CanvasLayout>("tennis\\Design.json");
        m_view->addObject(design);
        
        ball = design->getChild<StaticTextureRect>("#ball");
        blue = design->getChild<StaticTextureRect>("#blue");
        red = design->getChild<StaticTextureRect>("#red");
        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
    }

    void postload()
    {
        gameBox = design->getChild<CanvasLayout>("#area")->box();
        restart();
    }

    void restart()
    {
        gameover = false;
        ball->setOffset(Vec2(0, 0));
        design->getChild<StaticLabel>("#win_blue")->setVisible(false);
        design->getChild<StaticLabel>("#win_red")->setVisible(false);
        redScore = 0;
        blueScore = 0;
        design->getChild<StaticLabel>("#score_blue")->setText("0");
        design->getChild<StaticLabel>("#score_red")->setText("0");
        blue->setOffset(Vec2(0, blue->getOffset().y));
        red->setOffset(Vec2(0, red->getOffset().y));
        dir = 1;
        next();
    }

    void move()
    {
        if (gameover)
            return;

        auto time = timeDelta();
        auto ballPos = ball->getOffset();
        ballPos += velocity * time;
        ball->setOffset(ballPos);

        auto touch = ballPos.x < gameBox.bottomLeft.x + 16;
        if (touch && !leftTouch)
            velocity.x = -velocity.x;
        leftTouch = touch;

        touch = ballPos.x > gameBox.topRight.x - 16;
        if (touch && !rightTouch)
            velocity.x = -velocity.x;
        rightTouch = touch;

        if (ballPos.y < gameBox.bottomLeft.y + 16)
        {
            redScore++;
            design->getChild<StaticLabel>("#score_red")->setText(toString(redScore));
            if (redScore == 10)
            {
                gameover = true;
                design->getChild<StaticLabel>("#win_red")->setVisible(true);
            }
            next();
        }
        if (ballPos.y > gameBox.topRight.y - 16)
        {
            blueScore++;
            design->getChild<StaticLabel>("#score_blue")->setText(toString(blueScore));
            if (blueScore == 10)
            {
                gameover = true;
                design->getChild<StaticLabel>("#win_blue")->setVisible(true);
            }
            next();
        }

        touch = ballPos.y > blue->getOffset().y && isTouching(blue);
        if (touch && !blueTouch)
        {
            velocity.y = -velocity.y;
        }
        blueTouch = touch;

        touch = ballPos.y < red->getOffset().y && isTouching(red);
        if (touch && !redTouch)
        {
            velocity.y = -velocity.y;
        }
        redTouch = touch;
        
        if (m_inputRegister.keys.isPressed('a'))
        {
            auto pos = red->getOffset();
            pos.x -= time * platformSpeed;
            if (pos.x >= gameBox.bottomLeft.x + 64)
                red->setOffset(pos);
        }
        if (m_inputRegister.keys.isPressed('d'))
        {
            auto pos = red->getOffset();
            pos.x += time * platformSpeed;
            if (pos.x <= gameBox.topRight.x - 64)
                red->setOffset(pos);
        }

        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Left))
        {
            auto pos = blue->getOffset();
            pos.x -= time * platformSpeed;
            if (pos.x >= gameBox.bottomLeft.x + 64)
                blue->setOffset(pos);
        }
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Right))
        {
            auto pos = blue->getOffset();
            pos.x += time * platformSpeed;
            if (pos.x <= gameBox.topRight.x - 64)
                blue->setOffset(pos);
        }
    }

    bool isTouching(StaticTextureRect* platform)
    {
        auto ballPos = ball->getOffset();
        auto platformPos = platform->getOffset();
        if (ballPos.x >= platformPos.x - 48 && ballPos.x <= platformPos.x + 48)
        {
            if (ballPos.y > platformPos.y - 32 && ballPos.y < platformPos.y + 32)
                return true;
        } else {
            auto left = ballPos - platformPos - Vec2(-48, 0);
            if (left.length() < 32)
                return true;
            auto right = ballPos - platformPos - Vec2(48, 0);
            if (right.length() < 32)
                return true;
        }
        return false;
    }

    void next()
    {
        dir = -dir;
        velocity = Vec2(ballSpeed * (1 - 2 * randomInt(0, 1)), ballSpeed * dir);
        ball->setOffset(Vec2(0, 0));
        leftTouch = false;
        rightTouch = false;
        blueTouch = false;
        redTouch = false;
    }

    shared_ptr<CanvasLayout> design;

    StaticTextureRect* ball;
    StaticTextureRect* blue;
    StaticTextureRect* red;

    BoundingBox gameBox;
    Vec2 velocity;
    bool leftTouch;
    bool rightTouch;
    bool blueTouch;
    bool redTouch;

    bool gameover;
    int redScore;
    int blueScore;
    int dir;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
