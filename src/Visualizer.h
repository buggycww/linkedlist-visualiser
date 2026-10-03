#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <deque>
#include <string>
#include <vector>
#include "LinkedList_Pooled.h"
#include "NodePool.h"

class Visualizer {
public:
    Visualizer(LinkedList_Pooled& list, NodePool& pool,
               unsigned width = 1200, unsigned height = 700);

    void queueInsertFront(int value);
    void queueInsertBack(int value);
    void queueDelete(int value);
    void queueWait(const std::string& label, float seconds);

    void run();

private:
    sf::RenderWindow window;
    sf::Font font;

    LinkedList_Pooled& list;
    NodePool& pool;

    struct QueuedAction {
        std::string label;
        float preDelay;
        std::function<void()> action;
        float postDelay;
    };

    std::deque<QueuedAction> originalScript;   // preserved for replay
    std::deque<QueuedAction> script;           // consumed each run

    enum class Phase { PreDelay, PostDelay };
    Phase phase = Phase::PreDelay;
    float timer = 0.f;
    std::string currentLabel;

    int highlightValue = -1;
    sf::Color highlightColor = sf::Color::White;

    bool showingReplayButton = false;
    bool mouseOverButton = false;
    sf::Vector2f mousePos{0.f, 0.f};

    static constexpr float NODE_W   = 70.f;
    static constexpr float NODE_H   = 50.f;
    static constexpr float NODE_GAP = 30.f;
    static constexpr float LIST_Y   = 260.f;
    static constexpr float POOL_Y   = 410.f;
    static constexpr float POOL_BOX = 26.f;
    static constexpr float POOL_GAP = 4.f;

    void processEvents();
    void advanceScript(float dt);
    void drawHeader();
    void drawList();
    void drawPool();
    void drawStatusBar();
    void drawReplayButton();
    void drawArrow(sf::Vector2f from, sf::Vector2f to, sf::Color color);

    sf::FloatRect replayButtonRect() const;
    void triggerReplay();
    std::vector<int> listValues() const;
};