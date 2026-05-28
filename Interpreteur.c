#include "main.h"


char Current_Cmd[MAX_CMD_LENGTH + 1];
u16 i_Current_Cmd = 0;
u16 i_Lecture_Current_Cmd = 0;



const Command Command_List[] = {


    { "TEST", Test_Func},
    { "PING", Ping_Func},
    { "TIMER", Timer_Func},
    { "DEBUGEN", Debug_En_Func},
    { "VITESSE", vitessse_RGB},

    // LEDS

    { "RGB", RGB_Func},

    // fichier log
    { "LOGRST", Flash_LogFile_RESET_Func},
    { "LOGINIT", InitLog_Func},
    { "LOGPRINT", printLog_Func},
    { "LOGADD", Log_Add_Func},
    { "LOGFILL", Log_Fill_Test_Func},

    // fichier setup
    { "SETUPERASE",  Setup_File_Erase_Func},
    { "SETUPREAD",   Setup_File_Start_Read_Func},
    { "SETUPRELOAD", Setup_File_Reload_Func},
    { "SETUPADD",    Setup_File_Add_Func},


    // AXSTS

    { "AXBAUD", AX_Uart_Half_Duplex_Set_Freq_Func},
    { "WR", Send_STS_Func},
    { "RD", Get_STS_Func},
    { "AXTESTID",Test_ID_AX_Func},
    { "STSTESTID",Test_ID_STS_Func},

    { "MOVEAX", Move_AX_Func},
    { "POSAX", Get_Pos_AX_Func},
    { "INITAX", init_AX_Func},
    { "SPEEDAX", speed_AX_Func},

    { "MOVESTS", Move_STS_Func},
    { "POSSTS", Get_Pos_STS_Func},
    { "INITSTS", init_STS_Func},
    { "SPEEDSTS", speed_STS_Func},

    { "ENPOMPE", All_Pompe_ON},
    { "ENVANNE", EN_Vanne_Func},

    //Bras
    {"INITBRAS", Bras_Demarrer_Init_All},
    { "GRAB", action_attraper_cagette},
    { "STORE", action_ranger_cagette},
    { "GRABANDSTORE", action_attraper_et_ranger_cagette},
    { "DROP", action_deposer_cagette},
    { "FOLD", action_rentrer_bras},
    {"FREEBRAS", Bras_Free_All},

    //bras curseur
    {"DOWN",action_descendre_bras_curseur},
    {"UP",action_montee_bras_curseur},
    {"INITCURSEUR",curseur_Init},
    {"FREECURSEUR",curseur_free},

    //Retourne cagete
    {"RETOURNEN", retourne_n},
    {"INITACTION", init_actionneurs_Func},

    //sequence barillet
    { "INITB", init_barillet},
    { "TOURNE", action_tourne_barillet},
    {"TOURNEABS" ,action_tourne_barillet_absolue},
    { "FREEB", free_barillet},

    { "RETOURNED",action_retourner_cagette_droite},
    { "RETOURNEG",action_retourner_cagette_gauche},

    { "I2CDISC", I2C_Discover_Cmd},


    //guides pour aligner les cagettes

    {"OPENBRASG",open_bras_gauche},
    {"CLOSEBRASG" , close_bras_gauche},
    {"OPENBRASD",open_bras_droite},
    {"OPENBRASGM",open_bras_gauche_max},
    {"OPENBRASDM",open_bras_droite_max},
    {"CLOSEBRASD", close_bras_droite},
    {"CLOSEBRAS" , close_bras},
    {"OPENBRAS",open_bras},
    {"OPENBRASM",open_bras_max},
    {"INITGUIDE",init_guide},
    {"FREEGUIDE", free_guide},

    //AU
    {"AU", trigger_Soft_AU},

    // manual flash
    { "FRD", FlashRd_Func},
    { "FSRD", FlashRawSector_Rd_Func},
    { "FWR", FlashWr_Func},
    { "FER", FlashEr_Func},

    // reprog
    { "STARTHEXDEC", HEX_Decodeur_Start_Func},
    { "STOPHEXDEC", HEX_Decodeur_Stop_Func},

    //{ "FERASE", HEX_Decodeur_Erase_Func},
    { "REPROG", HEX_Decodeur_Reprog_Func},


    { "REBOOT", Reboot_Func},
    { "REBOOTUSB", Reboot_USB_Func},

    { "VERSION", Print_Version_Func},

    { "PRINTCMD", Print_All_CMD_Cmd},
    { "HELP", Print_All_CMD_Cmd},






};

const u16 Command_List_Length = sizeof (Command_List) / sizeof (Command_List[0]);

