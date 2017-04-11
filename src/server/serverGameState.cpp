#include <cstdio>
#include <unordered_map>
#include <utility>

#include "../game/Game.h"
#include "../player/Marine.h"
#include "../creeps/Zombie.h"
#include "../game/GameManager.h"
#include "../inventory/weapons/HandGun.h"
#include "../inventory/weapons/Rifle.h"
#include "../inventory/weapons/ShotGun.h"
#include "servergamestate.h"

GameManager *gm = GameManager::GameManager::instance();
std::vector<AttackAction> attackList;
std::vector<DeleteAction> deleteList;
std::vector<WeaponDropAction> dropList;

/**
 * Saves a attack action in the vector.
 * This vector stores every attack action received since the last sync packet was sent.
 * John Agapeyev March 19
 */
void saveAttack(const AttackAction& aa) {
    attackList.push_back(aa);
}

/**
 * Clears all the stored attack actions.
 * John Agapeyev March 19
 */
void clearAttackActions() {
    attackList.clear();
}

void clearDeleteActions() {
    deleteList.clear();
}

void clearWeaponDrops() {
    dropList.clear();
}

/**
 * Updates a player marine based on a given move action struct.
 * John Agapeyev March 19
 */
void updateMarine(const MoveAction& ma) {
    std::lock_guard<std::mutex> lock(mut);
    if (gm->hasMarine(ma.id)) {
        auto& marine = gm->getMarine(ma.id);
        marine.setPosition(ma.xpos, ma.ypos);
        marine.setDX(ma.xdel);
        marine.setDY(ma.ydel);
        marine.setVelocity(ma.vel);
        marine.setAngle(ma.direction);
    } else {
        logv("Marine not found with id %d\n", ma.id);
    }
}

void performAttack(const AttackAction& aa) {
    std::lock_guard<std::mutex> lock(mut);
    switch(aa.entitytype) {
        case UDPHeaders::MARINE:
            if (gm->hasMarine(aa.entityid)) {
                GameManager::instance()->getWeapon(aa.weaponid)->fire(aa.xpos, aa.ypos, aa.direction);
            } else {
                logv("Marine not found with id %d\n", aa.entityid);
            }
            break;
        case UDPHeaders::ZOMBIE:
            if (gm->zombieExists(aa.entityid)) {
                auto& zombie = gm->getZombie(aa.entityid);
                zombie.setPosition(aa.xpos, aa.ypos);
                zombie.setAngle(aa.direction);
                zombie.zAttack();
            } else {
                logv("Zombie not found with id %d\n", aa.entityid);
            }
            break;
        case UDPHeaders::TURRET:
            if (gm->hasTurret(aa.entityid)) {
                auto& turret = gm->getTurret(aa.entityid);
                turret.setPosition(aa.xpos, aa.ypos);
                turret.setAngle(aa.direction);
                turret.shootTurret();
            } else {
                logv("Turret not found with id %d\n", aa.entityid);
            }
            break;
        default:
            break;
    }
}

void processBarricade(const BarricadeAction& ba) {
    std::lock_guard<std::mutex> lock(mut);
    Barricade& tempBarricade = GameManager::instance()->getBarricade(ba.barricadeid);
    if (ba.actionid == UDPHeaders::PICKUP) {
        //No noticeable code in game logic for picking up a barricade
    } else if (ba.actionid == UDPHeaders::DROPOFF) {
        if (tempBarricade.isPlaceable()) {
            tempBarricade.placeBarricade();
            tempBarricade.setPosition(ba.xpos, ba.ypos);
        }
    } else {
        logv("Received barricade packet with unknown action id\n");
    }
}

void processTurret(const TurretAction& ta) {
    std::lock_guard<std::mutex> lock(mut);
    Turret& tempTurret = GameManager::instance()->getTurret(ta.turretid);
    if (ta.actionid == UDPHeaders::PICKUP) {
        tempTurret.pickUpTurret();
        tempTurret.setPosition(ta.xpos, ta.ypos);
    } else if (ta.actionid == UDPHeaders::DROPOFF) {
        tempTurret.placeTurret();
        tempTurret.setPosition(ta.xpos, ta.ypos);
    } else {
        logv("Received turret packet with unknown action id\n");
    }
}

void handleDropRequest(const WeaponDropAction& wda) {
    if (wda.weaponid == -1) {
        GameManager::instance()->getMarine(wda.playerid).checkStoreCollision();
    } else {
        //Player dropped an existing weapon
        gm->createWeaponDrop(wda.xpos, wda.ypos, wda.weaponid);
    }
}

