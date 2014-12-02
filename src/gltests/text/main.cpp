#include <gamebase/engine/Application.h>
#include <gamebase/engine/FilledRect.h>
#include <gamebase/graphics/Image.h>
#include <gamebase/graphics/TextureProgram.h>
#include <gamebase/utils/FileIO.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <sstream>
#include <fstream>
#include <locale>
#include <iostream>
#include <unordered_map>

using namespace gamebase;

#pragma pack(push, 1)
struct BFFHeader {
    unsigned char bffID[2];
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int cellWidth;
    unsigned int cellHeight;
    unsigned char bitsPerPixel;
    unsigned char firstSymbol;
};
#pragma pack(pop)

static_assert(sizeof(BFFHeader) == 20, "bad size of BFF header");

namespace {
const unsigned int MIN_FONT_HEIGHT = 10;
const unsigned int MAX_FONT_HEIGHT = 256;
const std::string FONTS_PATH = "..\\..\\resources\\fonts\\";
}

std::string extractFontName(const std::string& fname)
{
    size_t lastSpaceIndex = fname.find_last_of(" _");
    if (lastSpaceIndex == std::string::npos)
        THROW_EX() << "Can't extract font name. Font's file name has incorrect format: " << fname;
    size_t lastSlashIndex = fname.find_last_of("\\/");
    if (lastSlashIndex != std::string::npos && lastSlashIndex > lastSpaceIndex)
        THROW_EX() << "Can't extract font name. Unexpected slash after font's name: " << fname;
    size_t start = lastSlashIndex == std::string::npos ? 0 : lastSlashIndex + 1;
    return fname.substr(start, lastSpaceIndex - start);
}

unsigned int extractFontSize(const std::string& fname)
{
    static const std::locale LOCALE;
    std::string nameWithoutExtension = fname.substr(0, fname.find_last_of("."));
    size_t lastSpaceIndex = nameWithoutExtension.find_last_of(" _");
    if (lastSpaceIndex == std::string::npos
        || lastSpaceIndex == nameWithoutExtension.size() - 1
        || !std::isdigit(nameWithoutExtension[lastSpaceIndex + 1], LOCALE))
        THROW_EX() << "Font's file name has incorrect format: " << fname;
    unsigned int result = 0;
    std::stringstream ss(nameWithoutExtension.substr(lastSpaceIndex + 1));
    ss >> result;
    if (result < MIN_FONT_HEIGHT || result > MAX_FONT_HEIGHT)
        THROW_EX() << "Font " << nameWithoutExtension
            << " has incorrect size: " << result
            << ". Must be in [" << MIN_FONT_HEIGHT << "; " << MAX_FONT_HEIGHT << "]";
    return result;
}

class IFont {
public:
    virtual ~IFont() {}

    virtual const std::string& name() const = 0;

    virtual float fontSize() const = 0;

    virtual Vec2 cellSize() const = 0;

    virtual const Texture& texture() const = 0;

    virtual float getWidth(char ch) const = 0;

    virtual BoundingBox glyphTextureRect(char ch) const = 0;
};

class OriginFont : public IFont {
public:
    OriginFont(const std::string& fileName)
        : m_name(extractFontName(fileName))
        , m_fileName(fileName)
        , m_fontSize(extractFontSize(fileName))
    {}

    void load()
    {
        auto data = loadBinaryFile(m_fileName);
        auto& header = reinterpret_cast<BFFHeader&>(data.front());
        if (header.bffID[0] != 0xBF || header.bffID[1] != 0xF2)
            THROW_EX() << "Not a BFF-file, incorrect ID";
        size_t imageSize = header.imageWidth * header.imageHeight * header.bitsPerPixel / 8;
        if (data.size() != sizeof(BFFHeader) + sizeof(m_glyphWidths) + imageSize)
            THROW_EX() << "Incorrect size of BFF-file";
        if (header.bitsPerPixel != 32)
            THROW_EX() << "Loader supports only 32 bits per pixel";

        m_cellSize = Size(header.cellWidth, header.cellHeight).toVector();
        m_glyphTextureSize = Vec2(
            float(header.cellWidth) / header.imageWidth,
            float(header.cellHeight) / header.imageHeight);
        m_glyphsPerLine = header.imageWidth / header.cellWidth;
        m_minIndex = header.firstSymbol;
        m_maxIndex = static_cast<unsigned char>(std::min(
            m_minIndex + (header.imageHeight / header.cellHeight) * m_glyphsPerLine, 255u));

        memcpy(m_glyphWidths, (&data.front()) + sizeof(BFFHeader), sizeof(m_glyphWidths));
        std::vector<uint8_t> imageData(imageSize);
        memcpy(&imageData.front(), (&data.front()) + sizeof(BFFHeader) + sizeof(m_glyphWidths), imageSize);
        Image image(std::move(imageData), Size(header.imageWidth, header.imageHeight));
        m_glyphAtlas = Texture(image);
    }

