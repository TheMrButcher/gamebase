#include <gamebase/engine/Application.h>
#include <gamebase/text/FontStorage.h>
#include <gamebase/text/TextGeometry.h>
#include <gamebase/graphics/TextureProgram.h>
#include <fstream>
#include <locale>
#include <iostream>
#include <unordered_map>

using namespace gamebase;

class MyApplication : public Application {
public:
    virtual void load() override
    {
        float y = -300;
        const std::string text("Привет, мир!");
        for (unsigned int fontSize = 16; fontSize <= 40; fontSize += 2, y += 40) {
            m_textGeomBuffers.push_back(createLabel(
                text, Vec2(-500, y), "Times New Roman", static_cast<float>(fontSize)));
            m_textGeomBuffers.push_back(createLabel(
                text, Vec2(-250, y), "Arial", static_cast<float>(fontSize)));
        }

        std::string bigText("«Космические рейнджеры» — компьютерная игра в научно-фантастическом стиле, разработанная российской студией Elemental Games. Издана компанией 1С в 2002 году. На Западе игра была выпущена под названием Space rangers. Игра принадлежит к жанру «эпических игр» с элементами RPG, пошаговой стратегии, текстового квеста и аркады. Игра повествует о противоборстве между Галактическим содружеством, состоящим из пяти рас, включая людей, и вторгшимися силами клисан — неизвестной, агрессивно настроенной формой жизни. Игроку предстоит выступить в роли рейнджера-добровольца, участника организации, созданной специально для борьбы с клисанами.");
        AlignProperties alignProps;
        alignProps.font.size = 32;
        alignProps.horAlign = HorAlign::Center;
        alignProps.vertAlign = VertAlign::Center;
        m_textGeomBuffers.push_back(std::make_pair(alignProps.font.get(),
            createTextGeometryBuffers(bigText, alignProps, BoundingBox(Vec2(0, -350), Vec2(500, 350)))));
    }

    virtual void render() override
    {
        const auto& program = textureProgram();
        program.transform = projectionTransform();
        program.color = Color(1, 1, 0);
        for (auto it = m_textGeomBuffers.begin(); it != m_textGeomBuffers.end(); ++it) {
            program.texture = it->first->texture();
            program.draw(it->second.vbo, it->second.ibo);
        }
    }

    virtual void processKeyDown(unsigned char key) override
    {
        if (key == 27)
            stop();
    }

private:
    std::pair<std::shared_ptr<IFont>, GLBuffers> createLabel(
        const std::string& text, const Vec2& pos, const std::string& fontFamily, float fontSize)
    {
        auto font = fontStorage().getFont(fontFamily, fontSize);
        auto textGeom = createTextGeometry(placeText(text, pos), font.get());
        return std::make_pair(font, createTextGeometryBuffers(textGeom));
    }

    std::vector<std::pair<std::shared_ptr<IFont>, GLBuffers>> m_textGeomBuffers;
};

MyApplication app;

int main(int argc, char** argv)
{
    if (!app.init(&argc, argv, Application::Window, 1024, 768))
        return 1;
    app.run();
    return 0;
}
