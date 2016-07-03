#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

struct TowerData
{
    int level;
    int minDamage;
    int maxDamage;
    float range;
    Timer timer;
};

struct EnemyData
{
    int hp;
    Timer timer;
};

struct ArrowData
{
    int damage;
    int targetID;
};

class MyApp : public App
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
        Tree2,
        Tower
    };
    
    MyApp()
    {
        setDesign("towers\\Design.json");
    }

    void load()
    {
        randomize();

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

        gv.setGameBox(BoundingBox(gmap.width * 128, gmap.height * 100));

        w = gmap.width * 128;
        h = gmap.height * 100;
        auto gdx = -w / 2 + 128;
        auto gdy = -h / 2 + 100;
        dx = -w / 2 + 64;
        dy = -h / 2 + 50;
        for (int x = 0; x < gmap.width; ++x)
        {
            for (int y = 0; y < gmap.height; ++y)
            {
                DrawObj obj;
                switch (omap.get(x, y))
                {
                case Rock:  obj = loadObj<DrawObj>("towers\\Rock.json");  break;
                case Bush1: obj = loadObj<DrawObj>("towers\\Bush1.json"); break;
                case Bush2: obj = loadObj<DrawObj>("towers\\Bush2.json"); break;
                case Tree1: obj = loadObj<DrawObj>("towers\\Tree1.json"); break;
                case Tree2: obj = loadObj<DrawObj>("towers\\Tree2.json"); break;
                default: continue;
                }
                obj.setPos(x * 128 + dx, y * 100 + dy);
                objs.add(obj);
            }
        }

        for (int x = 0; x < gmap.width; x += 2)
        {
            for (int y = 0; y < gmap.height; y += 2)
            {
                if (gmap.get(x, y) == Grass)
                {
                    auto obj = loadObj<Texture>("towers\\Grass.json");
                    obj.setPos(x * 128 + gdx, y * 100 + gdy);
                    ground.add(obj);
                }
                if (gmap.get(x, y) == Path)
                {
                    auto obj = loadObj<Texture>("towers\\Path.json");
                    obj.setPos(x * 128 + gdx, y * 100 + gdy);
                    ground.add(obj);
                }
                if (gmap.get(x, y) == Water)
                {
                    auto obj = loadObj<Texture>("towers\\Water.json");
                    obj.setPos(x * 128 + gdx, y * 100 + gdy);
                    ground.add(obj);
                }
                if (gmap.get(x, y) == Sand)
                {
                    auto obj = loadObj<Texture>("towers\\Sand.json");
                    obj.setPos(x * 128 + gdx, y * 100 + gdy);
                    ground.add(obj);
                }
                if (gmap.get(x, y) == Start)
                {
                    auto obj = loadObj<GameObj>("towers\\Start.json");
                    obj.anim.run("anim");
                    obj.setPos(x * 128 + gdx, y * 100 + gdy);
                    start = IntVec2(x, y);
                    gmap.set(x, y, Path);
                    ground.add(obj);
                }
                if (gmap.get(x, y) == Finish)
                {
                    auto obj = loadObj<GameObj>("towers\\Finish.json");
                    obj.anim.run("anim");
                    obj.setPos(x * 128 + gdx, y * 100 + gdy);
                    finish = IntVec2(x, y);
                    gmap.set(x, y, Path);
                    ground.add(obj);
                }
            }
        }

        findPath(IntVec2(-1, -1), start);
        restart();

        connect0(build, enterBuildMode);
        connect0(upgrade, upgradeTower);
    }

    void enterBuildMode()
    {
        buildMode = true;
    }

    void restart()
    {
        gameover = false;
        score = 0;
        money = 10;
        hp = 20;
        waveLevel = 0;
        curTower = GameObj();
        upgrade.hide();
        props.hide();
        
        startWave();

        buildMode = false;
        greenRect.hide();
        redRect.hide();
    }

    void startWave()
    {
        isWave = true;
        leftToSpawn = 20;
        ++waveLevel;
        waveLevelLabel.setText(toString(waveLevel));
    }
    
    void processInput()
    {
        if (gameover)
            return;

        auto cpos = gv.view();
        if (input.leftPressed())
            cpos.x -= 1600 * timeDelta();
        if (input.rightPressed())
            cpos.x += 1600 * timeDelta();
        if (input.downPressed())
            cpos.y -= 1600 * timeDelta();
        if (input.upPressed())
            cpos.y += 1600 * timeDelta();
        gv.setView(cpos);

        if (buildMode && gv.isMouseOn())
        {
            auto mpos = gv.mousePos();
            int x = (mpos.x + w/2) / 128;
            int y = (mpos.y + h/2) / 100;
            if (gmap.get(x, y) == Grass && omap.get(x, y) == None && money >= 5)
            {
                greenRect.show();
                greenRect.setPos(x * 128 + dx, y * 100 + dy);
                redRect.hide();

                if (input.leftButtonJustOutpressed())
                {
                    omap.set(x, y, Tower);
                    auto tower = loadObj<GameObj>("towers\\Tower.json");
                    tower.setPos(x * 128 + dx, y * 100 + dy);
                    towers.add(tower);
                    connect1(tower, selectTower, tower);
                    money -= 5;
                    moneyLabel.setText(toString(money));

                    auto& data = towers.data(tower);
                    data.level = 1;
                    data.minDamage = 1;
                    data.maxDamage = 2;
                    data.range = 2;
                }
            }
            else
            {
                redRect.show();
                redRect.setPos(x * 128 + dx, y * 100 + dy);
                greenRect.hide();
            }

            if (input.rightButtonJustOutpressed())
            {
                buildMode = false;
                greenRect.hide();
                redRect.hide();
            }
        }

        if (!buildMode && gv.isMouseOn() && input.rightButtonJustOutpressed())
        {
            curTower = GameObj();
            upgrade.hide();
            props.hide();
            range.hide();
        }
    }

    void move()
    {
        if (gameover)
            return;

        if (isWave)
        {
            if (timer.isPeriod(400))
            {
                auto enemy = loadObj<GameObj>("towers\\Enemy.json");
                enemy.setPos(
                    start.x * 128 - w / 2 + 64 + randomFloat() * 128,
                    start.y * 100 - h / 2 + 50 + randomFloat() * 100);
                for (int i = 0; i < path.size(); ++i)
                    enemy.anim.run(path[i]);
                enemy.anim.run("move", 1);
                enemy.anim.run("start", 2);
                enemies.add(enemy);
                enemies.data(enemy).hp = 3 * waveLevel;
                if (--leftToSpawn <= 0)
                    isWave = false;
            }
        }
        else
        {
            if (timer.isPeriod(12000))
                startWave();
        }

        feach(auto tower, towers.all())
        {
            auto& tdata = towers.data(tower);
            if (tdata.timer.isPeriod(1000))
            {
                Box rbox(256 * tdata.range, 200 * tdata.range);
                rbox.move(tower.pos());
                feach(auto enemy, enemies.find(rbox))
                {
                    auto delta = enemy.pos() - tower.pos();
                    delta.y *= 256.0 / 200;
                    if (delta.length() > 128 * tdata.range || enemy.anim.isRunning(3))
                        continue;
                    auto arrow = loadObj<GameObj>("towers\\Arrow.json");
                    arrow.setPos(tower.pos());
                    arrows.add(arrow);

                    auto& adata = arrows.data(arrow);
                    adata.damage = randomInt(tdata.minDamage, tdata.maxDamage);
                    adata.targetID = enemy.id();
                    break;
                }
            }
        }

        feach(auto arrow, arrows.all())
        {
            auto& adata = arrows.data(arrow);
            if (!enemies.has(adata.targetID))
            {
                arrows.remove(arrow);
                continue;
            }

            auto enemy = enemies.get(adata.targetID);
            auto apos = arrow.pos();
            auto delta = enemy.pos() - apos;
            if (delta.length() < 16)
            {
                auto& edata = enemies.data(enemy);
                edata.hp -= adata.damage;
                enemy.anim.pause(0);
                edata.timer.start();
                if (edata.hp <= 0 && !enemy.anim.isRunning(3))
                {
                    enemy.anim.run("exp", 3);
                    money++;
                    moneyLabel.setText(toString(money));
                }
                arrows.remove(arrow);
                continue;
            }

            arrow.setAngle(delta.angle());
            delta.normalize();
            delta *= timeDelta() * 500;
            apos += delta;
            arrow.setPos(apos);
        }

        feach(auto enemy, enemies.all())
        {
            if (enemy.anim.isEmpty(0))
            {
                hp--;
                hpLabel.setText(toString(hp));
                if (hp == 0)
                {
                    gameover = true;
                    gameoverLabel.show();
                    return;
                }
                enemies.remove(enemy);
            }
            else if (enemy.anim.isPaused(0)) {
                if (enemies.data(enemy).timer.isPeriod(1000))
                    enemy.anim.resume(0);
            }
        }
    }

    void selectTower(GameObj tower)
    {
        if (buildMode)
            return;

        range.show();
        range.setPos(tower.pos());
        props.show();
        upgrade.show();

        auto& data = towers.data(tower);
        range.setScale(data.range);
        levelLabel.setText(toString(data.level));
        damageLabel.setText(toString(data.minDamage) + "-" + toString(data.maxDamage));
        rangeLabel.setText(toString(data.range + 0.0001, 3));

        curTower = tower;
    }

    void upgradeTower()
    {
        auto& data = towers.data(curTower);
        if (money < data.level * 2 + 1)
            return;
        
        money -= data.level * 2 + 1;
        moneyLabel.setText(toString(money));

        data.level++;
        data.minDamage = data.level;
        data.maxDamage = data.level * 2;
        data.range += 0.2;
        selectTower(curTower);
    }

    bool findPath(IntVec2 prev, IntVec2 cur)
    {
        cout << "Cur: " << cur << endl;
        if (cur == finish)
            return true;

        IntVec2 steps[] = {
            IntVec2(0, 2),
            IntVec2(2, 0),
            IntVec2(0, -2),
            IntVec2(-2, 0)
        };

        string anims[] = {
            "up", "right", "down", "left"
        };

        for (int i = 0; i < 4; ++i)
        {
            auto next = cur + steps[i];
            if (prev != next && gmap.get(next) == Path)
            {
                path.push_back(anims[i]);
                if (findPath(cur, next))
                    return true;
                path.pop_back();
            }
        }
        return false;
    }

    GameMap<GroundType> gmap;
    GameMap<ObjType> omap;
    FromDesign(GameView, gv);
    FromDesign(SimpleLayer, objs);
    FromDesign(SimpleLayer, ground);
    LayerFromDesign(TowerData, towers);
    LayerFromDesign(EnemyData, enemies);
    LayerFromDesign(ArrowData, arrows);
    FromDesign(FilledRect, greenRect);
    FromDesign(FilledRect, redRect);
    FromDesign(GameObj, range);

    FromDesign(Button, build);
    FromDesign(Button, upgrade);
    FromDesign(Canvas, props);
    
    FromDesign2(Label, gameoverLabel, "gameover");
    FromDesign2(Label, moneyLabel, "money");
    FromDesign(Label, levelLabel);
    FromDesign(Label, damageLabel);
    FromDesign(Label, rangeLabel);
    FromDesign2(Label, waveLevelLabel, "level");
    FromDesign2(Label, hpLabel, "hp");

    GameObj curTower;
    int score;
    bool gameover;

    bool buildMode;

    float dx;
    float dy;
    float w;
    float h;

    IntVec2 start;
    IntVec2 finish;
    vector<string> path;

    Timer timer;
    bool isWave;
    int waveLevel;
    int leftToSpawn;

    int money;
    int hp;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
