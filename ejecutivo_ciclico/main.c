#include <stdio.h>
#include "luz.h"
#include "codigo.h"
#include "alarma.h"
#include "timeval_helper.h"

int p_alarma;
int boton;
int presencia;
struct timeval espera_deadline1;

void scan_keyboard () {
  char buf[256];
  struct timeval timeout = {0,0};
  fd_set rdset;
  FD_ZERO (&rdset); 
  FD_SET (0, &rdset);
  if (select (1, &rdset, NULL, NULL, &timeout) > 0) {
    fgets (buf, 256, stdin);
    if ((buf[0])!='\n'){
      switch (buf[0]) {
        case 'a': 
          p_alarma = 1;
          static struct timeval t_espera = { 1, 0 };
		  gettimeofday (&espera_deadline1, NULL);
		  timeval_add (&espera_deadline1, &espera_deadline1, &t_espera); 
          printf("p_alarma = 1\n"); 
          break;
        case 'b': 
          boton = 1; 
          printf("boton luz = 1\n");  
          break;
        case 'p': 
          presencia = 1; 
          printf("presencia = 1\n");  
          break;
      }
    }
  }
}

int main (void)
{
	static struct timeval period = { 1, 0 };
	struct timeval next;
	fsm_t* luz = fsm_new_luz ();
	fsm_t* codigo = fsm_new_codigo ();
	fsm_t* alarma = fsm_new_alarma ();

	gettimeofday (&next, NULL);

	printf("\n");
	printf("\t****************************************\n\n");
	printf("\t\tTeclas de activación:\n\n");
	printf("\tPresencia: p\n");
	printf("\tBotón luz: b\n");
	printf("\tPulsador código alarma: a\n");
	printf("\tContraseña de la alarma: 2 3 2\n\n");
	printf("\t****************************************\n\n");

	while (1) {
		fsm_fire (luz);
		fsm_fire (codigo);
		fsm_fire (alarma);
		timeval_add (&next, &next, &period);
		scan_keyboard ();
	}
}

