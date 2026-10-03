#include "Visualizer.h"
#include <iostream>
#include <cmath>
#include <optional>

Visualizer::Visualizer(LinkedList_Pooled& l, NodePool& p,
                       unsigned w, unsigned h)
    : window(sf::VideoMode({w, h}), "Linked List Visualizer")
    , list(l), pool(p)
{
    window.setFramerateLimit(60);

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf") &&
        !font.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        std::cerr << "Failed to load font.\n";
    }
}

// ---------- Queue API (all push to originalScript) ----------

void Visualizer::queueInsertFront(int value) {
    QueuedAction q;
    q.label = "insertFront(" + std::to_string(value) + ")";
    q.preDelay = 0.4f;
    q.action = [this, value]{
        list.insertFront(value);
        highlightValue = value;
        highlightColor = sf::Color(80, 220, 120);
    };
    q.postDelay = 0.7f;
    originalScript.push_back(q);
}

void Visualizer::queueInsertBack(int value) {
    QueuedAction q;
    q.label = "insertBack(" + std::to_string(value) + ")";
    q.preDelay = 0.4f;
    q.action = [this, value]{
        list.insertBack(value);
        highlightValue = value;
        highlightColor = sf::Color(80, 220, 120);
    };
    q.postDelay = 0.7f;
    originalScript.push_back(q);
}

void Visualizer::queueDelete(int value) {
    QueuedAction highlight;
    highlight.label = "delete(" + std::to_string(value) + ")";
    highlight.preDelay = 0.f;
    highlight.action = [this, value]{
        highlightValue = value;
        highlightColor = sf::Color(220, 80, 80);
    };
    highlight.postDelay = 0.8f;
    originalScript.push_back(highlight);

    QueuedAction remove;
    remove.label = "";
    remove.preDelay = 0.f;
    remove.action = [this, value]{
        list.deleteValue(value);
        highlightValue = -1;
    };
    remove.postDelay = 0.4f;
    originalScript.push_back(remove);
}

void Visualizer::queueWait(const std::string& label, float seconds) {
    QueuedAction q;
    q.label = label;
    q.preDelay = seconds;
    q.action = []{};
    q.postDelay = 0.f;
    originalScript.push_back(q);
}

// ---------- Main loop ----------

void Visualizer::run() {
    script = originalScript;

    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processEvents();
        advanceScript(dt);

        window.clear(sf::Color(24, 26, 32));
        drawHeader();
        drawList();
        drawPool();
        drawStatusBar();
        drawReplayButton();
        window.display();
    }
}

void Visualizer::processEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        else if (const auto* m = event->getIf<sf::Event::MouseMoved>()) {
            mousePos = { static_cast<float>(m->position.x),
                         static_cast<float>(m->position.y) };
            mouseOverButton = showingReplayButton &&
                              replayButtonRect().contains(mousePos);
        }
        else if (const auto* m = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (m->button == sf::Mouse::Button::Left && showingReplayButton) {
                sf::Vector2f p{ static_cast<float>(m->position.x),
                                static_cast<float>(m->position.y) };
                if (replayButtonRect().contains(p)) triggerReplay();
            }
        }
        else if (const auto* k = event->getIf<sf::Event::KeyPressed>()) {
            if (k->code == sf::Keyboard::Key::R && showingReplayButton)
                triggerReplay();
        }
    }
}

void Visualizer::advanceScript(float dt) {
    if (script.empty()) {
        showingReplayButton = !originalScript.empty();
        return;
    }

    timer += dt;
    QueuedAction& q = script.front();

    if (phase == Phase::PreDelay) {
        if (timer >= q.preDelay) {
            q.action();
            currentLabel = q.label;
            phase = Phase::PostDelay;
            timer = 0.f;
        }
    } else {
        if (timer >= q.postDelay) {
            script.pop_front();
            phase = Phase::PreDelay;
            timer = 0.f;
        }
    }
}

void Visualizer::triggerReplay() {
    list.clear();
    pool.reset();
    script = originalScript;
    highlightValue = -1;
    currentLabel.clear();
    phase = Phase::PreDelay;
    timer = 0.f;
    showingReplayButton = false;
    mouseOverButton = false;
}

// ---------- Drawing ----------

std::vector<int> Visualizer::listValues() const {
    std::vector<int> vals;
    LinkedList_Pooled::Node* cur = list.getHead();
    while (cur) {
        vals.push_back(cur->value);
        cur = cur->next;
    }
    return vals;
}

void Visualizer::drawHeader() {
    sf::Text title(font, "Linked List + Pool Allocator", 26);
    title.setPosition({30.f, 24.f});
    title.setFillColor(sf::Color(220, 220, 240));
    window.draw(title);

    if (!currentLabel.empty()) {
        sf::Text step(font, currentLabel, 20);
        step.setPosition({30.f, 62.f});
        step.setFillColor(sf::Color(200, 180, 120));
        window.draw(step);
    }
}

