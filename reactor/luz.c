#include "luz.h"
#include "timeval_helper.h"
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include "codigo.h"

extern int p_alarma; // boton codigo alarma
extern int boton; // encender y apagar luz
extern int presencia; // encender luz y alarma
static struct timeval espera_deadline;

static int hay_presencia (fsm_t* this) { return presencia; }
static int boton_pulsado (fsm_t* this) { return boton; }

static int timeout (fsm_t* this) { 
	struct timeval now;
	gettimeofday (&now, NULL);
	return timeval_less (&espera_deadline, &now); 
}

static void enciende (fsm_t* this) { 
	static struct timeval t_espera = { 60, 0 };
	gettimeofday (&espera_deadline, NULL);
	timeval_add (&espera_deadline, &espera_deadline, &t_espera);
	printf ("******* Luz ON *******\n"); 
	boton = 0; 
}

static void mantiene_encendida (fsm_t* this) { 
	static struct timeval t_espera = { 60, 0 };
	gettimeofday (&espera_deadline, NULL);
	timeval_add (&espera_deadline, &espera_deadline, &t_espera);
	printf ("******* Luz sigue ON *******\n"); 
	boton = 0; 
}

static void apaga (fsm_t* this) {
	printf ("******* Luz OFF *******\n");
    boton = 0;
    presencia = 0;	
}

fsm_t* fsm_new_luz ()
{
	static fsm_trans_t tt[] = {
		{ 0, hay_presencia, 1, enciende },
		{ 0, boton_pulsado, 1, enciende },
		{ 1, hay_presencia, 1, mantiene_encendida },
		{ 1, boton_pulsado, 0, apaga },
		{ 1, timeout, 0, apaga },
		{ -1, NULL, -1, NULL }
	};
	return fsm_new (tt);
}

