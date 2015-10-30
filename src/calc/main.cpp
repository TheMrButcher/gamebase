#include <gamebase/engine/SimpleApplication.h>
#include <gamebase/engine/LinearLayout.h>
#include <gamebase/engine/Button.h>
#include <gamebase/engine/TextBox.h>
#include <gamebase/serial/JsonDeserializer.h>
#include <boost/lexical_cast.hpp>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
{
public:
    void load()
    {
        auto mainLayout = deserialize<LinearLayout>("calc\\MainLayout.json");

        auto textBoxesLayout = deserialize<LinearLayout>("calc\\VerticalLayout.json");
        argBox1 = deserialize<TextBox>("calc\\TextBox.json");
        textBoxesLayout->addObject(argBox1);

        argBox2 = deserialize<TextBox>("calc\\TextBox.json");
        textBoxesLayout->addObject(argBox2);

        resultBox = deserialize<TextBox>("calc\\TextBox.json");
        textBoxesLayout->addObject(resultBox);
        
        auto buttonsLayout = deserialize<LinearLayout>("calc\\VerticalLayout.json");

        auto sumButton = deserialize<Button>("calc\\SumButton.json");
        sumButton->setCallback(bind(&MyApp::sum, this));
        buttonsLayout->addObject(sumButton);

        auto diffButton = deserialize<Button>("calc\\DiffButton.json");
        diffButton->setCallback(bind(&MyApp::diff, this));
        buttonsLayout->addObject(diffButton);

        auto mulButton = deserialize<Button>("calc\\MulButton.json");
        mulButton->setCallback(bind(&MyApp::mul, this));
        buttonsLayout->addObject(mulButton);

        auto divButton = deserialize<Button>("calc\\DivButton.json");
        divButton->setCallback(bind(&MyApp::div, this));
        buttonsLayout->addObject(divButton);

        mainLayout->addObject(buttonsLayout);
        mainLayout->addObject(textBoxesLayout);

        m_view->addObject(mainLayout);
    }

    double readArg1()
    {
        return boost::lexical_cast<double>(argBox1->text());
    }

    double readArg2()
    {
        return boost::lexical_cast<double>(argBox2->text());
    }

    void writeResult(double value)
    {
        resultBox->setText(boost::lexical_cast<string>(value));
    }

    void sum()
    {
        writeResult(readArg1() + readArg2());
    }

    void diff()
    {
        writeResult(readArg1() - readArg2());
    }

    void mul()
    {
        writeResult(readArg1() * readArg2());
    }

    void div()
    {
        writeResult(readArg1() / readArg2());
    }

    shared_ptr<TextBox> argBox1;
    shared_ptr<TextBox> argBox2;
    shared_ptr<TextBox> resultBox;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
