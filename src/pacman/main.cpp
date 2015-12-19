#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/ObjectConstructTools.h>
#include <gamebase/engine/GameMap.h>
#include <gamebase/engine/Timer.h>
#include <gamebase/math/IntVector.h>


using namespace gamebase;
using namespace std;

int V = 128;
int GV = 100;

Color ghostColors[] = {
    Color(1, 0, 0),
    Color(0, 1, 1),
    Color(1, 0.5, 1),
    Color(1, 0.5, 0)
};

class MyApp : public SimpleApplication
{
public:
    enum Type
    {
        None,
        Wall,
        Coin,
        Energizer,
        Pacman,
        Ghost
    };

    void load()
    {
        randomize();

        design = deserialize<CanvasLayout>("pacman\\Design.json");

        map<Color, Type> colorToType;
        colorToType[Color(0, 0, 0)] = None;
        colorToType[Color(1, 1, 1)] = Wall;
        colorToType[Color(1, 1, 0)] = Coin;
        colorToType[Color(0, 1, 0)] = Energizer;
        colorToType[Color(0, 0, 1)] = Pacman;
        colorToType[Color(1, 0, 0)] = Ghost;
        gameMap = loadMap("pacman\\map.png", colorToType);
        
        w = gameMap.width * 32;
        h = gameMap.height * 32;
        auto canvas = design->getChild<CanvasLayout>("#canvas");
        canvas->setFixedBox(w, h);
        totalCoinsNum = 0;
        for (int x = 0; x < gameMap.width; ++x)
        {
            for (int y = 0; y < gameMap.height; ++y)
            {
                shared_ptr<ObjectConstruct> obj;
                switch (gameMap.get(x, y))
                {
                case Wall:
                    obj = deserialize<ObjectConstruct>("pacman\\Wall.json");
                    walls.push_back(obj.get());
                    break;

                case Coin:
                    obj = deserialize<ObjectConstruct>("pacman\\Coin.json");
                    ++totalCoinsNum;
                    coins.push_back(obj.get());
                    break;
                
                case Energizer:
                    obj = deserialize<ObjectConstruct>("pacman\\Energizer.json");
                    energizers.push_back(obj.get());
                    break;

                case Pacman:
                    obj = deserialize<ObjectConstruct>("pacman\\Pacman.json");
                    pacman = obj.get();
                    startPos = IntVec2(x, y);
                    break;

                case Ghost:
                    {
                        obj = deserialize<ObjectConstruct>("pacman\\Ghost.json");
                        obj->getChild<StaticTextureRect>("#ghost")->setColor(ghostColors[ghosts.size() % 4]);
                        GhostState state;
                        state.app = this;
                        state.obj = obj.get();
                        state.startPos = IntVec2(x, y);
                        ghosts.push_back(state);
                        break;
                    }
                }

                if (obj)
                {
                    obj->setOffset(Vec2(x * 32 - w / 2, y * 32 - h / 2));
                    canvas->addObject(obj);
                }
            }
        }

        pathMap = createMap<int>(gameMap.width, gameMap.height);

        m_view->addObject(design);
        
        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));

        restart();
    }

    void restart()
    {
        hp = 2;
        gameover = false;
        design->getChild<StaticLabel>("#gameover")->setVisible(false);
        score = 0;
        design->getChild<StaticLabel>("#score")->setText("0");
        newGame();
    }

    void newGame()
    {
        for (int i = 0; i < coins.size(); ++i)
            coins[i]->setVisible(true);
        for (int i = 0; i < energizers.size(); ++i)
            energizers[i]->setVisible(true);
        moveToStart();
        eatenCoinsNum = 0;
    }

    void moveToStart()
    {
        for (int i = 0; i < ghosts.size(); ++i)
        {
            ghosts[i].restart();
            ghosts[i].obj->setOffset(Vec2(ghosts[i].startPos.x * 32 - w / 2, ghosts[i].startPos.y * 32 - h / 2));
        }
        pacman->setOffset(Vec2(startPos.x * 32 - w / 2, startPos.y * 32 - h / 2));
        pacman->resetAllChannels();
        pacman->runAnimation("right", 0);
        velocity = Vec2(V, 0);
        pos = startPos;
        pacman->runAnimation("move", 1);
        offset = Vec2(0, 0);
        mode = Normal;
    }

    void move()
    {
        if (mode == Pause)
        {
            if (timer.time() > 2500)
            {
                mode = Normal;
                newGame();
            }
            else
            {
                return;
            }
        }
        if (gameover)
            return;
        if (offset.isZero())
        {
            if (m_inputRegister.specialKeys.isPressed(SpecialKey::Right))
            {
                if (gameMap.get(pos.x + 1, pos.y) != Wall)
                {
                    if (velocity.isZero())
                        pacman->runAnimation("move", 1);
                    velocity = Vec2(V, 0);
                    pacman->runAnimation("right", 0);
                }
            }
            if (m_inputRegister.specialKeys.isPressed(SpecialKey::Up))
            {
                if (gameMap.get(pos.x, pos.y + 1) != Wall)
                {
                    if (velocity.isZero())
                        pacman->runAnimation("move", 1);
                    velocity = Vec2(0, V);
                    pacman->runAnimation("up", 0);
                }
            }
            if (m_inputRegister.specialKeys.isPressed(SpecialKey::Left))
            {
                if (gameMap.get(pos.x - 1, pos.y) != Wall)
                {
                    if (velocity.isZero())
                        pacman->runAnimation("move", 1);
                    velocity = Vec2(-V, 0);
                    pacman->runAnimation("left", 0);
                }
            }
            if (m_inputRegister.specialKeys.isPressed(SpecialKey::Down))
            {
                if (gameMap.get(pos.x, pos.y - 1) != Wall)
                {
                    if (velocity.isZero())
                        pacman->runAnimation("move", 1);
                    velocity = Vec2(0, -V);
                    pacman->runAnimation("down", 0);
                }
            }
        }

        offset += velocity * timeDelta();
        if (offset.x < -32)
        {
            pos.x--;
            offset.x = 0;
            if (gameMap.get(pos.x - 1, pos.y) == Wall)
            {
                pacman->resetChannel(1);
                velocity = Vec2(0, 0);
            }
        }
        if (offset.x > 32)
        {
            pos.x++;
            offset.x = 0;
            if (gameMap.get(pos.x + 1, pos.y) == Wall)
            {
                pacman->resetChannel(1);
                velocity = Vec2(0, 0);
            }
        }
        if (offset.y < -32)
        {
            pos.y--;
            offset.y = 0;
            if (gameMap.get(pos.x, pos.y - 1) == Wall)
            {
                pacman->resetChannel(1);
                velocity = Vec2(0, 0);
            }
        }
        if (offset.y > 32)
        {
            pos.y++;
            offset.y = 0;
            if (gameMap.get(pos.x, pos.y + 1) == Wall)
            {
                pacman->resetChannel(1);
                velocity = Vec2(0, 0);
            }
        }
        pacman->setOffset(Vec2(pos.x * 32 - w / 2 + offset.x, pos.y * 32 - h / 2 + offset.y));

        for (int i = 0; i < ghosts.size(); ++i)
            ghosts[i].move();

        for (int i = 0; i < coins.size(); ++i)
        {
            if (coins[i]->isVisible() && pacman->movedBox().intersects(coins[i]->movedBox()))
            {
                coins[i]->setVisible(false);
                score += 100;
                eatenCoinsNum++;
                design->getChild<StaticLabel>("#score")->setText(toString(score));
                if (eatenCoinsNum == totalCoinsNum)
                {
                    mode = Pause;
                    for (int i = 0; i < walls.size(); ++i)
                        walls[i]->runAnimation("finish");
                    timer.start();
                    return;
                }
            }
        }

        for (int i = 0; i < energizers.size(); ++i)
        {
            if (energizers[i]->isVisible() && pacman->movedBox().intersects(energizers[i]->movedBox()))
            {
                energizers[i]->setVisible(false);
                score += 100;
                timer.start();
                mode = Afraid;
                for (int i = 0; i < ghosts.size(); ++i)
                    if (ghosts[i].isAlive)
                    {
                        ghosts[i].obj->resetChannel(1);
                        ghosts[i].obj->runAnimation("afraid", 1);
                    }
                design->getChild<StaticLabel>("#score")->setText(toString(score));
            }
        }

        if (mode == Afraid && timer.time() > 5000)
        {
            mode = Recover;
            for (int i = 0; i < ghosts.size(); ++i)
                if (ghosts[i].isAlive)
                    ghosts[i].obj->runAnimation("recover", 1);
            timer.start();
        }

        if (mode == Recover && timer.time() > 1500)
        {
            mode = Normal;
            for (int i = 0; i < ghosts.size(); ++i)
                if (ghosts[i].isAlive)
                    ghosts[i].obj->runAnimation("normal", 1);
        }

        for (int i = 0; i < ghosts.size(); ++i)
        {
            auto& ghost = ghosts[i];
            if (ghost.isAlive && ghost.obj->movedBox().intersects(pacman->movedBox()))
            {
                if (mode == Normal)
                {
                    --hp;
                    if (hp < 0)
                    {
                        design->getChild<StaticLabel>("#gameover")->setVisible(true);
                        pacman->resetAllChannels();
                        gameover = true;
                        return;
                    }
                    moveToStart();
                }
                else
                {
                    ghost.isAlive = false;
                    ghost.obj->runAnimation("kill", 1);
                    score += 1000;
                    design->getChild<StaticLabel>("#score")->setText(toString(score));
                }
            }
        }
    }

    Vec2 whereToGo(const IntVec2& from, const IntVec2& to)
    {
        static const IntVec2 steps[] = {
            IntVec2(1, 0),
            IntVec2(0, 1),
            IntVec2(-1, 0),
            IntVec2(0, -1)
        };

        for (int x = 0; x < gameMap.width; ++x)
            for (int y = 0; y < gameMap.height; ++y)
                pathMap[x][y] = 1000000;
        pathMap[from.x][from.y] = 0;
        float curMinDist = dist(from, to);
        IntVec2 curTarget = from;

        vector<IntVec2> curWave;
        curWave.push_back(from);

        vector<IntVec2> nextWave;
        while (!curWave.empty())
        {
            while (!curWave.empty())
            {
                auto curPos = curWave.back();
                curWave.pop_back();
                
                int stepsNum = pathMap[curPos.x][curPos.y] + 1;
                for (int i = 0; i < 4; ++i)
                {
                    auto newPos = curPos + steps[i];
                    if (gameMap.get(newPos.x, newPos.y) == Wall)
                        continue;
                    if (pathMap[newPos.x][newPos.y] <= stepsNum)
                        continue;
                    pathMap[newPos.x][newPos.y] = stepsNum;
                    nextWave.push_back(newPos);

                    float newDist = dist(newPos, to);
                    if (newDist < curMinDist)
                    {
                        curTarget = newPos;
                        curMinDist = newDist;
                    }
                }
            }

            curWave.swap(nextWave);
        }

        if (curTarget == from)
            return Vec2(0, 0);

        IntVec2 prevPos;
        IntVec2 curPos = curTarget;
        while (curPos != from)
        {
            IntVec2 nextPos;
            for (int i = 0; i < 4; ++i)
            {
                nextPos = curPos + steps[i];
                if (pathMap[nextPos.x][nextPos.y] + 1 == pathMap[curPos.x][curPos.y])
                    break;
            }
            prevPos = curPos;
            curPos = nextPos;
        }
        auto delta = prevPos - curPos;
        return Vec2(delta.x * GV, delta.y * GV);
    }

    shared_ptr<CanvasLayout> design;
    int score;
    float w;
    float h;
    ObjectConstruct* pacman;
    vector<ObjectConstruct*> coins;
    vector<ObjectConstruct*> energizers;
    vector<ObjectConstruct*> walls;

    struct GhostState
    {
        MyApp* app;
        ObjectConstruct* obj;
        IntVec2 startPos;
        bool isAlive;
        IntVec2 pos;
        Vec2 offset;
        Vec2 velocity;

        void restart()
        {
            obj->resetAllChannels();
            obj->runAnimation("normal", 1);
            obj->runAnimation("right", 0);
            velocity = Vec2(0, 0);
            pos = startPos;
            isAlive = true;
            offset = Vec2(0, 0);
        }

        void move()
        {
            if (offset.isZero())
            {
                if (!isAlive)
                {
                    if (pos == startPos)
                    {
                        isAlive = true;
                        obj->runAnimation("normal", 1);
                    }
                }
                vector<Vec2> variants;
                if (app->gameMap.get(pos.x + 1, pos.y) != Wall)
                    variants.push_back(Vec2(GV, 0));
                if (app->gameMap.get(pos.x - 1, pos.y) != Wall)
                    variants.push_back(Vec2(-GV, 0));
                if (app->gameMap.get(pos.x, pos.y + 1) != Wall)
                    variants.push_back(Vec2(0, GV));
                if (app->gameMap.get(pos.x, pos.y - 1) != Wall)
                    variants.push_back(Vec2(0, -GV));

                if (variants.size() > 2 || velocity.isZero())
                {
                    if (isAlive)
                    {
                        if (app->mode == Normal)
                        {
                            velocity = app->whereToGo(pos, app->pos);
                        }
                        else
                        {
                            velocity = variants[rand() % variants.size()];
                        }
                    }
                    else
                    {
                        velocity = app->whereToGo(pos, startPos);
                    }

                    if (velocity.x > 0)
                        obj->runAnimation("right", 0);
                    if (velocity.x < 0)
                        obj->runAnimation("left", 0);
                    if (velocity.y > 0)
                        obj->runAnimation("up", 0);
                    if (velocity.y < 0)
                        obj->runAnimation("down", 0);
                }
            }

            offset += velocity * timeDelta();
            if (offset.x < -32)
            {
                pos.x--;
                offset.x = 0;
                if (app->gameMap.get(pos.x - 1, pos.y) == Wall)
                    velocity = Vec2(0, 0);
            }
            if (offset.x > 32)
            {
                pos.x++;
                offset.x = 0;
                if (app->gameMap.get(pos.x + 1, pos.y) == Wall)
                    velocity = Vec2(0, 0);
            }
            if (offset.y < -32)
            {
                pos.y--;
                offset.y = 0;
                if (app->gameMap.get(pos.x, pos.y - 1) == Wall)
                    velocity = Vec2(0, 0);
            }
            if (offset.y > 32)
            {
                pos.y++;
                offset.y = 0;
                if (app->gameMap.get(pos.x, pos.y + 1) == Wall)
                    velocity = Vec2(0, 0);
            }
            obj->setOffset(Vec2(pos.x * 32 - app->w / 2 + offset.x, pos.y * 32 - app->h / 2 + offset.y));
        }
    };
    vector<GhostState> ghosts;

    GameMap<Type> gameMap;
    vector<vector<int>> pathMap;
    Vec2 velocity;
    IntVec2 startPos;
    IntVec2 pos;
    Vec2 offset;

    enum Mode
    {
        Normal,
        Afraid,
        Recover,
        Pause
    };
    Mode mode;
    Timer timer;

    bool gameover;
    int hp;
    int eatenCoinsNum;
    int totalCoinsNum;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