void Interp(char c) {
    // fin de commande = entree
    if (c != '\n' && c != '\r') {
        if (c == 0x08) {    // code pour "effacer" dans tera term ?
            if (i_Current_Cmd) {
                i_Current_Cmd --;   // retour d'un cran en arriere
                Current_Cmd[i_Current_Cmd] = '\0';  // on efface le caractere
            }
        } else if (i_Current_Cmd < MAX_CMD_LENGTH) { //si on est toujours dans la commande, on ajoute
            Current_Cmd[i_Current_Cmd] = c;
            i_Current_Cmd++;
        } else if (i_Current_Cmd < (MAX_CMD_LENGTH + 1)) { // protection overshoot
            i_Current_Cmd++;
        }
    } else { // on a tappee entree, il faut trouver quelle fonction executer..
        Current_Cmd[i_Current_Cmd] = '\0'; // mise d'un fin de chaine a la fin
        if (i_Current_Cmd) {
            //printf("Got: %s\n", Current_Cmd);
            if (i_Current_Cmd == (MAX_CMD_LENGTH + 1)) {
                #ifndef WORLD_OF_SILENCE
                    printf("ERROR_Cmd_Length_Overshoot\r\n");
                #endif
            } else if (Current_Cmd[0] == ':') {
                HEX_decodeur(&Current_Cmd[1]);

            } else {

                // printf("Got %s\n", Current_Cmd);

                // on recherche combien de caracteres fait la commande en elle meme (sans les PARAM)
                u16 Len = 0;
                u16 Ind = 0;
                u8 Found = 0;
                while ( (Current_Cmd[Len] >= 'a' && Current_Cmd[Len] <= 'z') ||
                        (Current_Cmd[Len] >= 'A' && Current_Cmd[Len] <= 'Z') ||
                        (Current_Cmd[Len] >= '0' && Current_Cmd[Len] <= '9')    ) {
                    Current_Cmd[Len] = To_UpperCase(Current_Cmd[Len]);
                    Len++;
                }
                if (Len) {
                    Current_Cmd[Len] = '\0';
                    i_Lecture_Current_Cmd = Min_Ternaire(i_Current_Cmd, (Len+1));
                    while ((!Found) && (Ind < Command_List_Length)) {
                        if (strcmp(&Current_Cmd[0], Command_List[Ind].Name) == 0) {
                            Found = 1;
                        } else {
                            Ind++;
                        }
                    }
                }
                if (!Found) {
                    #ifndef WORLD_OF_SILENCE
                        printf("Cmd_Not_Found,");
                        printf(&Current_Cmd[0]);
                        printf("\n");
                    #endif
                } else {
                    u8 val8 = (*Command_List[Ind].Func)();
                    #ifndef WORLD_OF_SILENCE
                        if (val8)
                            printf("Cmd_Error %d\r\n", val8);
                    #endif
                }
            }
        }
        i_Current_Cmd = 0;
    }
}

char To_UpperCase(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    else
        return c;
}

u8 Get_Param_Float(float *retour) {
    float valf = 0;
    float div = 0;
    u8 Is_Neg = 0;
    u8 Result_Is_Error = 1;
    // tant qu'on est sur un caract�re, et qu'on a pas trouv� un chiffre
    while ((Current_Cmd[i_Lecture_Current_Cmd] != '\0') && (Current_Cmd[i_Lecture_Current_Cmd] != '-') &&
            !((Current_Cmd[i_Lecture_Current_Cmd] >= '0') && (Current_Cmd[i_Lecture_Current_Cmd] <= '9')))
    {
        i_Lecture_Current_Cmd++;
    }

    // if (Current_Cmd[i_Lecture_Current_Cmd] == '\0') // si on est au bout, on renvoie error...
    //    return 1;

    if ((Current_Cmd[i_Lecture_Current_Cmd] == '-')) {
        Is_Neg = 1;
        i_Lecture_Current_Cmd++;
    }

    while (((Current_Cmd[i_Lecture_Current_Cmd] >= '0') && (Current_Cmd[i_Lecture_Current_Cmd] <= '9')) || (Current_Cmd[i_Lecture_Current_Cmd] == '.')) {
        if (Current_Cmd[i_Lecture_Current_Cmd] != '.') {
            valf *= 10;
            valf += Current_Cmd[i_Lecture_Current_Cmd] - '0';
            div *= 10;
            Result_Is_Error = 0;
        } else {
            div = 1;
        }
        i_Lecture_Current_Cmd++;
    }

    if (!Result_Is_Error) {
        if (div == 0)
            div = 1;

        valf = valf / div;

        if (Is_Neg)
            *retour = -valf;
        else
            *retour = valf;
    }
    return Result_Is_Error;
}

