#include "Text.hpp"

using namespace alce;

void Text::Init()
{
    
}

void Text::Start()
{

}

void Text::Render()
{
    Alce.GetWindow().draw(richText);

    if (borderRadius > 0)
    {
        sf::ConvexShape roundedBox;

        roundedBox.setPointCount(8);
        roundedBox.setPoint(0, sf::Vector2f(borderRadius, 0));
        roundedBox.setPoint(1, sf::Vector2f(size.x - borderRadius, 0));
        roundedBox.setPoint(2, sf::Vector2f(size.x, borderRadius));
        roundedBox.setPoint(3, sf::Vector2f(size.x, size.y - borderRadius));
        roundedBox.setPoint(4, sf::Vector2f(size.x - borderRadius, size.y));
        roundedBox.setPoint(5, sf::Vector2f(borderRadius, size.y));
        roundedBox.setPoint(6, sf::Vector2f(0, size.y - borderRadius));
        roundedBox.setPoint(7, sf::Vector2f(0, borderRadius));
        roundedBox.setPosition(transform.position.ToVector2f());
        roundedBox.setFillColor(backgroundColor.ToSFMLColor());
        roundedBox.setOutlineThickness(borderWidth);
        roundedBox.setOutlineColor(borderColor.ToSFMLColor());
        Alce.GetWindow().draw(roundedBox);
    }
    else 
    {
        sf::RectangleShape border;

        border.setPosition(transform.position.ToVector2f());
        border.setSize(size.ToVector2f());
        border.setOutlineThickness(borderWidth);
        border.setOutlineColor(borderColor.ToSFMLColor());
        border.setFillColor(sf::Color::Transparent);
        
        Alce.GetWindow().draw(border);
    }

    Alce.GetWindow().draw(richText);
}

sf::Color MapColorFromString(String colorString) 
{
    Dictionary<String, Color> colorMap = {
        {"red", alce::Colors::Red},
        {"green", alce::Colors::Green},
        {"blue", alce::Colors::Blue},
        {"white", alce::Colors::White},
        {"black", alce::Colors::Black},
        {"yellow", alce::Colors::Yellow},
        {"cyan", alce::Colors::Cyan},
        {"magenta", alce::Colors::Magenta},
        {"orange", alce::Colors::Orange},
        {"purple", alce::Colors::Purple},
        {"pink", alce::Colors::Pink},
        {"transparent", alce::Colors::Transparent}
    };

    if (colorMap.HasKey(colorString)) 
    {
        return colorMap.Get(colorString).ToSFMLColor();
    }

    if (colorString[0] == '#') 
    {
        Color hexColor(colorString);
        return hexColor.ToSFMLColor();
    }

    return sf::Color::White;
}

void Text::Update() 
{
    if (!enabled) return;

    richText = sfe::RichText(*Alce.GetFont(font).get());
    richText.setCharacterSize(fontSize);

    Vector2 pos = transform.position + padding;
    richText.setPosition(pos.ToVector2f());
    richText.setRotation(transform.rotation);
    richText.setScale(transform.scale.ToVector2f());

    std::string text = ToAnsiString();  

    std::regex tagRegex(R"(<(\/?)([a-zA-Z]+)(?:='([^']+)')?>)");
    std::stack<sf::Text::Style> styleStack;
    std::stack<sf::Color> colorStack;
    sf::Text::Style currentStyle = sf::Text::Regular;
    sf::Color currentColor = color.ToSFMLColor();

    bool isBold = false, isItalic = false, isUnderlined = false, isStrikeThrough = false;

    std::sregex_iterator iter(text.begin(), text.end(), tagRegex);
    std::sregex_iterator end;
    std::size_t lastPos = 0;

    for (; iter != end; ++iter) 
    {
        auto match = *iter;
        std::string tag = match[2].str();
        bool isClosingTag = match[1].str() == "/";
        std::string tagValue = match[3].str();
        std::size_t pos = match.position();

        if (pos > lastPos) 
        {
            currentStyle = sf::Text::Regular;
            if (isBold) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::Bold);
            if (isItalic) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::Italic);
            if (isUnderlined) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::Underlined);
            if (isStrikeThrough) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::StrikeThrough);

            richText << currentColor << currentStyle << sf::String::fromUtf8(text.begin() + lastPos, text.begin() + pos);
        }

        if (!isClosingTag) 
        {
            if (tag == "bold") 
            {
                styleStack.push(currentStyle);
                isBold = true;
            } 
            else if (tag == "italic") 
            {
                styleStack.push(currentStyle);
                isItalic = true;
            } 
            else if (tag == "underlined") 
            {
                styleStack.push(currentStyle);
                isUnderlined = true;
            } 
            else if (tag == "strikeThrough") 
            {
                styleStack.push(currentStyle);
                isStrikeThrough = true;
            } 
            else if (tag == "color") 
            {
                colorStack.push(currentColor);
                currentColor = MapColorFromString(tagValue);
            }
        } 
        else 
        {
            if (tag == "bold") 
            {
                currentStyle = styleStack.top();
                styleStack.pop();
                isBold = false;
            } 
            else if (tag == "italic") 
            {
                currentStyle = styleStack.top();
                styleStack.pop();
                isItalic = false;
            } 
            else if (tag == "underlined") 
            {
                currentStyle = styleStack.top();
                styleStack.pop();
                isUnderlined = false;
            } 
            else if (tag == "strikeThrough") 
            {
                currentStyle = styleStack.top();
                styleStack.pop();
                isStrikeThrough = false;
            } 
            else if (tag == "color") 
            {
                currentColor = colorStack.top();
                colorStack.pop();
            }
        }

        lastPos = pos + match.length();
    }

    if (lastPos < text.size()) 
    {
        currentStyle = sf::Text::Regular;
        if (isBold) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::Bold);
        if (isItalic) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::Italic);
        if (isUnderlined) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::Underlined);
        if (isStrikeThrough) currentStyle = static_cast<sf::Text::Style>(currentStyle | sf::Text::StrikeThrough);

        richText << currentColor << currentStyle << sf::String::fromUtf8(text.begin() + lastPos, text.end());
    }

    size = Vector2(richText.getGlobalBounds().width, richText.getGlobalBounds().height);
    size += (padding * 2);
}