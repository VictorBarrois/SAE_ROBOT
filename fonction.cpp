#include "fonction.h"
#include "SRF05.h"
#include "mbed.h"

#define KD 1.5 // Coefficient pour le terme dérivé
#define KPD 72
#define KPG 72
unsigned int VMOY = 40;
Grove_LCD_RGB_Backlight LCD(D14, D15);
Serial pc(USBTX, USBRX); //MP3
Serial device(PTE22, PTE23); //MP3
SRF05 Capt_avant(A5, D4);
SRF05 Capt_gauche(PTE30, D5);
AnalogIn capt_1(A1);
AnalogIn capt_2(A2);
AnalogIn capt_3(A3);
AnalogIn capt_4(A4);
DigitalIn JACK(PTE20); // enfoncé à 0;
DigitalIn FDC(PTE21);  // enfoncé à 0;
InterruptIn am1(D2);   // encodeur mot_G
InterruptIn am2(D11);  // encodeur mot_D
DigitalIn bm1(D3);     // encodeur mot_G
DigitalIn bm2(D10);    // encodeur mot_D
PwmOut MOT_D(D8);      // puissance mot_D
PwmOut MOT_G(D6);      // puissance mot_G
DigitalOut SMOT_D(D9); // sens mot_D
DigitalOut SMOT_G(D7); // sens mot_G

int16_t cpt1 = 0, cpt2 = 0, cptd = 0,cptg = 0,cptm = 0, t_flag = 0;
int16_t IR, jack, fdc;
float capt1, capt1_av, capt4_av, capt2, capt3, capt4, delta;
char tab[20];
float cmdD, cmdG;
int8_t etat = 0, etatprio = 0,etatw = 0;
float derivative, prev_delta;
int16_t cpt1av;
float captav, captg, captavav, captgav, dt;

Timer t;
Timer t2;

void init() {
    JACK.mode(PullUp);
    FDC.mode(PullUp);
    MOT_G.period_us(100);
    MOT_D.period_us(100);
    am1.rise(&incrementation1);
    am2.rise(&incrementation2);
}

void incrementation1(void) {
    if (bm1.read() == 0)
        cpt1++;
    else
        cpt1--;
}

void incrementation2(void) {
    if (bm2.read() == 1)
        cpt2++;
    else
        cpt2--;
}

void vitesse(float a, float b) {
    if (a > 100)
        a = 100;
    if (a < -100)
        a = -100;

  // batterie = ((bat.read() * 3.3) * (59.0 / 12.0)) + 0.6;
    if (a >= 0) {
        SMOT_G.write(0);
        MOT_G.pulsewidth_us(a);
    } else {
        SMOT_G.write(1);
        MOT_G.pulsewidth_us(100 + a);
    }
    if (b >= 0) {
        SMOT_D.write(0);
        MOT_D.pulsewidth_us(b);
    } else {
        SMOT_D.write(1);
        MOT_D.pulsewidth_us(100 + b);
    }
}

void tourne(int16_t angle) {
    cpt1 = 0;
    cpt2 = 0;
    angle = (angle * 435 / 360) / 2;
    if(angle > 0)
    {
        do {
            if (cpt1 > (-1 * cpt2))
                vitesse(35.0, -40.0);
            else
                vitesse(35.0, -30.0);
            wait_us(150.0);
        } while (cpt1 < angle);
    }
    else {
        do {
            if((-1*cpt1) > cpt2 )
                vitesse(-35.0,40.0);
            else
                vitesse(-35.0,30.0);
            wait_us(150.0);
        }while(cpt1 > angle);
    }
    vitesse(0.0, 0.0);
}

void tourne_ligne(int16_t angle) {
    cpt1 = 0;
    cpt2 = 0;
    angle = (angle * 435 / 360) / 2;
    if(angle > 0)
    {
        do {
            if (cpt1 > (-1 * cpt2))
                vitesse(35.0, -40.0);
            else
                vitesse(35.0, -30.0);
            wait_us(150.0);
        } while (cpt1 < angle);
        do{
            if (cpt1 > (-1 * cpt2))
                vitesse(35.0, -40.0);
            else
                vitesse(35.0, -30.0);
            wait_us(150.0);
        }while(capt_3.read()<0.6);
    }
    else {
        do {
            if((-1*cpt1) > cpt2 )
                vitesse(-35.0,40.0);
            else
                vitesse(-35.0,30.0);
            wait_us(150.0);
        }while(cpt1 > angle);
        do{
            if (cpt1 > (-1 * cpt2))
                vitesse(-35.0, 40.0);
            else
                vitesse(-35.0, 30.0);
            wait_us(150.0);
        }while(capt_2.read()<0.6);
    }
    vitesse(0.0, 0.0);
}

void avance(int16_t dist) {
    cpt1 = 0;
    cpt2 = 0;
    if(dist >0)
    {
        do {
            if (cpt1 > cpt2) {
                vitesse(35.0, 40.0);
            } else if (cpt1 <= cpt2) {
                vitesse(35.0, 30.0);
            }
            wait_us(150);
        } while (cpt1 < dist);
    }
    else 
    {
        do{
            if(cpt1 > cpt2)
            {
                vitesse(-35.0,-30.0);
            }
            else if(cpt1<= cpt2)
            {
                vitesse(-35.0,-40.0);
            }
            wait_us(150);
        }while(cpt1 > dist);
    }
    vitesse(0.0, 0.0);
}

void toutdroit() {
    if (cpt1 > cpt2) {
        vitesse(35.0, 40.0);
    } else if (cpt1 <= cpt2) {
        vitesse(35.0, 30.0);
    }
    wait_us(50);
}

