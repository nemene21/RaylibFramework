#include <iostream>
#include <map>
#include <raylib.h>
#include <component.h>
#include <unordered_set>

class Entity {
protected:
    std::map<ComponentType, Component*> components;
    std::unordered_set<std::string> groups;

    void process_components(float delta);

public:
    Entity();

    virtual void process(float delta);
    virtual void draw(float delta);
};