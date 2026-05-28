#ifndef __BARILLET_H__
#define __BARILLET_H__
#define SECTION_ROTATION (4096*1.2)
#define CAPTEUR_PIN 18
extern uint8_t start_barillet;
extern u32 nb_section;
extern u32 dist_total;
extern uint8_t etat_barillet;
uint8_t init_barillet(void);
uint8_t free_barillet(void);
void action_tourne_barillet_loop(void);
uint8_t action_tourne_barillet (void);
uint8_t action_tourne_barillet_absolue(void);

#endif // __BARILLET_H__