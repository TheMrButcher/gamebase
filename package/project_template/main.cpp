#include <gamebase/engine/BasicTools.h>

using namespace gamebase;
using namespace std;

class MyApp : public SimpleApplication
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
