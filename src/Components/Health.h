#ifndef UNTITLED_HEALTH_H
#define UNTITLED_HEALTH_H


struct Health: public Component {
    Health(int health): health(health), maxHealth(health) {}
    int health;
    int maxHealth;
};


#endif //UNTITLED_HEALTH_H
