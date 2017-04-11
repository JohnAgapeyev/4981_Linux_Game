/*------------------------------------------------------------------------------
* Source: MeleeWeapon.cpp        
*
* Functions:
*
* Date: 
*
* Revisions: 
* Edited By : Tim Makimov on 2017/APR/10
*
* Designer: 
*
* Author: 
*
* Notes:
------------------------------------------------------------------------------*/


/*
    Created  by Mark Tattrie    March 28, 2017
*/
#include "MeleeWeapon.h"
#include "../../collision/HitBox.h"
#include "../../game/GameManager.h"
#include "../../collision/CollisionHandler.h"
#include "../../audio/AudioManager.h"
#include <queue>
#include <stdio.h>
#include <iostream>
#include "../../log/log.h"
#include <memory>

/**
 * Date: Mar 28, 2017
 * Author: Mark Tattrie
 * Function Interface: MeleeWeapon::MeleeWeapon(string type, TEXTURES sprite, string fireSound,
 *       string hitSound, string reloadSound, string emptySound, int range, int damage, int AOE,
 *       int penetration, int clip, int clipMax, int ammo, int reloadDelay, int fireDelay, int32_t id)
 * Description:
 * Ctor for MeleeWeapon
 */
MeleeWeapon::MeleeWeapon(const string& type, TEXTURES sprite, const string& fireSound,
        const string& hitSound, const string& reloadSound, const string& emptySound, const int range,
        const int damage, const int AOE, const int penetration, const int accuracy, const int clip, const int clipMax,
        const int ammo, const int reloadDelay, const int fireDelay, const int texX, const int texY, int32_t id)
: Weapon(type, sprite, fireSound, hitSound, reloadSound, emptySound, range, damage, AOE,
        penetration, accuracy, clip, clipMax, ammo, reloadDelay, fireDelay, texX, texY, id) {

}

/**
 * Date: Mar 28, 2017
 * Author: Mark Tattrie
 * Function Interface: bool MeleeWeapon::fire(Movable &mov)
 * Description:
 * Melee weapon takes in a Movable entity, grabs its position and angle then puts a hitbox infront
 * of it based on the range of the weapon. Then checks for collision between that hitbox and other
 * objects.
 */
bool MeleeWeapon::fire(const float x, const float y, const double angle) {
    if (!Weapon::fire(x, y, angle)) {
        return false;
    }

    const double degrees = angle - 90;
    const double radians = degrees * M_PI / 180;

    const int originX = x + (Z_WIDTH / 2);
    const int originY = y + (Z_HEIGHT / 2);
    const int deltaX  = range / 2 * cos(radians);
    const int deltaY  = range / 2 * sin(radians);

    const int endX = originX + deltaX;
    const int endY = originY + deltaY;

    const int hitBoxX = endX - (range / 2);
    const int hitBoxY = endY - (range / 2);
    const int hitBoxW = range;
    const int hitBoxH = range;

    HitBox hitBox({hitBoxX, hitBoxY, hitBoxW, hitBoxH});
    CollisionHandler& ch = GameManager::instance()->getCollisionHandler();

    Entity tempEntity(0, hitBox.getRect());
    for(const auto& x: ch.detectMeleeCollision(ch.getMarineTree().retrieve(&tempEntity), hitBox)) {
        //update hit marine
        x->collidingProjectile(damage);
    }

    for(const auto& y : ch.detectMeleeCollision(ch.getTurretTree().retrieve(&tempEntity), hitBox)) {
        //update hit turret
        y->collidingProjectile(damage);
    }

    for(const auto& z : ch.detectMeleeCollision(ch.getBarricadeTree().retrieve(&tempEntity), hitBox)) {
        //update hit barricade
        z->collidingProjectile(damage);
    }
    ++clip;

    return true;
}
