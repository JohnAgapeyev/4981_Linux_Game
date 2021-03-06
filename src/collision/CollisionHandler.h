/*------------------------------------------------------------------------------
* Header: CollisionHandler.h
*
* Functions:
*
*
* Date:
*
* Revisions:
* Edited By : Justen DePourcq- Style guide
*
* Designer:
*
* Author:
*
* Notes:
*
------------------------------------------------------------------------------*/
#ifndef COLLISION_H
#define COLLISION_H

#include <vector>
#include <queue>

#include "HitBox.h"
#include "Quadtree.h"
#include "../inventory/weapons/Target.h"

class Movable;

class CollisionHandler {
public:
    CollisionHandler();
    ~CollisionHandler() = default;

    // Check for damage collisions, return object if hits
    const HitBox *detectDamageCollision(std::vector<Entity*> returnObjects, const Entity *entity);

    // Check for projectile collisions, return object if hits
    const HitBox *detectProjectileCollision(std::vector<Entity*> returnObjects, const Entity *entity);

    //Check for collisions during movement
    bool detectMovementCollision(const std::vector<Entity*> returnObjects, const Entity *entity);

    //check for pick up collision, return object if can pick up
    Entity *detectPickUpCollision(std::vector<Entity*> returnObjects, const Entity *entity);

    void detectLineCollision(TargetList& targetList, const int gunX, const int gunY, const double angle, const int range);

    std::vector<Entity *> detectMeleeCollision(std::vector<Entity *> entities, const HitBox& hb);
    std::vector<Entity *>getQuadTreeEntities(Quadtree& q, const Entity *entity); // General Collision handler, pass in quadtree check

    std::vector<Entity *>getQuadTreeEntities(const Quadtree& q, const Entity *entity) const; // General Collision handler, pass in quadtree check

    void clear();
    void insertMarine(Entity *e);
    void insertZombie(Entity *e);
    void insertBarricade(Entity *e);
    void insertTurret(Entity *e);
    void insertWall(Entity *e);
    void insertPickUp(Entity *e);
    void insertObj(Entity *e);
    void insertStore(Entity *e);

    auto& getZombieMovementTree() const {return zombieMovementTree;}
    auto& getMarineTree() const {return marineTree;}
    auto& getZombieTree() const {return zombieTree;}
    auto& getBarricadeTree() const {return barricadeTree;}
    auto& getTurretTree() const {return turretTree;}
    auto& getWallTree() const {return wallTree;}
    auto& getPickUpTree() const {return pickUpTree;}
    auto& getObjTree() const {return objTree;}
    auto& getStoreTree() const {return storeTree;}

private:
    void checkForTargetsInVector(const int gunX, const int gunY, const int endX, const int endY,
        TargetList& targetList, const std::vector<Entity*>& allEntities, const int type) const;

    void insertZombieMovementEntity(Entity *e);

    Quadtree zombieMovementTree;
    Quadtree marineTree; //can take dmg
    Quadtree zombieTree; //can take dmg
    Quadtree barricadeTree; //can take dmg
    Quadtree turretTree;
    Quadtree wallTree;
    Quadtree pickUpTree;
    Quadtree objTree;
    Quadtree storeTree;
};


#endif