    virtual const std::string& name() const override { return m_name; }

    virtual float fontSize() const override { return static_cast<float>(m_fontSize); }

    virtual Vec2 cellSize() const override { return m_cellSize; }

    virtual const Texture& texture() const override { return m_glyphAtlas; }

    virtual float getWidth(char ch) const override
    {
        return static_cast<float>(m_glyphWidths[static_cast<unsigned char>(ch)]);
    }

    virtual BoundingBox glyphTextureRect(char ch) const override
    {
        auto index = static_cast<unsigned char>(ch);
        if (index < m_minIndex || index > m_maxIndex)
            return BoundingBox();
        index -= m_minIndex;
        unsigned char lineIndex = index / m_glyphsPerLine;
        unsigned char columnIndex = index - lineIndex * m_glyphsPerLine;
        Vec2 leftBottom(m_glyphTextureSize.x * columnIndex, m_glyphTextureSize.y * lineIndex);
        return BoundingBox(leftBottom, leftBottom + m_glyphTextureSize);
    }

private:
    std::string m_name;
    std::string m_fileName;
    unsigned int m_fontSize;
    unsigned char m_minIndex;
    unsigned char m_maxIndex;
    unsigned char m_glyphsPerLine;
    Vec2 m_cellSize;
    Vec2 m_glyphTextureSize;
    unsigned char m_glyphWidths[256];
    Texture m_glyphAtlas;
};

class Font : public IFont {
public:
    Font(const IFont* originFont, float fontSize)
        : m_originFont(originFont)
        , m_fontSize(fontSize)
        , m_scale(fontSize / originFont->fontSize())
    {}

    virtual const std::string& name() const override { return m_originFont->name(); }

    virtual float fontSize() const override { return m_fontSize; }

    virtual Vec2 cellSize() const override { return m_originFont->cellSize() * m_scale; }

    virtual const Texture& texture() const override { return m_originFont->texture(); }

    virtual float getWidth(char ch) const override
    {
        return m_scale * m_originFont->getWidth(ch);
    }

    virtual BoundingBox glyphTextureRect(char ch) const override
    {
        return m_originFont->glyphTextureRect(ch);
    }

private:
    const IFont* m_originFont; 
    float m_fontSize;
    float m_scale;
};

bool compareFonts(const std::shared_ptr<IFont>& font1, const std::shared_ptr<IFont>& font2)
{
    return font1->fontSize() < font2->fontSize();
}

unsigned int round(float f)
{
    return static_cast<unsigned int>(f + 0.5f);
}

bool compareSizes(const std::shared_ptr<IFont>& font, float fontSize)
{
    return round(font->fontSize()) < round(fontSize);
}

namespace {
const float EPSILON = 0.00001f;
}

bool isEqual(float a, float b, float eps = EPSILON)
{
    return std::abs(a - b) < eps;
}

struct FileDesc {
    enum Type {
        None,
        Directory,
        File,
        Other
    };

    FileDesc(): type(None) {}
    FileDesc(
        const std::string& path,
        const std::string& fileName,
        const std::string& extension,
        Type type)
        : path(path)
        , fileName(fileName)
        , extension(extension)
        , type(type)
    {}

    std::string fullFileName() const
    {
        return extension.empty() ? fileName : fileName + '.' + extension;
    }

    std::string path;
    std::string fileName;
    std::string extension;
    Type type;
};