u8 Get_Param_u32(u32 *retour) {
    u32 val = 0;
    u8 Result_Is_Error = 1;

    // tant qu'on est sur un caract�re, et qu'on a pas trouv� un chiffre

    while ((Current_Cmd[i_Lecture_Current_Cmd] != '\0') &&
            // (Current_Cmd[i_Lecture_Current_Cmd] != '-') &&
            !((Current_Cmd[i_Lecture_Current_Cmd] >= '0') && (Current_Cmd[i_Lecture_Current_Cmd] <= '9')))  {
        i_Lecture_Current_Cmd++;
    }

    while ((Current_Cmd[i_Lecture_Current_Cmd] >= '0') && (Current_Cmd[i_Lecture_Current_Cmd] <= '9')) {
        val *= 10;
        val += Current_Cmd[i_Lecture_Current_Cmd] - '0';
        Result_Is_Error = 0;
        i_Lecture_Current_Cmd++;
    }

    if (!Result_Is_Error) {
        *retour = val;
    }
    return Result_Is_Error;
}

u8 Get_Param_x32(u32 *retour) {
    u32 val = 0;
    //u8 Is_Neg = 0;
    u8 Result_Is_Error = 1;

    // tant qu'on est sur un caract�re, et qu'on a pas trouv� un chiffre

    while ((Current_Cmd[i_Lecture_Current_Cmd] != '\0') &&
            //(Current_Cmd[i_Lecture_Current_Cmd] != '-') &&
            !((Current_Cmd[i_Lecture_Current_Cmd] >= '0') && (Current_Cmd[i_Lecture_Current_Cmd] <= '9')) &&
            !((To_UpperCase(Current_Cmd[i_Lecture_Current_Cmd]) >= 'A') && (To_UpperCase(Current_Cmd[i_Lecture_Current_Cmd]) <= 'F'))      )
    {
        i_Lecture_Current_Cmd++;
    }

/*
    if ((Current_Cmd[i_Lecture_Current_Cmd] == '-')) {
        Is_Neg = 1;
        i_Lecture_Current_Cmd++;
    }
 */

    while ( ((Current_Cmd[i_Lecture_Current_Cmd] >= '0') && (Current_Cmd[i_Lecture_Current_Cmd] <= '9')) ||
            ((To_UpperCase(Current_Cmd[i_Lecture_Current_Cmd]) >= 'A') && (To_UpperCase(Current_Cmd[i_Lecture_Current_Cmd]) <= 'F'))      )
    {
        val *= 16;
        if ((Current_Cmd[i_Lecture_Current_Cmd] >= '0') && (Current_Cmd[i_Lecture_Current_Cmd] <= '9'))
            val += Current_Cmd[i_Lecture_Current_Cmd] - '0';
        else if ((To_UpperCase(Current_Cmd[i_Lecture_Current_Cmd]) >= 'A') && (To_UpperCase(Current_Cmd[i_Lecture_Current_Cmd]) <= 'F'))
            val += 10 + To_UpperCase(Current_Cmd[i_Lecture_Current_Cmd]) - 'A';

        Result_Is_Error = 0;
        i_Lecture_Current_Cmd++;
    }

    if (!Result_Is_Error) {
        /*if (Is_Neg)
            *retour = - val;
        else*/

        *retour = val;
    }
    return Result_Is_Error;
}

u8 Get_Param_End_String(char Dest_Str[], u8 Max_Len)
{
    u8 Len = 0;

    u16 i_Start = 0, i_End = 0;

    while (Current_Cmd[i_Lecture_Current_Cmd] == ' ') {
        i_Lecture_Current_Cmd ++;   // recherche du premier caractere pas espace
    }
    i_Start = i_Lecture_Current_Cmd;    //

    while (Current_Cmd[i_Lecture_Current_Cmd] != '\0') {
        i_Lecture_Current_Cmd ++;   // go until end
    }
    i_End = i_Lecture_Current_Cmd;  // i_End = 2em "

    if (i_End > i_Start) {  // si on a bien trouve des choses
        if (((i_End - i_Start) < (Max_Len - 1)) && (Max_Len != 0)) {    // et que ca rentre dans le buffer
            u16 i = i_Start;
            while (i != i_End) {
                Dest_Str[Len] = Current_Cmd[i];
                Len ++;
                i ++;
            }
            Dest_Str[Len] = '\0';
        }
    }
    return Len;
}

u16 i_Print_All_Cmd = 0;

u8 Print_All_CMD_Cmd (void) {
    i_Print_All_Cmd = 1;
    return 0;
}

void Print_All_Cmd_Loop (void) {
    static uint32_t Last_Time_Print_All_Cmd = 0;
    if (i_Print_All_Cmd) {
        if ((Timer_ms1 - Last_Time_Print_All_Cmd) > 10) {
            Last_Time_Print_All_Cmd = Timer_ms1;
            if ((i_Print_All_Cmd - 1) < Command_List_Length) {
                printf("%s\n", Command_List[i_Print_All_Cmd - 1].Name);
                i_Print_All_Cmd ++;
            } else {
                i_Print_All_Cmd = 0;
            }
        }
    }
}
