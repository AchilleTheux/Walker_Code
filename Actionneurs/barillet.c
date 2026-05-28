#include "main.h"
#include "barillet.h"
// /!\ barillet à modifier pour fonctionné

u8 IDS_FEETECH[4] = {1, 2, 21, 22};
u8 DONE_FEETECH[4]={1,1,1,1};

#define SECTION_ROTATION (4096*1.2)
#define VITESSE_BARILLET 1023

#define VITESSE_BARILLET_RALENTIT 300
#define VITESSE_BARILLET_RALENTIT_INIT 100
uint16_t sens = 0; // sens = 0: horaire, = 1024: anti-horaire
u32 pos_barillet = 0;
u32 pos_barillet_future = 0;
u32 delta = 0;
uint8_t start_barillet = 0;
uint8_t etat_barillet = 0;
uint8_t etat_init_barillet = 0;
uint8_t done_barillet = 0;
uint8_t ralentit = 0;

uint8_t vrai_done_barillet = 0;
static const char *barillet_pending_command = "pepino";

u32 dist_total;
u32 nb_demi_section;
u32 last_posf;
u32 posf;
u32 last_capteur_val;
u32 capteur_val;
u32 dist;


uint32_t time1 = 0;
uint32_t time2 = 0;


static uint8_t barillet_error_code_from_sts_status(uint8_t sts_status) {
    if (sts_status == STS_STATUS_OK) return 0;
    return sts_status+10;
}

static void print_barillet_status(const char *command_name, uint8_t error_code) {
    printf("barillet %s %u\n", command_name, error_code);
}

uint8_t init_barillet(void) {
    for (int i=0; i<4; ++i) {
        Put_Feetech(IDS_FEETECH[i], STS_MODE, 2, NULL);
        Put_Feetech(IDS_FEETECH[i], STS_TORQUE_ENABLE, 1, NULL);
        Put_Feetech(IDS_FEETECH[i], STS_GOAL_TIME, 0, NULL);
    }
    gpio_init(CAPTEUR_PIN);
    gpio_set_dir(CAPTEUR_PIN, GPIO_IN);

    pos_barillet_future = pos_barillet;
    etat_init_barillet = 2;

    return 0;
}

uint8_t free_barillet(void) {
    for (int i=0; i<4; ++i) Put_Feetech(IDS_FEETECH[i], STS_TORQUE_ENABLE, 0, NULL);
    barillet_pending_command = "freeb";
    print_barillet_status("freeb", 0);
    return 0;
}

