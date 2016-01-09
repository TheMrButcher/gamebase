#pragma once

#include <gamebase/engine/Panel.h>
#include <gamebase/engine/TextBox.h>
#include <gamebase/text/Conversion.h>

namespace gamebase { namespace editor {

class FilePathDialog {
public:
    FilePathDialog() {}

    FilePathDialog(Panel* panel)
        : m_panel(panel)
        , m_textBox(panel->getChild<TextBox>("#textbox"))
        , m_ok(panel->getChild<Button>("#ok"))
        , m_cancel(panel->getChild<Button>("#cancel"))
    {
        m_panel->setVisible(false);
    }

    void initWithTextAndCancel(
        const std::string& initialText,
        const std::function<void(const std::string&)>& okCallback,
        const std::function<void()>& cancelCallback)
    {
        m_textBox->setText(initialText);
        m_panel->setCloseCallback(cancelCallback);
        m_cancel->setCallback(std::bind(&Panel::close, m_panel));
        m_ok->setCallback(std::bind(&FilePathDialog::processResult, this, okCallback));
        m_panel->resetPosition();
        m_panel->setVisible(true);
    }

    void initWithText(
        const std::string& initialText,
        const std::function<void(const std::string&)>& okCallback)
    {
        initWithTextAndCancel(initialText, okCallback, nullptr);
    }

    void init(const std::function<void(const std::string&)>& okCallback)
    {
        initWithText(result(), okCallback);
    }

    const std::string& result() const { return m_textBox->text(); }

private:
    void processResult(const std::function<void(const std::string&)>& callback)
    {
        if (result().empty())
            return;
        auto fileName = convertToLocal(result());
        if (fileName.empty()) {
            std::cout << "Failed to convert file name to local" << std::endl;
            return;
        }
        m_panel->setVisible(false);
        if (callback)
            callback(fileName);
    }

    Panel* m_panel;
    TextBox* m_textBox;
    Button* m_ok;
    Button* m_cancel;
};

} }