namespace fs = boost::filesystem;

bool fileExists(const fs::path& path)
{
    return fs::exists(path);
}

bool fileExists(const std::string& pathStr)
{
    return fileExists(boost::filesystem::path(pathStr));
}

FileDesc fileInfo(const fs::path& path)
{
    try {
        if (!fileExists(path))
            return FileDesc();
        std::string fileName = path.filename();
        std::string extension;
        FileDesc::Type type;
        if (fs::is_directory(path)) {
            type = FileDesc::Directory;
        } else {
            type = fs::is_regular_file(path) ? FileDesc::File : FileDesc::Other;
            extension = path.extension();
            if (!extension.empty() && extension[0] == '.')
                extension = extension.substr(1);
            fileName = fileName.substr(0, fileName.size() - extension.size() - 1);
        }
        return FileDesc(path.string(), fileName, extension, type);
    } catch (std::exception& ex) {
        THROW_EX() << "Can't get info about file: " << path.string() << ". Reason: " << ex.what();
    }
}

FileDesc fileInfo(const std::string& pathStr)
{
    return fileInfo(fs::path(pathStr));
}

std::vector<FileDesc> listFilesInDirectory(const std::string& pathStr)
{
    try {
        if (!fileExists(pathStr))
            THROW_EX() << pathStr << " doesn't exist";
        fs::path path(pathStr);
        if (!fs::is_directory(path))
            THROW_EX() << pathStr << " isn't directory";

        std::vector<FileDesc> result;
        fs::directory_iterator endIt;
        for (fs::directory_iterator dirIt(path); dirIt != endIt; ++dirIt)
            result.push_back(fileInfo(dirIt->path()));
        return result;
    } catch (std::exception& ex) {
        THROW_EX() << "Can't list files in directory: " << pathStr << ". Reason: " << ex.what();
    }
}

class FontStorage {
public:
    FontStorage() {}

    void load()
    {
        auto fontDirFilesList = listFilesInDirectory(FONTS_PATH);
        std::vector<std::string> fontFileNames;
        for (auto it = fontDirFilesList.begin(); it != fontDirFilesList.end(); ++it) {
            if (it->type == FileDesc::File && it->extension == "bff")
                fontFileNames.push_back(it->path);
        }

        for (auto it = fontFileNames.begin(); it != fontFileNames.end(); ++it)
            loadFont(*it);

        for (auto it = m_fontFamilies.begin(); it != m_fontFamilies.end(); ++it) {
            auto& fonts = it->second;
            std::sort(fonts.begin(), fonts.end(), compareFonts);
        }

        if (m_fontFamilies.empty()) {
            std::cerr << "Warning! No fonts are loaded" << std::endl;
            return;
        }

        auto it = m_fontFamilies.find("Times New Roman");
        m_defaultFamilyName = it == m_fontFamilies.end()
            ? m_fontFamilies.begin()->first
            : "Times New Roman";
    }

    std::shared_ptr<IFont> getFont(float fontSize) const
    {
        return getFont(m_defaultFamilyName, fontSize);
    }

    std::shared_ptr<IFont> getFont(const std::string& familyName, float fontSize) const
    {
        if (m_fontFamilies.empty())
            THROW_EX() << "Can't get any font. FontStorage is empty";
        if (fontSize <= 0)
            THROW_EX() << "Wrong size of font: " << fontSize;
        auto it = m_fontFamilies.find(familyName);
        auto& fonts = it == m_fontFamilies.end()
            ? m_fontFamilies.at(m_defaultFamilyName)
            : it->second;
        if (it == m_fontFamilies.end())
            std::cout << "Using default font family: " << m_defaultFamilyName << std::endl;
        else
            std::cout << "Found font family: " << familyName << std::endl;
        return findFont(fonts, fontSize);
    }

private:
    void loadFont(const std::string& fname)
    {
        try {
            auto font = std::make_shared<OriginFont>(fname);
            font->load();
            std::cout << "Loaded font: \"" << font->name() << "\", height: " << font->fontSize() << std::endl;
            m_fontFamilies[font->name()].push_back(font);
        } catch (Exception& ex) {
            std::cerr << "Can't load font from file: " << fname << ". Reason: " << ex.what() << std::endl;
            return;
        }
    }