/**
 * Creates a vector of PlayerData structs for use in generating outut
 * packets.
 * The PlayerData struct is defined in UDPHeaders.h
 * John Agapeyev March 19
 */
std::vector<PlayerData> getPlayers() {
    std::vector<PlayerData> rtn;
    PlayerData tempPlayer;
    std::lock_guard<std::mutex> lock(mut);
    for (const auto& idPlayerPair : gm->getMarineManager()) {
        const auto& marine = idPlayerPair.second;
        memset(&tempPlayer, 0, sizeof(tempPlayer));

        tempPlayer.playerid = idPlayerPair.first;
        tempPlayer.xpos = marine.getX();
        tempPlayer.ypos = marine.getY();
        tempPlayer.xdel = marine.getDX();
        tempPlayer.ydel = marine.getDY();
        tempPlayer.vel = marine.getVelocity();
        tempPlayer.direction = marine.getAngle();
        tempPlayer.health = marine.getHealth();

        logv("Player ID: %d\nPlayer x: %f\nPlayer y: %f\nPlayer dx: %f\nPlayer dy: %f\nDirection: %f\n",
            tempPlayer.playerid, tempPlayer.xpos, tempPlayer.ypos, tempPlayer.xdel, tempPlayer.ydel, tempPlayer.direction);

        rtn.push_back(tempPlayer);
    }
    return rtn;
}

/**
 * Similar to the getPlayers() method defined above, except this method retrieves
 * the ZombieData struct contents for each zombie.
 * John Agapeyev March 19
 */
std::vector<ZombieData> getZombies() {
    std::vector<ZombieData> rtn;
    ZombieData tempZombie;
    std::lock_guard<std::mutex> lock(mut);
    for (const auto& idZombiePair : gm->getZombieManager()) {
        const auto& zombie = idZombiePair.second;
        memset(&tempZombie, 0, sizeof(tempZombie));

        tempZombie.zombieid = idZombiePair.first;
        tempZombie.health = zombie.getHealth();
        tempZombie.xpos = zombie.getX();
        tempZombie.ypos = zombie.getY();
        tempZombie.xdel = zombie.getDX();
        tempZombie.ydel = zombie.getDY();
        tempZombie.direction = zombie.getAngle();

        rtn.push_back(tempZombie);
    }
    return rtn;
}

std::vector<DeleteAction> getDeletions() {
    return deleteList;
}

std::vector<TurretData> getTurrets() {
    std::vector<TurretData> rtn;
    TurretData tempturret;
    std::lock_guard<std::mutex> lock(mut);
    for (const auto& idturretPair : gm->getTurretManager()) {
        const auto& turret = idturretPair.second;
        if (turret.isActivated() && turret.isPlaced()) {
            memset(&tempturret, 0, sizeof(tempturret));

            tempturret.turretid = idturretPair.first;
            tempturret.xpos = turret.getX();
            tempturret.ypos = turret.getY();
            tempturret.direction = turret.getAngle();
            tempturret.health = turret.getHealth();

            rtn.push_back(tempturret);
        }
    }
    return rtn;
}

void deleteEntity(const DeleteAction& da) {
    switch(da.entitytype) {
        case UDPHeaders::MARINE:
            gm->deleteMarine(da.entityid);
            break;
        case UDPHeaders::ZOMBIE:
            gm->deleteZombie(da.entityid);
            break;
        case UDPHeaders::TURRET:
            gm->deleteTurret(da.entityid);
            break;
        case UDPHeaders::BARRICADE:
            gm->deleteBarricade(da.entityid);
            break;
        case UDPHeaders::WEAPON:
            gm->removeWeapon(da.entityid);
            break;
        case UDPHeaders::WEAPONDROP:
            gm->deleteWeaponDrop(da.entityid);
            break;
        default:
            logv("Deletion packet with unknown type received\n");
            break;
    }
}

void saveDeletion(const DeleteAction& da) {
    deleteList.push_back(da);
}

void saveDrop(const WeaponDropAction& wda) {
    dropList.push_back(wda);
}

std::vector<WeaponDropAction> getDrops() {
    return dropList;
}

/**
 * Simple wrapper to start the game from a game logic perspective
 * John Agapeyev March 19
 */
void startGame() {
    Game game;
    game.run();
}
