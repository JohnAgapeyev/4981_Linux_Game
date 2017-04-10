/*
    Created by Maitiu Morton 2/1/2017
        Edited by DericM 3/8/2017
        Edited by Mark Tattrie
*/
#include "Weapon.h"
#include <iostream>
#include <atomic>

#include "../../log/log.h"
#include "../../audio/AudioManager.h"
#include "../../game/GameManager.h"

using std::string;

/**
 * Date: Feb 8, 2017
 * Modified:
 *  Feb 9, 2017 - Jacob McPhail
 *  Mar 13, 2017 - Mark Tattrie
 * Author: Maitiu Morton
 * Function Interface: Weapon::Weapon(const string& type, TEXTURES sprite, const string& fireSound,
 *       const string& hitSound, const string& reloadSound, const string& emptySound, const int range,
 *       const int damage, const int AOE, const int penetration, const int clip, const int clipMax,
 *       const int ammo, const int reloadDelay, const int fireDelay, int32_t id)
 * Description:
 * Ctor for Weapon
 */
Weapon::Weapon(const string& type, TEXTURES sprite, const string& fireSound, const string& hitSound, const string& reloadSound,
        const string& emptySound, const int range, const int damage, const int AOE, const int penetration, const int accuracy,
        const int clip, const int clipMax, const int ammo, const int reloadDelay, const int fireDelay, int32_t id)
: type(type), spriteType(sprite), fireSound(fireSound), hitSound(hitSound), reloadSound(reloadSound), emptySound(emptySound),
        range(range), damage(damage), AOE(AOE), penetration(penetration), accuracy(accuracy), clip(clip), clipMax(clipMax), ammo(ammo),
        reloadDelay(reloadDelay), fireDelay(fireDelay), reloadTick(0), fireTick(0),  wID(id){

}

Weapon::Weapon(const Weapon& w)
: type(w.type), spriteType(w.spriteType), fireSound(w.fireSound), hitSound(w.hitSound), reloadSound(w.reloadSound), emptySound(w.emptySound),
        range(w.range), damage(w.damage), AOE(w.AOE), penetration(w.penetration), accuracy(w.accuracy), clip(w.clip), clipMax(w.clipMax),
        ammo(w.ammo), reloadDelay(w.reloadDelay), fireDelay(w.fireDelay), reloadTick(w.reloadTick),
        fireTick(w.fireTick), wID(w.getID()){
}


//Deric M       3/3/2017
bool Weapon::reduceClip(const int rounds){
    if(clip < rounds){
        reloadClip();
        return false;
    }
    clip -= rounds;
    return true;
}


//Mark T    3/8/2017
//Deric M       3/15/2017
bool Weapon::reloadClip(){
    int currentTime = SDL_GetTicks();
    if(currentTime < (reloadTick + reloadDelay)){
        return false;
    }
    reloadTick = currentTime;
    //must wait extra time to fire from reloading
    fireTick += reloadDelay;
    if(ammo <= 0){
        AudioManager::instance().playEffect(emptySound.c_str());
        return false;
    }

    int ammoNeeded = clipMax - clip;
    if(ammo < ammoNeeded){
        clip += ammo;
        ammo = 0;
        return true;
    }
    ammo -= ammoNeeded;
    clip += ammoNeeded;
    AudioManager::instance().playEffect(reloadSound.c_str());

    return true;
}


//Mark T    3/8/2017
//Deric M       3/15/2017
bool Weapon::chamberRound() {
    int currentTime = SDL_GetTicks();
    if(currentTime < (fireTick + fireDelay)){
        return false;
    }
    fireTick = currentTime;
    if(!reduceClip(1)){
        return false;
    }
    logv(3, "Current ammo: %d/%d\n", clip, ammo + clip);
    return true;
}

//Deric M       3/15/2017
/*
Previously this function took a movable. The reason this was changed is because
firing on the server should happen from the location the shot was fired, not the
current position of the movable.
*/
bool Weapon::fire(const float x, const float y, const double angle) {
    logv(3, "Weapon::fire()\n");
    if(!chamberRound()){
        return false;
    }
#ifndef SERVER
    if (networked) {
        if (type == TurretGunVars::TYPE) {
            GameManager::instance()->getZombie(movable.getId()).sendServAttackAction();
        } else if (type == ZombieHandVars::TYPE) {
            GameManager::instance()->getTurret(movable.getId()).sendServAttackAction();
        } else {
            GameManager::instance()->getPlayer().sendServAttackAction();
        }
    }
    AudioManager::instance().playEffect(fireSound.c_str());
#endif
    return true;
}