    std::shared_ptr<IFont> findFont(const std::vector<std::shared_ptr<IFont>>& fonts, float fontSize) const
    {
        auto fontIt = std::lower_bound(fonts.begin(), fonts.end(), fontSize, compareSizes);
        const IFont* originFont = nullptr;
        if (fontIt == fonts.end()) {
            originFont = fonts.back().get();
            std::cout << "Getting fonts.back(). Origin font size: " << originFont->fontSize() << std::endl;
        } else {
            if (isEqual((*fontIt)->fontSize(), fontSize)) {
                std::cout << "Getting origin font. Origin font size: " << (*fontIt)->fontSize() << std::endl;
                return *fontIt;
            }
            if (fontIt == fonts.begin()) {
                originFont = fonts.front().get();
                std::cout << "Getting fonts.front(). Origin font size: " << originFont->fontSize() << std::endl;
            } else {
                auto fontItPrev = fontIt - 1;
                float distToPrev = fontSize - (*fontItPrev)->fontSize();
                float distToNext = (*fontIt)->fontSize() - fontSize;
                originFont = distToPrev < distToNext ? fontItPrev->get() : fontIt->get();
                if (originFont == fontItPrev->get())
                    std::cout << "Getting prev. Origin font size: " << originFont->fontSize() << std::endl;
                else
                    std::cout << "Getting next. Origin font size: " << originFont->fontSize() << std::endl;
            }
        }
            
        return std::make_shared<Font>(originFont, fontSize);
    }

    std::unordered_map<std::string, std::vector<std::shared_ptr<IFont>>> m_fontFamilies;
    std::string m_defaultFamilyName;
};

float getTextLength(const std::string& str, const IFont* font)
{
    float result = 0;
    for (auto it = str.begin(); it != str.end(); ++it)
        result += font->getWidth(*it);
    return result;
}

std::vector<std::string> splitTextToWords(const std::string& text)
{
    std::vector<std::string> result;
    size_t curStart = 0;
    size_t nextDelim;
    while ((nextDelim = text.find(' ', curStart)) != std::string::npos) {
        if (curStart < nextDelim)
            result.push_back(text.substr(curStart, nextDelim - curStart));
        curStart = nextDelim + 1;
    }

    if (curStart != text.length())
        result.push_back(text.substr(curStart));
    return result;
}

struct AlignedString {
    AlignedString(const BoundingBox& bbox, std::string&& text)
        : bbox(bbox), text(std::move(text))
    {}

    AlignedString(const BoundingBox& bbox, const std::string& text)
        : bbox(bbox), text(text)
    {}

    const Vec2& offset() const { return bbox.bottomLeft; }

    BoundingBox bbox;
    std::string text;
};

std::vector<AlignedString> placeText(const std::string& text, const Vec2& pos)
{
    std::vector<AlignedString> alignedText;
    alignedText.push_back(AlignedString(BoundingBox(pos), text));
    return alignedText;
}

struct HorAlign {
    enum Enum {
        Left,
        Center,
        Right
    };
};

struct VertAlign {
    enum Enum {
        Top,
        Center,
        Bottom
    };
};

struct AlignProperties {
    AlignProperties(const IFont* font)
        : font(font)
        , horAlign(HorAlign::Left)
        , vertAlign(VertAlign::Top)
    {}

    const IFont* font;
    HorAlign::Enum horAlign;
    VertAlign::Enum vertAlign;
};

struct Line {
    Line() { clear(); }
    Line(Line&& other)
        : text(std::move(other.text))
        , length(other.length)
    {}

    Line& operator=(Line&& other)
    {
        text = std::move(other.text);
        length = other.length;
        return *this;
    }

    void clear()
    {
        text.clear();
        length = 0;
    }

    std::string text;
    float length;
};