void Visualizer::drawList() {
    std::vector<int> vals = listValues();
    size_t n = vals.size();
    float winW = static_cast<float>(window.getSize().x);

    if (n == 0) {
        sf::Text empty(font, "(empty list)", 22);
        empty.setFillColor(sf::Color(140, 140, 150));
        empty.setPosition({winW / 2.f - 70.f, LIST_Y});
        window.draw(empty);
        return;
    }

    float totalW = n * NODE_W + (n - 1) * NODE_GAP;
    float startX = (winW - totalW) / 2.f;

    // Two rails per gap: forward on top, backward on bottom.
    const sf::Color arrowColor(120, 160, 220);
    float yTop = LIST_Y + NODE_H * 0.35f;
    float yBot = LIST_Y + NODE_H * 0.65f;

    for (size_t i = 0; i + 1 < n; i++) {
        float x1 = startX + i * (NODE_W + NODE_GAP) + NODE_W;
        float x2 = startX + (i + 1) * (NODE_W + NODE_GAP);

        // next: -> right, arrowhead on the right
        drawArrow({x1 + 3.f, yTop}, {x2 - 3.f, yTop}, arrowColor);
        // prev: <- left, arrowhead on the left
        drawArrow({x2 - 3.f, yBot}, {x1 + 3.f, yBot}, arrowColor);
    }

    for (size_t i = 0; i < n; i++) {
        float x = startX + i * (NODE_W + NODE_GAP);

        sf::RectangleShape box({NODE_W, NODE_H});
        box.setPosition({x, LIST_Y});
        box.setFillColor(vals[i] == highlightValue
                             ? highlightColor
                             : sf::Color(50, 90, 140));
        box.setOutlineThickness(2.f);
        box.setOutlineColor(sf::Color(180, 210, 255));
        window.draw(box);

        sf::Text txt(font, std::to_string(vals[i]), 24);
        sf::FloatRect b = txt.getLocalBounds();
        txt.setPosition({x + (NODE_W - b.size.x) / 2.f - b.position.x,
                         LIST_Y + (NODE_H - b.size.y) / 2.f - b.position.y - 4.f});
        txt.setFillColor(sf::Color::White);
        window.draw(txt);
    }

    sf::Text h(font, "head", 14);
    h.setPosition({startX + NODE_W / 2.f - 16.f, LIST_Y + NODE_H + 8.f});
    h.setFillColor(sf::Color(140, 220, 140));
    window.draw(h);

    float lastX = startX + (n - 1) * (NODE_W + NODE_GAP);
    sf::Text t(font, "tail", 14);
    t.setPosition({lastX + NODE_W / 2.f - 12.f, LIST_Y + NODE_H + 8.f});
    t.setFillColor(sf::Color(140, 220, 140));
    window.draw(t);
}

void Visualizer::drawPool() {
    int cap  = pool.getCapacity();
    int used = pool.getUsed();
    float winW = static_cast<float>(window.getSize().x);

    float totalW = cap * POOL_BOX + (cap - 1) * POOL_GAP;
    float startX = (winW - totalW) / 2.f;

    sf::Text label(font,
                   "Pool: " + std::to_string(used) + " / " + std::to_string(cap),
                   18);
    label.setPosition({startX, POOL_Y - 30.f});
    label.setFillColor(sf::Color(200, 200, 210));
    window.draw(label);

    for (int i = 0; i < cap; i++) {
        sf::RectangleShape box({POOL_BOX, POOL_BOX});
        box.setPosition({startX + i * (POOL_BOX + POOL_GAP), POOL_Y});
        box.setOutlineThickness(1.f);
        box.setOutlineColor(sf::Color(90, 90, 100));
        box.setFillColor(pool.isInUse(i)
                             ? sf::Color(80, 200, 120)
                             : sf::Color(45, 45, 55));
        window.draw(box);
    }
}

void Visualizer::drawStatusBar() {
    std::string info = "size: " + std::to_string(list.getSize());
    if (!script.empty())
        info += "   |   steps remaining: " + std::to_string(script.size());
    else if (showingReplayButton)
        info += "   |   press R or click Replay";

    sf::Text txt(font, info, 16);
    txt.setPosition({30.f, static_cast<float>(window.getSize().y) - 40.f});
    txt.setFillColor(sf::Color(160, 160, 170));
    window.draw(txt);
}

sf::FloatRect Visualizer::replayButtonRect() const {
    constexpr float w = 170.f, h = 52.f;
    float x = (window.getSize().x - w) / 2.f;
    float y = static_cast<float>(window.getSize().y) - 130.f;
    return sf::FloatRect({x, y}, {w, h});
}

void Visualizer::drawReplayButton() {
    if (!showingReplayButton) return;

    sf::FloatRect r = replayButtonRect();

    sf::RectangleShape box({r.size.x, r.size.y});
    box.setPosition({r.position.x, r.position.y});
    box.setFillColor(mouseOverButton ? sf::Color(80, 180, 100)
                                     : sf::Color(60, 140, 80));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color::White);
    window.draw(box);

    sf::Text txt(font, "Replay (R)", 20);
    sf::FloatRect b = txt.getLocalBounds();
    txt.setPosition({r.position.x + (r.size.x - b.size.x) / 2.f - b.position.x,
                     r.position.y + (r.size.y - b.size.y) / 2.f - b.position.y - 3.f});
    txt.setFillColor(sf::Color::White);
    window.draw(txt);
}

void Visualizer::drawArrow(sf::Vector2f from, sf::Vector2f to, sf::Color c) {
    sf::Vertex line[2];
    line[0].position = from;
    line[0].color    = c;
    line[1].position = to;
    line[1].color    = c;
    window.draw(line, 2, sf::PrimitiveType::Lines);

    sf::Vector2f dir = to - from;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 0.001f) return;
    dir /= len;
    sf::Vector2f perp(-dir.y, dir.x);

    const float AH = 10.f, AW = 5.f;
    sf::Vector2f base = to - dir * AH;

    sf::ConvexShape tri(3);
    tri.setPoint(0, to);
    tri.setPoint(1, base + perp * AW);
    tri.setPoint(2, base - perp * AW);
    tri.setFillColor(c);
    window.draw(tri);
}