#include "codigo.h"
#include "timeval_helper.h"
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>

enum semp_state {
  idle,
  p1,
  p2,
  p3,
};

static int A = 2;
static int B = 3;
static int C = 2;
static int C1 = 0;
static int C2 = 0;
static int C3 = 0;
int acierto = 0; // variable compartida
extern int p_alarma; // variable compartida
static struct timeval espera_deadline;
extern struct timeval espera_deadline1;

static int trans (fsm_t* this) {
	struct timeval now;
	gettimeofday (&now, NULL);
	int check = timeval_less (&espera_deadline1, &now);
	if((p_alarma == 1) && (check == 0)) {
		return 1;
	}
	else{
		return 0;
	}
}

static int timeout (fsm_t* this) { 
	struct timeval now;
	gettimeofday (&now, NULL);
	int check = timeval_less (&espera_deadline, &now);
	if (check == 1) {
		printf("Se ha pasado el tiempo, introduzca el siguiente dígito\n");
	}
	return timeval_less (&espera_deadline, &now); 
}

static int code_check1 (fsm_t* this) {
	struct timeval now;
	gettimeofday (&now, NULL);
	int check = timeval_less (&espera_deadline, &now);
	if (A == C1 && B == C2 && C == C3 && check == 1) {
		printf("Se ha pasado el tiempo, comprobando contaseña...\n");
		return 1;
	}
	else{
		return 0;
	}
}

static int code_check2 (fsm_t* this) {
	struct timeval now;
	gettimeofday (&now, NULL);
	int check = timeval_less (&espera_deadline, &now);
	if ((A != C1 || B != C2 || C != C3) && check == 1) {
		printf("Se ha pasado el tiempo, comprobando contaseña...\n");
		return 1;
	}
	else{
		return 0;
	}
}

static void actualiza1 (fsm_t* this) { 
	C1++;
	if (C1 == 10) {
		C1 = 0;
	}
	printf ("Dígito 1 = %d\n", C1);
	static struct timeval t_espera = { 3, 0 };
	gettimeofday (&espera_deadline, NULL);
	timeval_add (&espera_deadline, &espera_deadline, &t_espera); 
	p_alarma = 0;
}

static void actualiza2 (fsm_t* this) { 
	C2++;
	if (C2 == 10) {
		C2 = 0;
	}
	printf ("Dígito 2 = %d\n", C2);
	static struct timeval t_espera = { 3, 0 };
	gettimeofday (&espera_deadline, NULL);
	timeval_add (&espera_deadline, &espera_deadline, &t_espera);
	p_alarma = 0;
}

static void actualiza3 (fsm_t* this) { 
	C3++;
	if (C3 == 10) {
		C3 = 0;
	}
	printf ("Dígito 3 = %d\n", C3);
	static struct timeval t_espera = { 3, 0 };
	gettimeofday (&espera_deadline, NULL);
	timeval_add (&espera_deadline, &espera_deadline, &t_espera);
	p_alarma = 0;
}

static void actualiza_tiempo (fsm_t* this) { 
	static struct timeval t_espera = { 3, 0 };
	gettimeofday (&espera_deadline, NULL);
	timeval_add (&espera_deadline, &espera_deadline, &t_espera);
	p_alarma = 0; 
} 

static void actualiza_acierto (fsm_t* this) { 
	C1++;
	printf ("Contraseña correcta\n");
	acierto = 1;
	C1 = 0;
	C2 = 0;
	C3 = 0;
}

static void actualiza_fallo (fsm_t* this) { 
	C1++;
	printf ("Contraseña incorrecta, vuelva a introducirla\n");
	acierto = 0;
	C1 = 0;
	C2 = 0;
	C3 = 0;
}

fsm_t* fsm_new_codigo ()
{
	static struct timeval t_espera = { 3, 0 };
	gettimeofday (&espera_deadline, NULL);
	timeval_add (&espera_deadline, &espera_deadline, &t_espera);

	static fsm_trans_t tt[] = {
		{ idle, trans, p1, actualiza1 },
		{ p1, trans, p1, actualiza1 },
		{ p1, timeout, p2, actualiza_tiempo },
		{ p2, trans, p2, actualiza2 },
		{ p2, timeout, p3, actualiza_tiempo },
		{ p3, trans, p3, actualiza3 },
		{ p3, code_check1, idle, actualiza_acierto },
		{ p3, code_check2, idle, actualiza_fallo },
		{ -1, NULL, -1, NULL }
	};
	return fsm_new (tt);
}

