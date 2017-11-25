#include <gamebase/Gamebase.h>

using namespace gamebase;
using namespace std;

class MyApp : public App
{
};

int main(int argc, char** argv)
{
    MyApp app;
    app.setConfig("config.json");
    app.setDesign("multisampling/Design.json");
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
