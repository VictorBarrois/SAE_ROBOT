#include "Grove_LCD_RGB_Backlight.h"
#include "SRF05.h"
#include "mbed.h"
#include <cstdint>

extern Grove_LCD_RGB_Backlight LCD;
extern Serial pc; //MP3
extern Serial device; //MP3
extern AnalogIn capt_1;
extern AnalogIn capt_2;
extern AnalogIn capt_3;
extern AnalogIn capt_4;
extern DigitalIn JACK; // enfoncé à 0;
extern DigitalIn FDC;  // enfoncé à 0;
extern InterruptIn am1;   // encodeur mot_G
extern InterruptIn am2;  // encodeur mot_D
extern DigitalIn bm1;     // encodeur mot_G
extern DigitalIn bm2;    // encodeur mot_D
extern PwmOut MOT_D;      // puissance mot_D
extern PwmOut MOT_G;      // puissance mot_G
extern DigitalOut SMOT_D; // sens mot_D
extern DigitalOut SMOT_G; // sens mot_G
extern SRF05 Capt_avant;

extern int16_t cpt1 , cpt2 , cptd, cptg ,cptm, t_flag ;
extern int16_t IR, jack, fdc;
extern float capt1, capt1_av, capt4_av, capt2, capt3, capt4, delta;
extern char tab[20];
extern float cmdD, cmdG;
extern int8_t etat,etatw;
extern float derivative, prev_delta;
extern int16_t cpt1av;
extern float captav, captg, captavav, captgav, dt;
extern unsigned int VMOY;
extern Timer t2;

void evite_obstacle(); 
void comptage();
void incrementation1(void);
void incrementation2(void);
void vitesse(float a, float b);
void tourne(int16_t angle);
void avance(int16_t dist);
void toutdroit();
void suivi_de_ligne();
void playpause();
void volume(int);
void previous();
void next();
void stop();
void fich1();
void fich2();
void init();
void priorite();
void tourne_ligne(int16_t);
