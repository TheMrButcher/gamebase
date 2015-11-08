#include <gamebase/engine/SimpleApplication.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/StaticTextureRect.h>
#include <gamebase/engine/Timer.h>
#include <gamebase/math/IntVector.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <boost/lexical_cast.hpp>
#include <Windows.h>

using namespace gamebase;
using namespace std;

class Figure
{
public:
    Figure() {}

    Figure(int startYArg, const Color& colorArg, const vector<IntVec2>& geomArg)
    {
        startY = startYArg;
        color = colorArg;
        geom = geomArg;
    }

    void rotateLeft()
    {
        for (auto it = geom.begin(); it != geom.end(); ++it)
            *it = rotate90CCW(*it);
    }

    void rotateRight()
    {
        for (auto it = geom.begin(); it != geom.end(); ++it)
            *it = rotate90CW(*it);
    }

    int startY;
    Color color;
    vector<IntVec2> geom;
};

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        srand(GetTickCount());

        design = deserialize<CanvasLayout>("tetris\\Design.json");
        m_view->addObject(design);

        fieldLayout = design->getChild<CanvasLayout>("#field");

        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));

        {
            vector<IntVec2> geom;
            geom.push_back(IntVec2(0, 1));
            geom.push_back(IntVec2(0, 0));
            geom.push_back(IntVec2(0, -1));
            geom.push_back(IntVec2(0, -2));
            figures.push_back(Figure(18, Color(1, 0, 0), geom));
        }

        {
            vector<IntVec2> geom;
            geom.push_back(IntVec2(1, 1));
            geom.push_back(IntVec2(0, 1));
            geom.push_back(IntVec2(0, 0));
            geom.push_back(IntVec2(0, -1));
            figures.push_back(Figure(18, Color(0, 1, 0), geom));
        }

        {
            vector<IntVec2> geom;
            geom.push_back(IntVec2(-1, 1));
            geom.push_back(IntVec2(0, 1));
            geom.push_back(IntVec2(0, 0));
            geom.push_back(IntVec2(0, -1));
            figures.push_back(Figure(18, Color(0, 0, 1), geom));
        }

        {
            vector<IntVec2> geom;
            geom.push_back(IntVec2(0, 1));
            geom.push_back(IntVec2(-1, 0));
            geom.push_back(IntVec2(0, 0));
            geom.push_back(IntVec2(1, 0));
            figures.push_back(Figure(18, Color(1, 0, 1), geom));
        }

        {
            vector<IntVec2> geom;
            geom.push_back(IntVec2(1, 1));
            geom.push_back(IntVec2(0, 1));
            geom.push_back(IntVec2(1, 0));
            geom.push_back(IntVec2(0, 0));
            figures.push_back(Figure(18, Color(1, 1, 0), geom));
        }

        {
            vector<IntVec2> geom;
            geom.push_back(IntVec2(1, 0));
            geom.push_back(IntVec2(0, 0));
            geom.push_back(IntVec2(0, -1));
            geom.push_back(IntVec2(-1, -1));
            figures.push_back(Figure(18, Color(0, 1, 1), geom));
        }

        {
            vector<IntVec2> geom;
            geom.push_back(IntVec2(-1, 0));
            geom.push_back(IntVec2(0, 0));
            geom.push_back(IntVec2(0, -1));
            geom.push_back(IntVec2(1, -1));
            figures.push_back(Figure(18, Color(1, 0.5, 0), geom));
        }

        record = 0;
        restart();
    }

    void restart()
    {
        gameover = false;
        design->getChild<StaticLabel>("#gameover")->setVisible(false);
        score = 0;
        design->getChild<StaticLabel>("#score")->setTextAndLoad("0");
        timer.start();
        fieldLayout->clear();

        for (int x = 0; x < 10; ++x)
            for (int y = 0; y < 20; ++y)
                field[x][y] = -1;

        newFigure();
    }

    void move()
    {
        if (gameover)
            return;

        if (timer.isPeriod(period))
        {
            if (!tryUpdatePos(0, -1))
            {
                for (int i = 0; i < curFigure.geom.size(); ++i)
                {
                    IntVec2 fullPos = curPos + curFigure.geom[i];
                    field[fullPos.x][fullPos.y] = textureIds[i];
                }
                
                vector<int> rawsToBurn;
                for (int y = 0; y < 20; ++y)
                {
                    bool hasHole = false;
                    for (int x = 0; x < 10; ++x)
                    {
                        if (field[x][y] == -1)
                            hasHole = true;
                    }
                    if (!hasHole)
                        rawsToBurn.push_back(y);
                }

                for (auto it = rawsToBurn.rbegin(); it != rawsToBurn.rend(); ++it)
                {
                    int rawY = *it;
                    for (int x = 0; x < 10; ++x)
                        fieldLayout->removeObject(field[x][rawY]);
                    for (int y = rawY; y < 19; ++y)
                    {
                        for (int x = 0; x < 10; ++x)
                        {
                            field[x][y] = field[x][y + 1];
                            if (field[x][y] != -1)
                                adjustTile(field[x][y], IntVec2(x, y));
                        }
                    }
                    for (int x = 0; x < 10; ++x)
                        field[x][19] = -1;
                }

                switch (rawsToBurn.size())
                {
                case 1: score += 100; break;
                case 2: score += 300; break;
                case 3: score += 700; break;
                case 4: score += 1500; break;
                }

                if (!rawsToBurn.empty())
                {
                    design->getChild<StaticLabel>("#score")->setTextAndLoad(
                        boost::lexical_cast<string>(score));
                }

                newFigure();
            }
        }

        if (m_inputRegister.keys.isJustPressed(13) || m_inputRegister.keys.isJustPressed('s'))
        {
            if (period != 50)
            {
                timer.start();
                period = 50;
            }
        }

        if (m_inputRegister.keys.isJustPressed('a'))
            tryUpdatePos(-1, 0);
        if (m_inputRegister.keys.isJustPressed('d'))
            tryUpdatePos(1, 0);

        if (m_inputRegister.keys.isJustPressed('q'))
        {
            Figure newFigure = curFigure;
            newFigure.rotateLeft();
            if (canPlace(curPos, newFigure))
            {
                curFigure = newFigure;
                adjustCurTiles();
            }
        }

        if (m_inputRegister.keys.isJustPressed('e'))
        {
            Figure newFigure = curFigure;
            newFigure.rotateRight();
            if (canPlace(curPos, newFigure))
            {
                curFigure = newFigure;
                adjustCurTiles();
            }
        }
    }

    bool tryUpdatePos(int xOffset, int yOffset)
    {
        auto newPos = curPos;
        newPos.x += xOffset;
        newPos.y += yOffset;
        if (canPlace(newPos, curFigure))
        {
            curPos = newPos;
            adjustCurTiles();
            return true;
        }
        return false;
    }

    void newFigure()
    {
        period = 500;
        curFigure = figures[rand() % figures.size()];
        curPos.y = curFigure.startY;
        if (canPlace(IntVec2(5, curPos.y), curFigure))
        {
            curPos.x = 5;
        }
        else
        {
            gameover = true;
            design->getChild<StaticLabel>("#gameover")->setVisible(true);
            if (score > record)
            {
                record = score;
                design->getChild<StaticLabel>("#record")->setTextAndLoad(
                    boost::lexical_cast<string>(record));
            }
            return;
        }

        textureIds.clear();
        for (int i = 0; i < curFigure.geom.size(); ++i)
        {
            auto tile = deserialize<StaticTextureRect>("tetris\\Tile.json");
            tile->setColor(curFigure.color);
            textureIds.push_back(fieldLayout->addObject(tile));
        }
        adjustCurTiles();
    }

    bool canPlace(const IntVec2& pos, const Figure& figure)
    {
        for (auto it = figure.geom.begin(); it != figure.geom.end(); ++it)
        {
            IntVec2 fullPos = pos + *it;
            if (fullPos.x < 0 || fullPos.x > 9)
                return false;
            if (fullPos.y < 0)
                return false;
            if (field[fullPos.x][fullPos.y] != -1)
                return false;
        }
        return true;
    }

    void adjustCurTiles()
    {
        for (int i = 0; i < textureIds.size(); ++i)
        {
            IntVec2 fullPos = curPos + curFigure.geom[i];
            adjustTile(textureIds[i], fullPos);
        }
    }

    void adjustTile(int id, const IntVec2& pos)
    {
        Vec2 vec(pos.x * 32 - 144, pos.y * 32 - 304);
        auto* tile = fieldLayout->getObject<StaticTextureRect>(id);
        auto* offset = tile->offset<FixedOffset>();
        offset->set(vec);
    }

    shared_ptr<CanvasLayout> design;
    CanvasLayout* fieldLayout;

    Figure curFigure;
    vector<int> textureIds;
    IntVec2 curPos;

    int score;
    int record;

    bool gameover;

    int field[10][20];
    Timer timer;
    int period;

    vector<Figure> figures;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
