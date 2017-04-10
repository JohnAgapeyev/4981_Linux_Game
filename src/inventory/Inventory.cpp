//created 2/5/17 Maitiu
#include <memory>

#include "Inventory.h"
#include "../game/GameManager.h"
#include "../log/log.h"
#include "../client/NetworkManager.h"

Inventory::Inventory() {
    if (!networked) {
        HandGun hGun(GameManager::instance()->generateID());
        GameManager::instance()->addWeapon(std::dynamic_pointer_cast<Weapon>(std::make_shared<HandGun>(hGun)));
        weaponIds[0] = hGun.getID();
    } else {
        weaponIds[0] = -1;
    }
    weaponIds[1] = -1;
    weaponIds[2] = -1;
}


void Inventory::switchCurrent(const int slot) {
    if (current != slot) {
        logv(3, "Switched to slot: %d\n", slot);
        current = slot;
    }
}

//Created By Maitiu
bool Inventory::pickUp(int32_t weaponId, const float x, const float y) {
    if (current == 0) {
        logv(3, "Can't Swap default gun \n");
        return false;
    }

    //drop Current Weapon

    dropWeapon(x, y);

    logv(3, "Picked up weapon\n");
    logv(3, "Swapped from %d ", weaponIds[current]);
    weaponIds[current] = weaponId;
    logv(3, "to %d\n\n", weaponIds[current]);
    return true;
}

/*Created By Maitiu
 * Returns pointer to Weapon in current slot
 * If slot is empty it returns a null pointer
 */
Weapon* Inventory::getCurrent() const {
    if (weaponIds[current] >= 0) {
        return GameManager::instance()->getWeapon(weaponIds[current]).get();
    }
    return nullptr;
}

/**
 * Function: getWeaponFromInventory
 *
 * Date:
 * JF: April 4, 2017:
 *
 * Designer:
 * Jacob Frank
 *
 * Programmer:
 * Jacob Frank
 *
 * Interface: getWeaponFromInventory(int inventorySlot)
 *                  int inventorySlot: The inventory slot to retrieve the weapon from
 *
 * Returns: A Weapon from the inventory if one is available
 *
 * Notes:
 * Function, when called, retrieves the weapon from the inventory slot requested
 */
Weapon* Inventory::getWeaponFromInventory(int inventorySlot) {
    if (weaponIds[inventorySlot] >= 0) {
        return GameManager::instance()->getWeapon(weaponIds[inventorySlot]).get();
    }
    return nullptr;
}

void Inventory::useItem() {
    if (medkit != nullptr) {
        medkit->OnConsume();
        medkit = nullptr;
    }

}

//Created By Maitiu
void Inventory::scrollCurrent(int direction) {
    int currentTime = SDL_GetTicks();

    if (currentTime > (slotScrollTick + scrollDelay)) {
        slotScrollTick = currentTime;
        direction += current;
        if (direction < 0) {
            current = 2;
        } else if (direction > 2) {
            current = 0;
        } else {
            current = direction;
        }
        logv(3, "Switched to slot:%d\n", current);
    }

}

/**
 * DEVELOPER: Maitiu
 * DESIGNER: Maitiu
 * DATE:      March 29 2017
 * Checks is current CSLot has Weapon then Checks its ammo and creates a weaponDrop and renders it.
 */
 void Inventory::dropWeapon(const float x, const float y) {
     if(current){
         Weapon *w = getCurrent();
         if (w) {
             if (networked) {
                 const std::string& weaponType = w->getType();
                 if (weaponType.compare("Rifle")) {
                     GameManager::instance()->getPlayer().sendServWeaponDropAction(weaponIds[current], UDPHeaders::RIFLE);
                 } else if(weaponType.compare("Shotgun")) {
                     GameManager::instance()->getPlayer().sendServWeaponDropAction(weaponIds[current], UDPHeaders::SHOTGUN);
                 } else if(weaponType.compare("Handgun")) {
                     GameManager::instance()->getPlayer().sendServWeaponDropAction(weaponIds[current], UDPHeaders::PISTOL);
                 }
             } else {
                 if (w->getAmmo() > 0) {
                     GameManager::instance()->createWeaponDrop(x,y, weaponIds[current]);

                 } else {
                     //delete weapon From Weapon Manager
                     GameManager::instance()->removeWeapon(weaponIds[current]);
                 }
                 weaponIds[current] = -1;
            }
         }
     }
 }

 /**
 * Date: Mar. 30, 2017
 * Designer: Mark Chen
 * Programmer: Mark Chen
 * Function Interface: void makeTurretInv()
 * Description:
 * Switches the defaultGun to a turretGun.
 */
 void Inventory::makeTurretInv() {
     //Weapon *w = getCurrent();
     GameManager::instance()->removeWeapon(weaponIds[current]);
     TurretGun tGun(GameManager::instance()->generateID());
     weaponIds[0] = tGun.getID();
     GameManager::instance()->addWeapon(std::dynamic_pointer_cast<Weapon>(std::make_shared<TurretGun>(tGun)));
 }

/**
* Date: April 10, 2017
* Designer: Mark Tattrie
* Programmer: Mark Tattire
* Function Interface: void makeZombieInv()
* Description:
* Switches the defaultGun to a ZombieHand.
*/
void Inventory::makeZombieInv() {
    //Weapon *w = getCurrent();
    GameManager::instance()->removeWeapon(weaponIds[current]);
    ZombieHand zHand(GameManager::instance()->generateID());
    weaponIds[0] = zHand.getID();
    GameManager::instance()->addWeapon(std::dynamic_pointer_cast<Weapon>(std::make_shared<ZombieHand>(zHand)));
}
