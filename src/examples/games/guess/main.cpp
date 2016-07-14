#include <gamebase/engine/SimpleApplication.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/TextBox.h>
#include <gamebase/engine/StaticLabel.h>
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

        auto mainLayout = deserialize<LinearLayout>("guess\\MainLayout.json");

        auto restartButton = deserialize<Button>("guess\\RestartButton.json");
        restartButton->setCallback(bind(&MyApp::restart, this));
        mainLayout->addObject(restartButton);

        textBox = deserialize<TextBox>("guess\\TextBox.json");
        mainLayout->addObject(textBox);

        auto enterButton = deserialize<Button>("guess\\EnterButton.json");
        enterButton->setCallback(bind(&MyApp::guess, this));
        mainLayout->addObject(enterButton);
        
        auto labelsLayout = deserialize<LinearLayout>("guess\\HorizontalLayout.json");

        less = deserialize<StaticLabel>("guess\\Less.json");
        labelsLayout->addObject(less);

        win = deserialize<StaticLabel>("guess\\Win.json");
        labelsLayout->addObject(win);

        more = deserialize<StaticLabel>("guess\\More.json");
        labelsLayout->addObject(more);
        
        mainLayout->addObject(labelsLayout);

        m_view->addObject(mainLayout);

        restart();
    }

    void hideLabels()
    {
        less->setVisible(false);
        win->setVisible(false);
        more->setVisible(false);
    }

    void restart()
    {
        number = rand() % 100;
        textBox->setText("");
        hideLabels();
    }

    void guess()
    {
        int inputNumber = boost::lexical_cast<int>(textBox->text());
        
        hideLabels();

        if (number < inputNumber)
            less->setVisible(true);
        else if (number > inputNumber)
            more->setVisible(true);
        else
            win->setVisible(true);
    }

    int number;
    shared_ptr<TextBox> textBox;
    shared_ptr<StaticLabel> less;
    shared_ptr<StaticLabel> win;
    shared_ptr<StaticLabel> more;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
