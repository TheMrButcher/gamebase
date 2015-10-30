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
    enum OpType
    {
        None,
        Sum,
        Diff,
        Mul,
        Div
    };

    void load()
    {
        op = None;

        auto mainLayout = deserialize<LinearLayout>("calc2\\MainLayout.json");

        textBox = deserialize<TextBox>("calc\\TextBox.json");
        mainLayout->addObject(textBox);
        
        auto buttonsLayout = deserialize<LinearLayout>("calc2\\HorizontalLayout.json");

        auto plusButton = deserialize<Button>("calc2\\PlusButton.json");
        plusButton->setCallback(bind(&MyApp::calculate, this, Sum));
        buttonsLayout->addObject(plusButton);

        auto minusButton = deserialize<Button>("calc2\\MinusButton.json");
        minusButton->setCallback(bind(&MyApp::calculate, this, Diff));
        buttonsLayout->addObject(minusButton);

        auto mulButton = deserialize<Button>("calc2\\MulButton.json");
        mulButton->setCallback(bind(&MyApp::calculate, this, Mul));
        buttonsLayout->addObject(mulButton);

        auto divButton = deserialize<Button>("calc2\\DivButton.json");
        divButton->setCallback(bind(&MyApp::calculate, this, Div));
        buttonsLayout->addObject(divButton);

        auto calcButton = deserialize<Button>("calc2\\CalcButton.json");
        calcButton->setCallback(bind(&MyApp::calculate, this, None));
        buttonsLayout->addObject(calcButton);

        mainLayout->addObject(buttonsLayout);

        m_view->addObject(mainLayout);
    }

    void calculate(OpType nextOp)
    {
        if (!prevValue.empty())
        {
            double prev = boost::lexical_cast<double>(prevValue);
            double cur = boost::lexical_cast<double>(textBox->text());
            double result;
            switch (op)
            {
            case None: result = cur; break;
            case Sum: result = prev + cur; break;
            case Diff: result = prev - cur; break;
            case Mul: result = prev * cur; break;
            case Div: result = prev / cur; break;
            }
            textBox->setText(boost::lexical_cast<string>(result));
        }

        op = nextOp;
        prevValue = textBox->text();
    }

    shared_ptr<TextBox> textBox;
    string prevValue;
    OpType op;
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
