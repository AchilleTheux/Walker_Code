#ifndef __GUIDE_H_
#define __GUIDE_H_

#define bras_gauche_open 1
#define bras_droite_open 2
#define bras_gauche_close 3
#define bras_droite_close 4
#define bras_free 5
#define bras_gauche_open_max 6
#define bras_droite_open_max 7


#define bras_open 8
#define bras_open_max 9
#define bras_close 10
#define bras_init 11

#define POS_GAUCHE_OPEN 2296 //3334 //il faudrait serrer un peu plus mais j'ai la flemme
#define POS_GAUCHE_OPEN_MAX 1920 //3904
#define POS_GAUCHE_CLOSE 3328 //2303
#define POS_DROITE_OPEN 1143 //2160 //il faudrait serrer un peu plus mais j'ai la flemme
#define POS_DROITE_OPEN_MAX 1666 //2600
#define POS_DROITE_CLOSE 144 //1140

#define GUIDE_STATE_WAIT_TX 100
#define GUIDE_STATE_POLL_REQUEST 110
#define GUIDE_STATE_POLL_WAIT 111
#define GUIDE_POS_TOL 30
#define GUIDE_MOVE_TIMEOUT_MS 2000
#define GUIDE_OUTPUT_MOVE_TIMEOUT 224

uint8_t guide_Loop (void);

u8 open_bras_gauche(void);
u8 close_bras_gauche(void);
u8 open_bras_droite(void);
u8 close_bras_droite(void);
u8 open_bras_droite_max(void);
u8 open_bras_gauche_max(void);
u8 free_guide(void);
u8 open_bras_max(void);
u8 open_bras(void);
u8 close_bras(void);
u8 init_guide(void);

extern u8 start_bras;

#endif // __GUIDE_H_
