#include <gamebase/engine/SimpleApplication.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/StaticLabel.h>
#include <gamebase/engine/StaticTextureRect.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <boost/lexical_cast.hpp>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    enum Player
    {
        None,
        First,
        Second
    };

    void load()
    {
        design = deserialize<LinearLayout>("crisscross\\Design.json");
        m_view->addObject(design);

        restart();

        design->getChild<Button>("#restart")->setCallback(bind(&MyApp::restart, this));
        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                string indexStr = boost::lexical_cast<string>(x) + boost::lexical_cast<string>(y);
                design->getChild<Button>("#button" + indexStr)->setCallback(bind(&MyApp::step, this, x, y));
            }
        }
    }

    void step(int x, int y)
    {
        if (gameOver)
            return;
        string indexStr = boost::lexical_cast<string>(x) + boost::lexical_cast<string>(y);
        field[x][y] = curPlayer;
        if (curPlayer == First)
        {
            design->getChild<StaticTextureRect>("#cross" + indexStr)->setVisible(true);
            curPlayer = Second;
            design->getChild<StaticLabel>("#step1")->setVisible(false);
            design->getChild<StaticLabel>("#step2")->setVisible(true);
        }
        else
        {
            design->getChild<StaticTextureRect>("#circle" + indexStr)->setVisible(true);
            curPlayer = First;
            design->getChild<StaticLabel>("#step1")->setVisible(true);
            design->getChild<StaticLabel>("#step2")->setVisible(false);
        }
        design->getChild<Button>("#button" + indexStr)->disable();

        Player winner = None;
        if (field[1][1] != None
            && ((field[0][0] == field[1][1] && field[1][1] == field[2][2])
                || (field[2][0] == field[1][1] && field[1][1] == field[0][2])))
            winner = field[1][1];
        for (int i = 0; i < 3; ++i)
        {
            if (field[i][0] != None && field[i][0] == field[i][1] && field[i][1] == field[i][2])
                winner = field[i][0];
            if (field[0][i] != None && field[0][i] == field[1][i] && field[1][i] == field[2][i])
                winner = field[0][i];
        }
        if (winner != None)
        {
            gameOver = true;
            if (winner == First)
                design->getChild<StaticLabel>("#win1")->setVisible(true);
            else
                design->getChild<StaticLabel>("#win2")->setVisible(true);
        }

        ++stepNum;
        if (!gameOver && stepNum == 9)
        {
            gameOver = true;
            design->getChild<StaticLabel>("#draw")->setVisible(true);
        }

        if (gameOver)
        {
            design->getChild<StaticLabel>("#step1")->setVisible(false);
            design->getChild<StaticLabel>("#step2")->setVisible(false);
        }
    }

    void restart()
    {
        stepNum = 0;
        gameOver = false;
        curPlayer = First;
        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                string indexStr = boost::lexical_cast<string>(x) + boost::lexical_cast<string>(y);
                design->getChild<Button>("#button" + indexStr)->enable();
                design->getChild<StaticTextureRect>("#cross" + indexStr)->setVisible(false);
                design->getChild<StaticTextureRect>("#circle" + indexStr)->setVisible(false);
                field[x][y] = None;
            }
        }
        design->getChild<StaticLabel>("#step2")->setVisible(false);
        design->getChild<StaticLabel>("#win1")->setVisible(false);
        design->getChild<StaticLabel>("#win2")->setVisible(false);
        design->getChild<StaticLabel>("#draw")->setVisible(false);
    }

    shared_ptr<LinearLayout> design;
    Player field[3][3];
    Player curPlayer;
    bool gameOver;
    int stepNum;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
