#include <gamebase/engine/Application.h>

using namespace gamebase;
using namespace std;

class MyApp : public Application
{
};

int main(int argc, char** argv)
{
    MyApp app;
    if (!app.init(&argc, argv))
        return 1;
    app.run();
    return 0;
}
