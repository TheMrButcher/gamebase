#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    enum GroundType {
        Grass,
        Path,
        Water,
        Sand,
        Start,
        Finish
    };

    enum ObjType {
        None,
        Rock,
        Bush1,
        Bush2,
        Tree1,
        Tree2
    };

    void load()
    {
        randomize();

        design = deserialize<LinearLayout>("towers\\Design.json");
        m_view->addObject(design);

        std::map<Color, GroundType> colorToGType;
        colorToGType[Color(0, 1, 0)] = Grass;
        colorToGType[Color(0, 0, 0)] = Path;
        colorToGType[Color(0, 0, 1)] = Water;
        colorToGType[Color(1, 1, 0)] = Sand;
        colorToGType[Color(1, 0, 1)] = Start;
        colorToGType[Color(1, 0, 0)] = Finish;

        std::map<Color, ObjType> colorToOType;
        colorToOType[Color(0, 0, 0)] = None;
        colorToOType[Color(1, 1, 1)] = Rock;
        colorToOType[Color(1, 0, 1)] = Bush1;
        colorToOType[Color(1, 0, 0)] = Bush2;
        colorToOType[Color(0, 1, 0)] = Tree1;
        colorToOType[Color(0, 0, 1)] = Tree2;

        gmap = loadMap<GroundType>("towers\\gmap.png", colorToGType);
        omap = loadMap<ObjType>("towers\\omap.png", colorToOType);

        gv = design->getChild<GameView>("#gv");
        gv->setGameBox(BoundingBox(gmap.width * 200, gmap.height * 120));
        auto* ground = gv->getLayer<StaticLayer>(0);
        auto* objects = gv->getLayer<StaticLayer>(1);

        auto dx = -gmap.width * 100 + 100;
        auto dy = -gmap.height * 60 + 60;
        for (int x = 0; x < gmap.width; ++x)
        {
            for (int y = 0; y < gmap.height; ++y)
            {
                if (gmap.get(x, y) == Grass)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Grass.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Path)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Path.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Water)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Water.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Sand)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Sand.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Start)
                {
                    auto obj = deserialize<AnimatedObjectConstruct>("towers\\Start.json");
                    obj->runAnimation("anim");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    ground->addObject(obj);
                }
                if (gmap.get(x, y) == Finish)
                {
                    auto obj = deserialize<AnimatedObjectConstruct>("towers\\Finish.json");
                    obj->runAnimation("anim");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    ground->addObject(obj);
                }

                if (omap.get(x, y) == Rock)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Rock.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Bush1)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Bush1.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Bush2)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Bush2.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Tree1)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Tree1.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    objects->addObject(obj);
                }
                if (omap.get(x, y) == Tree2)
                {
                    auto obj = deserialize<StaticTextureRect>("towers\\Tree2.json");
                    obj->setOffset(Vec2(x * 200 + dx, y * 120 + dy));
                    objects->addObject(obj);
                }
            }
        }

        restart();
    }

    void restart()
    {
        gameover = false;
        score = 0;
    }

    void move()
    {
        if (gameover)
            return;

        auto cpos = gv->viewCenter();
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Left))
            cpos.x -= 1600 * timeDelta();
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Right))
            cpos.x += 1600 * timeDelta();
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Down))
            cpos.y -= 1600 * timeDelta();
        if (m_inputRegister.specialKeys.isPressed(SpecialKey::Up))
            cpos.y += 1600 * timeDelta();
        gv->setViewCenter(cpos);
    }

    shared_ptr<LinearLayout> design;
    GameMap<GroundType> gmap;
    GameMap<ObjType> omap;
    GameView* gv;

    int score;
    bool gameover;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
