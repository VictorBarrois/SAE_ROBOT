#include "fonction.h"

#define ARRIVER2 680 //check
#define ARRIVER1 245 //check
#define DEPART2 310 //check
#define DEPART1 300 //check

Ticker tick;

int main() {
  init();
  uint8_t etat = 0;
  volume(12);
  stop();
  while (1) {
    jack = JACK.read();
    fdc = FDC.read();
    captav = Capt_avant.read();
    switch(etatw){
        case 0 : if(fdc == 1 && jack == 1) etatw = 4; break;
                 
        case 1 : 
                 
                 if(fdc == 0 && jack == 1) etatw = 99; 
                 if(cptm == 4 )
                 {
                     
                     vitesse(0.0,0.0);
                     wait(1);
                     tourne_ligne(-60);
                     cptm++;
                     wait(1);
                 }
                 if(cptm == 6)
                 {
                     vitesse(0.0,0.0);
                     wait(1);
                     tourne_ligne(-60);
                     cptm++;
                     wait(1);
                 }
                  if(cptd == 6)
                 {
                     vitesse(0.0,0.0);
                     wait(1);
                     tourne_ligne(60);
                     cptd++;
                     wait(1);
                 }
                  if(cptm == 9)
                 {
                     vitesse(0.0,0.0);
                     wait(1);
                     tourne_ligne(-70);
                     cptm++;
                     wait(1);
                 }
                  if(cptm == 12)
                 {
                     vitesse(0.0,0.0);
                     wait(1);
                     tourne_ligne(-60);
                     cptm++;
                     wait(1);
                 }
                  if(cptm == 14)
                 {
                     vitesse(0.0,0.0);
                     wait(1);
                     tourne_ligne(-60);
                     cptm++;
                     wait(1);
                 }
        break;
                 
                  
        
        case 4 :  if(fdc == 0) {etatw = 1; vitesse(0.0,0.0); wait(1);
            tourne_ligne(120); wait(1);} break;     
                 
        
    }
    switch(etatw){
        case 0 : vitesse(0.0,0.0); break;
        case 1 : suivi_de_ligne(); comptage(); break;
        case 99 : vitesse(0.0 , 0.0); break;
        case 4 : suivi_de_ligne(); break;
    }
    
    wait_us(50);
  }
}