void action_tourne_barillet_loop(void) {
    switch (etat_barillet) {
        case 0:
            if (start_barillet == 1) {
                start_barillet = 0;
                dist = 0;
                ralentit = 0;
                vrai_done_barillet = 0;
                delta = 0;
                ++etat_barillet;
            } else if (etat_init_barillet) {
                --etat_init_barillet;
                if (!etat_init_barillet) barillet_pending_command = "initb";
                start_barillet = 1;
                sens = 1024 * etat_init_barillet;
                nb_demi_section = 1;
                dist_total = SECTION_ROTATION; // réalise un changement de section pour se centrer
            }
        break;
        case 1: //get position initiale
            done_barillet=0;
            Get_Feetech(IDS_FEETECH[0], STS_PRESENT_POSITION, &last_posf, &done_barillet);
            ++etat_barillet;
        break;
        case 2: //lancement du mouvement
            if (done_barillet) {
                done_barillet = 0;
                for (int i=0; i<4; ++i) Put_Feetech(IDS_FEETECH[i], STS_GOAL_TIME, VITESSE_BARILLET + sens, i < 3 ? NULL : &done_barillet);
                ++etat_barillet;
            }
        break;
        case 3: //get current position and end the movement
            if (done_barillet) {
                done_barillet = 0;
                Get_Feetech(IDS_FEETECH[0], STS_PRESENT_POSITION, &posf, &done_barillet);
                ++etat_barillet;
            }
            time1 = Timer_us1;
            //printf("loop time %d, capteur: %d, dist: %d\n", time1-time2, capteur_val, dist);
            time2 = time1;
        break;
        case 4:
            if (done_barillet) {
                delta = abs(posf - last_posf);
                if (delta > 2048) delta = abs(4096 - delta); //Comme ca c'est mieux
                dist += delta;
                last_posf = posf;
                if (!ralentit && dist + SECTION_ROTATION / 5 >= dist_total) ++etat_barillet; // ralentit avant la fin
                else if (dist < dist_total) --etat_barillet;
                else etat_barillet = 6;
            }

            capteur_val = gpio_get(CAPTEUR_PIN);
            if (capteur_val != last_capteur_val) { // front montant ou descendant du capteur
                if (dist > SECTION_ROTATION / 3 && !capteur_val) { // presence du marqueur // front descendant
                    --nb_demi_section;
                    pos_barillet = (pos_barillet + (sens ? -1 : 1)) % 6; // numérote en sens anti-horaire
                    if (!nb_demi_section && !ralentit) etat_barillet = 5;
                }
            }
            last_capteur_val = capteur_val;

            if (!nb_demi_section && capteur_val) etat_barillet = 6; //on a vu la fente
        break;
        case 5: // front descendant du capteur ou ditance restance < à 1/5, on ralentit pour voir la fente
            for (int i=0; i<4; ++i) Put_Feetech(IDS_FEETECH[i], STS_GOAL_TIME, VITESSE_BARILLET_RALENTIT + sens, NULL);
            ralentit = 1;
            etat_barillet = 4;
        break;
        case 6:// on s'arrete car on a fait dist_total ou qu'on a vu la fente
            done_barillet = 0;
            if (!etat_init_barillet) vrai_done_barillet = 1;
            for (int i=0; i<4; ++i) Put_Feetech(IDS_FEETECH[i], STS_GOAL_TIME, 0, i < 3 ? NULL : &done_barillet);
            ++etat_barillet;
        break;
        case 7:// sending status
            if (done_barillet) {
                uint8_t error_code = barillet_error_code_from_sts_status(done_barillet);
                if (!error_code) {
                    pos_barillet = pos_barillet_future;
                }
                done_barillet = 0;
                print_barillet_status(barillet_pending_command, error_code);
                barillet_pending_command = "vache?";
                etat_barillet = 0;
            }
        break;
    }
}


uint8_t action_tourne_barillet(void) {
    if (Get_Param_u32(&nb_demi_section)) {
        print_barillet_status("tourne", PARAM_ERROR_CODE);
        return PARAM_ERROR_CODE;
    }
    nb_demi_section %= 6;
    if (!nb_demi_section) {
        print_barillet_status("tourne", 0);
        return 0;
    }
    if (nb_demi_section > 3) {
        nb_demi_section = 6 - nb_demi_section;
        sens = 1024;
        pos_barillet_future -= nb_demi_section;
    } else {
        sens = 0;
        pos_barillet_future += nb_demi_section;
    }
    dist_total = nb_demi_section * SECTION_ROTATION;
    nb_demi_section = 2 * nb_demi_section - 1;
    barillet_pending_command = "tourne";
    start_barillet = 1;
    return 0;
}

uint8_t action_tourne_barillet_absolue(void) {
    if (Get_Param_u32(&pos_barillet_future)) {
        print_barillet_status("tourneabs", PARAM_ERROR_CODE);
        return PARAM_ERROR_CODE;
    }

    pos_barillet_future %= 6;

    if (pos_barillet_future == pos_barillet) {
        print_barillet_status("tourneabs", 0);
        return 0;
    }

    u32 distance_1 = ((pos_barillet_future - pos_barillet)+6) % 6;
    u32 distance_2 = ((pos_barillet - pos_barillet_future)+6) % 6;

    if (distance_1 < distance_2) {
     nb_demi_section = distance_1;
     sens = 0;
    } else{
     nb_demi_section = distance_2;
     sens = 1024;
    }

    dist_total = nb_demi_section * SECTION_ROTATION;
    nb_demi_section = 2 * nb_demi_section - 1;

    barillet_pending_command = "tourneabs";
    start_barillet = 1;

    return 0;

}
