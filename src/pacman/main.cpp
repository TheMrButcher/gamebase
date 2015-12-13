#include <gamebase/engine/BasicTools.h>
#include <gamebase/engine/ObjectConstructTools.h>
#include <gamebase/engine/GameMap.h>
#include <gamebase/math/IntVector.h>


using namespace gamebase;
using namespace std;

int V = 64;

class MyApp : public SimpleApplication
{
public:
    enum Type
    {
        None,
        Wall,
        Coin,
        Energizer,
        Pacman
    };

    void load()
    {
        design = deserialize<CanvasLayout>("pacman\\Design.json");

        map<Color, Type> colorToType;
        colorToType[Color(0, 0, 0)] = None;
        colorToType[Color(1, 1, 1)] = Wall;
        colorToType[Color(1, 1, 0)] = Coin;
        colorToType[Color(0, 1, 0)] = Energizer;
        colorToType[Color(0, 0, 1)] = Pacman;
        gameMap = loadMap("pacman\\map.png", colorToType);
        
        w = gameMap.width * 32;
        h = gameMap.height * 32;
        auto canvas = design->getChild<CanvasLayout>("#canvas");
        canvas->setFixedBox(w, h);
        for (int x = 0; x < gameMap.width; ++x)
        {
            for (int y = 0; y < gameMap.height; ++y)
            {
                shared_ptr<ObjectConstruct> obj;
                if (gameMap.map[x][y] == Wall)
                {
                    obj = deserialize<ObjectConstruct>("pacman\\Wall.json");
                }
                if (gameMap.map[x][y] == Coin)
                {
                    obj = deserialize<ObjectConstruct>("pacman\\Coin.json");
                    coins.push_back(obj.get());
                }
                if (gameMap.map[x][y] == Energizer)
                {
                    obj = deserialize<ObjectConstruct>("pacman\\Energizer.json");
                    energizers.push_back(obj.get());
                }
                if (gameMap.map[x][y] == Pacman)
                {
                    obj = deserialize<ObjectConstruct>("pacman\\Pacman.json");
                    pacman = obj.get();
                    startPos = IntVec2(x, y);
                }

                if (obj)
                {
                    obj->setOffset(Vec2(x * 32 - w / 2, y * 32 - h / 2));
                    canvas->addObject(obj);
                }
            }
        }

        m_view->addObject(design);
        
        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));

        restart();
    }

    void restart()
    {
        design->getChild<StaticLabel>("#gameover")->setVisible(false);
        design->getChild<StaticLabel>("#score")->setText("0");
        score = 0;

        for (int i = 0; i < coins.size(); ++i)
            coins[i]->setVisible(true);
        for (int i = 0; i < energizers.size(); ++i)
            energizers[i]->setVisible(true);
        pacman->setOffset(Vec2(startPos.x * 32 - w / 2, startPos.y * 32 - h / 2));
        pacman->resetAllChannels();
        pacman->runAnimation("right", 0);
        velocity = Vec2(V, 0);
        pos = startPos;
        pacman->runAnimation("move", 1);
        offset = Vec2(0, 0);
    }

    void move()
    {
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

        for (int i = 0; i < coins.size(); ++i)
        {
            if (coins[i]->isVisible() && pacman->movedBox().intersects(coins[i]->movedBox()))
            {
                coins[i]->setVisible(false);
                score += 100;
                design->getChild<StaticLabel>("#score")->setText(toString(score));
            }
        }

        for (int i = 0; i < energizers.size(); ++i)
        {
            if (energizers[i]->isVisible() && pacman->movedBox().intersects(energizers[i]->movedBox()))
            {
                energizers[i]->setVisible(false);
                score += 1000;
                design->getChild<StaticLabel>("#score")->setText(toString(score));
            }
        }
    }

    shared_ptr<CanvasLayout> design;
    int score;
    float w;
    float h;
    ObjectConstruct* pacman;
    vector<ObjectConstruct*> coins;
    vector<ObjectConstruct*> energizers;
    GameMap<Type> gameMap;
    Vec2 velocity;
    IntVec2 startPos;
    IntVec2 pos;
    Vec2 offset;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