std::vector<Line> splitTextToLines(
    const std::string& text,
    const IFont* font,
    float maxLineLength)
{
    auto words = splitTextToWords(text);
    float spaceWidth = font->getWidth(' ');

    std::vector<Line> lines;
    Line curLine;
    for (auto it = words.begin(); it != words.end(); ++it) {
        const auto& word = *it;
        float wordLength = getTextLength(word, font);
        if (!curLine.text.empty() && curLine.length + spaceWidth + wordLength > maxLineLength) {
            lines.push_back(std::move(curLine));
            curLine.clear();
        }

        if (!curLine.text.empty()) {
            curLine.text += ' ';
            curLine.length += spaceWidth;
        }
        curLine.text += word;
        curLine.length += wordLength;
    }
    if (!curLine.text.empty())
        lines.push_back(std::move(curLine));

    return lines;
}

std::vector<AlignedString> alignText(
    const std::string& text, const AlignProperties& alignProps, const BoundingBox& box)
{
    if (!alignProps.font)
        THROW_EX() << "Can't align text, no font is provided";
    
    auto lines = splitTextToLines(text, alignProps.font, box.width());
    float maxLineLength = 0.0f;
    for (auto it = lines.begin(); it != lines.end(); ++it)
        maxLineLength = std::max(maxLineLength, it->length);
    float fontSize = alignProps.font->fontSize();
    float totalHeight = lines.size() * fontSize;

    float startY = box.bottomLeft.y;
    switch (alignProps.vertAlign) {
        case VertAlign::Top: startY += box.height(); break;
        case VertAlign::Center: startY += 0.5f * (box.height() + totalHeight); break;
        case VertAlign::Bottom: startY += totalHeight; break;
        default: THROW_EX() << "Bad VertAlign::Enum value: " << static_cast<int>(alignProps.vertAlign);
    }

    std::vector<AlignedString> result;
    result.reserve(lines.size());
    startY -= fontSize;
    for (auto it = lines.begin(); it != lines.end(); ++it, startY -= fontSize) {
        float lineLength = it->length;
        float startX = box.bottomLeft.x;
        switch (alignProps.horAlign) {
            case HorAlign::Left: break;
            case HorAlign::Center: startX += 0.5f * (maxLineLength - lineLength); break;
            case HorAlign::Right: startX += maxLineLength - lineLength; break;
            default: THROW_EX() << "Bad HorAlign::Enum value: " << static_cast<int>(alignProps.horAlign);
        }
        Vec2 start(startX, startY);
        BoundingBox lineBox(start, start + Vec2(lineLength, fontSize)); 
        result.emplace_back(AlignedString(lineBox, std::move(it->text)));
    }
    return result;
}

struct CharPosition {
    CharPosition(const BoundingBox& position, const BoundingBox& glyphTextureRect)
        : position(position), glyphTextureRect(glyphTextureRect)
    {}

    BoundingBox position;
    BoundingBox glyphTextureRect;
};

std::vector<CharPosition> createTextGeometry(
    const std::vector<AlignedString>& alignedText,
    const IFont* font)
{
    Vec2 cellSize = font->cellSize();
    std::vector<CharPosition> result;
    for (auto it = alignedText.begin(); it != alignedText.end(); ++it) {
        Vec2 offset = it->offset();
        const std::string& line = it->text;
        for (auto charIt = line.begin(); charIt != line.end(); ++charIt) {
            char ch = *charIt;
            BoundingBox position(offset, offset + cellSize);
            result.push_back(CharPosition(
                position, font->glyphTextureRect(ch)));
            offset.x += font->getWidth(ch);
        }
    }
    return result;
}

