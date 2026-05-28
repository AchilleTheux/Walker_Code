#ifndef __AU_H_
#define __AU_H_

extern uint8_t State_AU;
u8 trigger_Soft_AU(void);
void init_AU(void);
void AU_Loop(void);

/// arret d'urgence ///
#define PULL_AU 0 // Bouton non appuyé
#define PUSH_AU 1 // Bouton appuyé

#define PIN_AU 3
#define EN_12V_PIN 19

#endif // __AU_H_
