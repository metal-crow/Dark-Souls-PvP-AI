#ifndef WeaponData_H
#define WeaponData_H

int StaminaDrainForAttack(unsigned int weapon_id, unsigned short animationType_id);

float PoiseDamageForAttack(unsigned int weapon_id, unsigned short animationType_id);

unsigned char IsWeaponShield(unsigned int weapon_id);

#endif