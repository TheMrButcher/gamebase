#include <gamebase/engine/SimpleApplication.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/CanvasLayout.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/StaticTextureRect.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <boost/lexical_cast.hpp>
#include <Windows.h>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        srand(GetTickCount());

        design = deserialize<LinearLayout>("jumpball\\Design.json");
        m_view->addObject(design);
        
        ball = design->getChild<StaticTextureRect>("#ball");
        columnsLayout = design->getChild<CanvasLayout>("#columns");

        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));

        score = 0;
        record = 0;
    }

    void postload()
    {
        gameBox = columnsLayout->box();
        restart();
    }

    void restart()
    {
        gameover = false;
        ball->setOffset(Vec2(-450, 0));
        columnsLayout->setOffset(Vec2(0, 0));
        velocity = 0;
        score = 0;
        design->getChild<StaticLabel>("#score")->setText("0");

        columns.clear();
        columnsLayout->clear();
        
        for (int i = 0; i < 50; ++i)
        {
            auto column = deserialize<StaticTextureRect>("jumpball\\Column.json");
            auto height = rand() % (int)(gameBox.height() - 292) + 100;
            column->setFixedBox(64, height);
            Vec2 pos(i * 250, 0.5f * (height - gameBox.height()));
            column->setOffset(pos);
            columnsLayout->addObject(column);

            columns.push_back(column.get());
        }
    }

    void move()
    {
        if (gameover)
            return;

        auto columnsPos = columnsLayout->getOffset();
        auto ballPos = ball->getOffset();

        BoundingBox ballBox(64, 64);
        ballBox.move(ballPos);

        int newScore = 0;
        for (int i = 0; i < columns.size(); ++i)
        {
            auto columnBox = columns[i]->box().transformed(
                columns[i]->position());
            columnBox.move(columnsPos);
            if (ballBox.intersects(columnBox))
                gameover = true;
            if (columnBox.topRight.x < ballBox.bottomLeft.x)
                newScore = i + 1;
        }

        if (newScore == 50)
            gameover = true;

        if (ballBox.bottomLeft.y < gameBox.bottomLeft.y || ballBox.topRight.y > gameBox.topRight.y)
            gameover = true;

        if (score != newScore)
        {
            score = newScore;
            design->getChild<StaticLabel>("#score")->setText(
                boost::lexical_cast<string>(score));
        }

        if (gameover)
        {
            if (record < score)
            {
                record = score;
                design->getChild<StaticLabel>("#record")->setText(
                    boost::lexical_cast<string>(record));
            }
            return;
        }

        auto time = timeDelta();

        columnsPos.x -= 200 * time;
        columnsLayout->setOffset(columnsPos);

        ballPos.y += velocity * time;
        ball->setOffset(ballPos);
        
        velocity -= 150 * time;
        if (m_inputRegister.specialKeys.isJustPressed(SpecialKey::Up))
            velocity += 100;
    }

    shared_ptr<LinearLayout> design;

    StaticTextureRect* ball;
    CanvasLayout* columnsLayout;
    vector<StaticTextureRect*> columns;

    BoundingBox gameBox;

    float velocity;
    int score;
    int record;

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