GLBuffers createTextGeometryBuffers(const std::vector<CharPosition>& textGeom)
{
    std::vector<float> vertices;
    vertices.reserve(textGeom.size() * 16);
    std::vector<short> indices;
    indices.reserve(textGeom.size() * 6);
    short offset = 0;
    for (auto it = textGeom.begin(); it != textGeom.end(); ++it) {
        const auto& pos = it->position;
        const auto& texBox = it->glyphTextureRect;

        vertices.push_back(pos.bottomLeft.x); vertices.push_back(pos.bottomLeft.y);
        vertices.push_back(texBox.bottomLeft.x); vertices.push_back(texBox.topRight.y);
        
        vertices.push_back(pos.bottomLeft.x); vertices.push_back(pos.topRight.y);
        vertices.push_back(texBox.bottomLeft.x); vertices.push_back(texBox.bottomLeft.y);
        
        vertices.push_back(pos.topRight.x); vertices.push_back(pos.bottomLeft.y);
        vertices.push_back(texBox.topRight.x); vertices.push_back(texBox.topRight.y);
        
        vertices.push_back(pos.topRight.x); vertices.push_back(pos.topRight.y);
        vertices.push_back(texBox.topRight.x); vertices.push_back(texBox.bottomLeft.y);

        indices.push_back(offset + 0); indices.push_back(offset + 1); indices.push_back(offset + 2);
        indices.push_back(offset + 1); indices.push_back(offset + 2); indices.push_back(offset + 3);

        offset += 4;
    }
    return GLBuffers(VertexBuffer(vertices), IndexBuffer(indices));
}

class MyApplication : public Application {
public:
    virtual void load() override
    {
        m_fontStorage.load();
        /*m_font.reset(new OriginFont(FONTS_PATH + "Times New Roman 20.bff"));
        m_font->load();
        std::cout << "Loaded font: \"" << m_font->name() << "\", height: " << m_font->fontSize() << std::endl;*/

        //auto filledRect = std::make_shared<TextureRect>();
        //filledRect->setRect(BoundingBox(Vec2(-256, -256), Vec2(256, 256)));
        //filledRect->setTexture(m_font->texture());
        //filledRect->setColor(Color(1, 0, 0));
        //m_rootObject = filledRect;

        /*std::string text("Привет, мир!");
        std::vector<AlignedString> alignedText;
        alignedText.push_back(AlignedString(BoundingBox(Vec2(0, 0)), std::move(text)));
        auto textGeom = createTextGeometry(alignedText, m_font.get());
        m_textGeomBuffers = createTextGeometryBuffers(textGeom);*/

        float y = -300;
        const std::string text("Привет, мир!");
        for (unsigned int fontSize = 16; fontSize <= 40; fontSize += 2, y += 40) {
            m_textGeomBuffers.push_back(createLabel(
                text, Vec2(-500, y), "Times New Roman", static_cast<float>(fontSize)));
            m_textGeomBuffers.push_back(createLabel(
                text, Vec2(-250, y), "Arial", static_cast<float>(fontSize)));
        }

        std::string bigText("«Космические рейнджеры» — компьютерная игра в научно-фантастическом стиле, разработанная российской студией Elemental Games. Издана компанией 1С в 2002 году. На Западе игра была выпущена под названием Space rangers. Игра принадлежит к жанру «эпических игр» с элементами RPG, пошаговой стратегии, текстового квеста и аркады. Игра повествует о противоборстве между Галактическим содружеством, состоящим из пяти рас, включая людей, и вторгшимися силами клисан — неизвестной, агрессивно настроенной формой жизни. Игроку предстоит выступить в роли рейнджера-добровольца, участника организации, созданной специально для борьбы с клисанами.");
        auto font = m_fontStorage.getFont("Arial", 32);
        AlignProperties alignProps(font.get());
        alignProps.horAlign = HorAlign::Center;
        alignProps.vertAlign = VertAlign::Center;
        auto alignedText = alignText(bigText, alignProps, BoundingBox(Vec2(0, -350), Vec2(500, 350)));
        auto textGeometry = createTextGeometry(alignedText, font.get());
        m_textGeomBuffers.push_back(std::make_pair(font, createTextGeometryBuffers(textGeometry)));
    }

    virtual void render() override
    {
        /*const auto& program = textureProgram();
        program.transform = projectionTransform();
        program.texture = m_font->texture();
        program.color = Color(1, 1, 0);
        program.draw(m_textGeomBuffers.vbo, m_textGeomBuffers.ibo);*/

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
        auto font = m_fontStorage.getFont(fontFamily, fontSize);
        std::vector<AlignedString> alignedText;
        alignedText.push_back(AlignedString(BoundingBox(pos), text));
        auto textGeom = createTextGeometry(alignedText, font.get());
        return std::make_pair(font, createTextGeometryBuffers(textGeom));
    }

    FontStorage m_fontStorage;
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
