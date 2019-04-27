#include "luz.h"
#include "timeval_helper.h"
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include "codigo.h"
#include "alarma.h"

extern int acierto; // variable compartida
extern int p_alarma; // variable compartida
extern int presencia; // encender luz y alarma
//static struct timeval espera_deadline;

static int hay_presencia (fsm_t* this) { return presencia; }
static int check_code (fsm_t* this) { return acierto; }

static void enciende (fsm_t* this) { 
	//static struct timeval t_espera = { 1, 0 };
	//gettimeofday (&espera_deadline, NULL);
	//timeval_add (&espera_deadline, &espera_deadline, &t_espera);
	printf ("******* Alarma ON *******\n"); 
	acierto = 0; 
	presencia = 0; 
}

static void mantiene_encendida (fsm_t* this) { 
	//static struct timeval t_espera = { 1, 0 };
	//gettimeofday (&espera_deadline, NULL);
	//timeval_add (&espera_deadline, &espera_deadline, &t_espera);
	printf ("******* Alarma sigue ON *******\n"); 
	acierto = 0; 
	presencia = 0;
}

static void apaga (fsm_t* this) { printf ("******* Alarma OFF *******\n"); acierto = 0; }

fsm_t* fsm_new_alarma ()
{
	static fsm_trans_t tt[] = {
		{ 0, hay_presencia, 1, enciende },
		{ 0, check_code, 1, enciende },
		{ 1, hay_presencia, 1, mantiene_encendida },
		{ 1, check_code, 0, apaga },
		{ -1, NULL, -1, NULL }
	};
	return fsm_new (tt);
}

