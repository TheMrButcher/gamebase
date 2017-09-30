#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
    void load()
    {
        randomize();
        restart();
        connect(restartButton, restart);
		connect(numberBox, enter);
        connect(enterButton, enter);
    }

    void restart()
    {
        gameover = false;
        number = randomInt(0, 99);
		numberBox << "";
        hideLabels();
    }

    void hideLabels()
    {
        lessLabel.hide();
        winLabel.hide();
        moreLabel.hide();
    }

    void enter()
    {
        if (gameover)
            return;
		int numFromUser;
		numberBox >> numFromUser;
        hideLabels();
        if (number < numFromUser)
            lessLabel.show();
        if (number > numFromUser)
            moreLabel.show();
        if (number == numFromUser)
        {
            winLabel.show();
            gameover = true;
        }
    }

    FromDesign(TextBox, numberBox);
    FromDesign(Button, enterButton);
    FromDesign(Button, restartButton);
    FromDesign(Label, lessLabel);
    FromDesign(Label, winLabel);
    FromDesign(Label, moreLabel);

    bool gameover;
    int number;
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setDesign("guess\\Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