void suivi_de_ligne() {
    capt1 = capt_1.read();
    capt2 = capt_2.read();
    capt3 = capt_3.read();
    capt4 = capt_4.read();
    if (t_flag == 0) {
        t_flag = 1;
        t.start();
    }
    delta = capt2 - capt3;
    dt = t.read();

    derivative = (delta - prev_delta) / dt;
    t.reset();
    prev_delta = delta;
    cmdD = VMOY + (KPD * delta) + (KD * derivative);
    cmdG = VMOY - (KPG * delta) - (KD * derivative);

    vitesse(cmdG, cmdD);
}

void evite_obstacle() {
    captav = Capt_avant.read();
    captg = Capt_gauche.read();
  capt2 = capt_2.read();
  capt3 = capt_3.read();
  switch (etat) {
  case 0:
    if (captav <= 15 && captavav > 15) {
      vitesse(0.0, 0.0);
      wait(1);
      tourne(90);
      wait(1);
      cpt1 = 0;
      cpt2 = 0;
      etat = 1;
    }
    break;
  case 1:
    if (captg >= 40 && captgav < 40) {
      cpt1av = cpt1;
      avance(100);
      wait(1);
      tourne(300);
      wait(1);
      avance(200);
      wait(1);
      tourne(300);
      wait(1);
      avance(cpt1av + 100);
      wait(1);
      tourne(90);
      wait(1);
      cpt1 = 0;
      cpt2 = 0;
      etat = 0;
    }
    break;
  }
  switch (etat) {

  case 0:
    toutdroit();
    break;

  case 1:
    toutdroit();
    break;
  }
}

void comptage() {
    capt4 = capt_4.read();
    capt1 = capt_1.read();
  
    switch(etat)
    {

        
    case 0: if(capt1 > 0.6 && capt1_av < 0.6) etat = 1;
            else if(capt4 > 0.6 && capt4_av < 0.6) etat = 2; break;
      
    case 1:
        if(capt4 > 0.6)
        {
            cptm++;
            LCD.setRGB(255, 255, 255);
            LCD.locate(0, 0);
            sprintf(tab,"G = %d --- D = %d\n\r", cptg,cptd);
            LCD.print(tab);
            LCD.locate(0, 1);
            sprintf(tab, "M = %d\n\r", cptm);
            LCD.print(tab);
            etat = 0; 
        } 
        else if(capt1 < 0.6)
        {

            cptg++;
            LCD.setRGB(255, 255, 255);
            LCD.locate(0, 0);
            sprintf(tab, "G = %d --- D = %d\n\r", cptg,cptd);
            LCD.print(tab);
            LCD.locate(0, 1);
            sprintf(tab, "M = %d\n\r", cptm);
            LCD.print(tab);
            etat = 0;
        } 
        
        break;

    case 2: 
        if(capt1 > 0.6)
        { 
            cptm++;
            LCD.setRGB(255, 255, 255);
            LCD.locate(0, 0);
            sprintf(tab, "G = %d --- D = %d\n\r", cptg,cptd);
            LCD.print(tab);
            LCD.locate(0, 1);
            sprintf(tab, "M = %d\n\r", cptm);
            LCD.print(tab);
            etat = 0;
        }
        else if(capt4 < 0.6)
        {
            cptd++;
            LCD.setRGB(255, 255, 255);
            LCD.locate(0, 0);
            sprintf(tab, "G = %d --- D = %d\n\r", cptg,cptd);
            LCD.print(tab);
            LCD.locate(0, 1);
            sprintf(tab, "M = %d\n\r", cptm);
            LCD.print(tab);
            etat = 0;
        } break;

    }
    
    
  capt4_av = capt4;
  capt1_av = capt1;
}

void playpause() {
  char tab1[5] = {0x7E, 0x03, 0xAA, 0, 0xEF};
  tab1[3] = tab1[1] + tab1[2];
  for (int i = 0; i < 5; i++) {
    device.putc(tab1[i]);
  }
}
void volume(int parametre) {
  char tab2[6] = {0x7E, 0x04, 0xAE, parametre, 0, 0xEF};
  tab2[4] = tab2[1] + tab2[2] + tab2[3];
  for (int i = 0; i < 6; i++) {
    device.putc(tab2[i]);
  }
}

void next() {
  char tab3[5] = {0x7E, 0x03, 0xAC, 0, 0xEF};
  tab3[3] = tab3[1] + tab3[2];
  for (int i = 0; i < 5; i++) {
    device.putc(tab3[i]);
  }
}

void previous() {
  char tab4[5] = {0x7E, 0x03, 0xAD, 0, 0xEF};
  tab4[3] = tab4[1] + tab4[2];
  for (int i = 0; i < 5; i++) {
    device.putc(tab4[i]);
  }
}
void stop() {
  char tab5[5] = {0x7E, 0x03, 0xAB, 0, 0xEF};
  tab5[3] = tab5[1] + tab5[2];
  for (int i = 0; i < 5; i++) {
    device.putc(tab5[i]);
  }
}
void fich1() {
  char tab6[9] = {0x7E, 0x07, 0xA3, 'c', 'a', 'r', 's', 0, 0xEF};
  tab6[7] = tab6[1] + tab6[2] + tab6[3] + tab6[4] + tab6[5] + tab6[6];
  for (int i = 0; i < 9; i++) {
    device.putc(tab6[i]);
  }
}
void fich2() {
  char tab7[8] = {0x7E, 0x06, 0xA3, 'w', 'i', 'n', 0, 0xEF};
  tab7[6] = tab7[1] + tab7[2] + tab7[3] + tab7[4] + tab7[5];
  for (int i = 0; i < 8; i++) {
    device.putc(tab7[i]);
  }
}


