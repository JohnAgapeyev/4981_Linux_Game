#ifndef SERVERGAMESTATE_H
#define SERVERGAMESTATE_H

#include <vector>
#include <utility>
#include "../game/GameManager.h"
#include "../UDPHeaders.h"
#include "server.h"

extern GameManager *gm;
extern std::vector<AttackAction> attackList;
extern std::vector<DeleteAction> deleteList;
extern std::vector<WeaponDropAction> dropList;

void updateMarine(const MoveAction& ma);
void performAttack(const AttackAction& aa);
void processBarricade(const BarricadeAction& ba);
void processTurret(const TurretAction& ta);
void deleteEntity(const DeleteAction& da);
void handleDropRequest(const WeaponDropAction& wda);
void saveDeletion(const DeleteAction& da);
void saveAttack(const AttackAction& aa);
void saveDrop(const WeaponDropAction& wda);
void clearAttackActions();
void clearDeleteActions();
void clearWeaponDrops();
void startGame();

std::vector<PlayerData> getPlayers();
std::vector<ZombieData> getZombies();
std::vector<DeleteAction> getDeletions();
std::vector<WeaponDropAction> getDrops();

#endif
