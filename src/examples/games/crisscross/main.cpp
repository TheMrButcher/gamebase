#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
public:
    enum Player
    {
        None,
        First,
        Second
    };

    MyApp()
    {
        setDesign("crisscross\\Design.json");
    }

    void load()
    {
        restart();
        connect(design.child<Button>("restart"), restart);
        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                string indexStr = toString(x) + toString(y);
                connect(design.child<Button>("button" + indexStr), step, x, y);
            }
        }
    }

    void step(int x, int y)
    {
        if (gameOver)
            return;
        string indexStr = toString(x) + toString(y);
        field[x][y] = curPlayer;
        if (curPlayer == First)
        {
            design.child<Texture>("cross" + indexStr).show();
            curPlayer = Second;
            step1.hide();
            step2.show();
        }
        else
        {
            design.child<Texture>("circle" + indexStr).show();
            curPlayer = First;
            step1.show();
            step2.hide();
        }
        design.child<Button>("button" + indexStr).disable();

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
                win1.show();
            else
                win2.show();
        }

        ++stepNum;
        if (!gameOver && stepNum == 9)
        {
            gameOver = true;
            draw.show();
        }

        if (gameOver)
        {
            step1.hide();
            step2.hide();
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
                string indexStr = toString(x) + toString(y);
                design.child<Button>("button" + indexStr).enable();
                design.child<Texture>("cross" + indexStr).hide();
                design.child<Texture>("circle" + indexStr).hide();
                field[x][y] = None;
            }
        }
        step1.show();
        step2.hide();
        win1.hide();
        win2.hide();
        draw.hide();
    }

    FromDesign(Label, step1);
    FromDesign(Label, step2);
    FromDesign(Label, win1);
    FromDesign(Label, win2);
    FromDesign(Label, draw);

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
