#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/ObjectConstructTools.h>
#include <gamebase/engine/Timer.h>

using namespace gamebase;
using namespace std;

const int N = 16;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        design = deserialize<CanvasLayout>("pool\\Design.json");
        m_view->addObject(design);
        
        ball[0] = design->getChild<ObjectConstruct>("#ball");
        ball[0]->setCallback(bind(&MyApp::enterStrikeMode, this));

        for (int i = 1; i < N; ++i)
        {
            auto newBall = deserialize<ObjectConstruct>("pool\\Ball.json");
            newBall->getChild<StaticTextureRect>("#ball")->setColor(Color(1, 1, 1));
            newBall->getChild<StaticLabel>("#label")->setText(toString(i));
            design->getChild<CanvasLayout>("#canvas")->addObject(newBall);
            ball[i] = newBall.get();
        }

        stepLabels[FirstPlayer] = design->getChild<StaticLabel>("#step1");
        stepLabels[SecondPlayer] = design->getChild<StaticLabel>("#step2");

        winLabels[FirstPlayer] = design->getChild<StaticLabel>("#win1");
        winLabels[SecondPlayer] = design->getChild<StaticLabel>("#win2");

        scoreLabels[FirstPlayer] = design->getChild<StaticLabel>("#score1");
        scoreLabels[SecondPlayer] = design->getChild<StaticLabel>("#score2");

        design->getChild<StaticLabel>("#filled1")->setVisible(false);
        design->getChild<StaticLabel>("#filled2")->setVisible(false);

        design->getChild<StaticLabel>("#striped1")->setVisible(false);
        design->getChild<StaticLabel>("#striped2")->setVisible(false);

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
        ball[0]->setOffset(Vec2(0, -200));

        ball[1]->setOffset(Vec2(0, 100));

        ball[13]->setOffset(Vec2(-16, 125));
        ball[7]->setOffset(Vec2(16, 125));

        ball[2]->setOffset(Vec2(-32, 150));
        ball[8]->setOffset(Vec2(0, 150));
        ball[14]->setOffset(Vec2(32, 150));

        ball[15]->setOffset(Vec2(-48, 175));
        ball[4]->setOffset(Vec2(-16, 175));
        ball[9]->setOffset(Vec2(16, 175));
        ball[6]->setOffset(Vec2(48, 175));
        
        ball[11]->setOffset(Vec2(-64, 200));
        ball[3]->setOffset(Vec2(-32, 200));
        ball[12]->setOffset(Vec2(0, 200));
        ball[5]->setOffset(Vec2(32, 200));
        ball[10]->setOffset(Vec2(64, 200));

        strikeMode = false;
        cue->setVisible(false);

        for (int i = 0; i < 2; ++i)
        {
            stepLabels[i]->setVisible(false);
            winLabels[i]->setVisible(false);
            scoreLabels[i]->setText("0");
            score[i] = 0;
        }
        stepLabels[0]->setVisible(true);

        curPlayer = FirstPlayer;
        otherPlayer = SecondPlayer;
        moving = false;
        changePlayer = false;
        fault = false;
    }

    void enterStrikeMode()
    {
        if (moving)
            return;
        
        strikeMode = true;
        force = 0;
    }

    void move()
    {
        if (!moving && fault && timer.time() > 1500)
        {
            fault = false;
            ball[0]->resetAllChannels();
            ball[0]->runAnimation("appear");
            ball[0]->enable();
            velocity[0] = Vec2(0, 0);
            inGame[0] = true;
            ball[0]->setAngle(0);
            ball[0]->setOffset(Vec2(0, -200));
        }

        float time = timeDelta();

        for (int i = 0; i < N; ++i)
        {
            Vec2 ballPos = ball[i]->getOffset();
            auto change = velocity[i] * time;
            ballPos += change;
            ball[i]->setAngle(ball[i]->angle() + change.length() / 10);
            velocity[i] *= (1.0 - time * 0.3);
            if (velocity[i].length() < 5)
                velocity[i] = Vec2(0, 0);
            ball[i]->setOffset(ballPos);

            if (inGame[i])
            {
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

                        changePlayer = score[0] != 8 && score[1] != 8;
                        moving = true;
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

                        if (i == 0)
                        {
                            if (score[0] != 8 && score[1] != 8)
                            {
                                timer.start();
                                fault = true;
                                changePlayer = true;
                                cout << "Started fault timer" << endl;
                            }
                        }
                        else
                        {
                            score[curPlayer]++;
                            scoreLabels[curPlayer]->setText(toString(score[curPlayer]));
                            if (!fault)
                                changePlayer = false;
                            if (score[curPlayer] >= 8)
                            {
                                stepLabels[curPlayer]->setVisible(false);
                                winLabels[curPlayer]->setVisible(true);
                                fault = false;
                                changePlayer = false;
                            }
                        }
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
        }

        for (int i = 0; i < N; ++i)
        {
            for (int j = i + 1; j < N; ++j)
            {
                if (inGame[i] && inGame[j])
                {
                    auto ballPosI = ball[i]->getOffset();
                    auto ballPosJ = ball[j]->getOffset();
                    auto posDelta = ballPosI - ballPosJ;
                    if (posDelta.length() < 2 * ballRadius)
                    {
                        auto velDelta = velocity[i] - velocity[j];
                        posDelta.normalize();
                        auto relativeVelocity = dot(posDelta, velDelta);
                        if (relativeVelocity < 0)
                        {
                            velocity[i] -= relativeVelocity * posDelta;
                            velocity[j] += relativeVelocity * posDelta;
                            break;
                        }
                    }
                }
            }
        }

        bool curMoving = false;
        for (int i = 0; i < N; ++i)
            if (inGame[i] && !velocity[i].isZero())
                curMoving = true;
        if (moving && !curMoving)
        {
            moving = false;
            if (changePlayer)
            {
                stepLabels[curPlayer]->setVisible(false);
                stepLabels[otherPlayer]->setVisible(true);
                swap(curPlayer, otherPlayer);
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

    ObjectConstruct* ball[N];
    Vec2 velocity[N];
    bool inGame[N];

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

    enum Player
    {
        FirstPlayer,
        SecondPlayer
    };
    Player curPlayer;
    Player otherPlayer;
    bool moving;
    bool changePlayer;
    int score[2];
    StaticLabel* stepLabels[2];
    StaticLabel* winLabels[2];
    StaticLabel* scoreLabels[2];
    Timer timer;
    bool fault;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
