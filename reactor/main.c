#include <stdio.h>
#include "reactor.h"
#include "luz.h"
#include "alarma.h"
#include "codigo.h"
#include "timeval_helper.h"

fsm_t* luz;
fsm_t* alarma;
fsm_t* codigo;

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
          static struct timeval t_espera = { 3, 0 };
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

static
void
task_luz (struct event_handler_t* this)
{
  static const struct timeval period = { 1, 0 };
  fsm_fire (luz);
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

static
void
task_alarma (struct event_handler_t* this)
{
  static const struct timeval period = { 1, 0 };
  fsm_fire (alarma);
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

static
void
task_codigo (struct event_handler_t* this)
{
  static const struct timeval period = { 1, 0 };
  fsm_fire (codigo);
  timeval_add (&this->next_activation, &this->next_activation, &period);
}

int
main ()
{
  static struct timeval period = { 1, 0 };
  struct timeval next;

  luz = fsm_new_luz ();
  alarma = fsm_new_alarma ();
  codigo = fsm_new_codigo ();

  printf("\n");
  printf("\t****************************************\n\n");
  printf("\t\tTeclas de activación:\n\n");
  printf("\tPresencia: p\n");
  printf("\tBotón luz: b\n");
  printf("\tPulsador código alarma: a\n");
  printf("\tContraseña de la alarma: 2 3 2\n\n");
  printf("\t****************************************\n\n");

  EventHandler luz_eh, codigo_eh, alarma_eh;
  reactor_init ();

  event_handler_init (&alarma_eh, 1, task_alarma);
  reactor_add_handler (&alarma_eh);

  event_handler_init (&codigo_eh, 2, task_codigo);
  reactor_add_handler (&codigo_eh);

  event_handler_init (&luz_eh, 3, task_luz);
  reactor_add_handler (&luz_eh);

  while (1) {
    timeval_add (&next, &next, &period);
    scan_keyboard();
    reactor_handle_events ();
  }
}
