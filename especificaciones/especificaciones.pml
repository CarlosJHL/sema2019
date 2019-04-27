/*
Practica: SEMA
*/

// ----- ESPECIFICACIONES -----

ltl spec1 {
	[](P_LUZ -> <>((estado == ON_LUZ)))
}

ltl spec2 {
	[]((B && (estado == OFF_LUZ)) -> <>(estado == ON_LUZ))
}

ltl spec3 {
	[]((B && (estado == ON_LUZ) && ((!P_LUZ) W (estado == OFF_LUZ))) -> <>(estado == OFF_LUZ)) // weak until - no hay presencia mientras estado == ON_LUZ
}

ltl spec4 {
	[](((!B) && (estado == ON_LUZ) && ((!P_LUZ) W (estado == OFF_LUZ))) -> <>(estado == OFF_LUZ))
}

ltl spec5 {
	[](((!B) && P_LUZ && (estado == ON_LUZ)) -> <>(estado == ON_LUZ))
}

// ALARMA

ltl spec6 {
	[](P_ALARMA -> <>(estado_alarma == ON_ALARMA))
}

ltl spec7 {
	[]((code_ok && []((!P_ALARMA)) && (estado_alarma == ON_ALARMA)) -> <>(estado_alarma == OFF_ALARMA)) // Probar quitando lo de []!P
}

ltl spec8 {
	[]((code_ok && []((!P_ALARMA)) && (estado_alarma == OFF_ALARMA)) -> <>(estado_alarma == ON_ALARMA)) // Probar quitando lo de []!P
}

//--------------------------------------------------------------
// ----- VERIFICACION_LUZ -----

mtype = {ON_LUZ, OFF_LUZ, ON_ALARMA, OFF_ALARMA};
bit B = 0;
bit P_LUZ = 0;
bit P_ALARMA = 0;
bit code_ok = 0;
byte estado = OFF_LUZ;
byte estado_alarma = OFF_ALARMA;

active proctype luz() {
	do
	:: (estado == OFF_LUZ) -> atomic {
		if 
		:: P_LUZ -> estado = ON_LUZ; printf("Enciende luz por presencia\n"); P_LUZ = 0;
		:: B -> estado = ON_LUZ; printf("Enciende luz por boton\n"); B = 0;
		fi
	}
	:: (estado == ON_LUZ) -> atomic {
		if
		:: P_LUZ -> estado = ON_LUZ; printf("Se mantiene encendida la luz por presencia\n"); P_LUZ = 0;
		:: (B && (!P_LUZ)) -> estado = OFF_LUZ; printf("Apaga luz por boton\n"); B = 0;
		:: true -> estado = OFF_LUZ; printf("Apaga luz por tiempo\n");
		fi
	}
	od
}

active proctype alarma() {
	do
	:: (estado_alarma == OFF_ALARMA) -> atomic {
		if 
		:: P_ALARMA -> estado_alarma = ON_ALARMA; printf("Enciende alarma por presencia\n"); P_ALARMA = 0;
		:: code_ok -> estado_alarma = ON_ALARMA; printf("Enciende alarma por codigo\n"); code_ok = 0;
		fi
	}
	:: (estado_alarma == ON_ALARMA) -> atomic {
		if
		:: P_ALARMA -> estado_alarma = ON_ALARMA; printf("Se mantiene encendida la alarma por presencia\n"); P_ALARMA = 0;
		:: (code_ok && (!P_ALARMA)) -> estado_alarma = OFF_ALARMA; printf("Apaga alarma por codigo\n"); code_ok = 0;
		fi
	}
	od
}

active proctype codigo() {
	do
	:: if
	   :: code_ok = 1;
	   :: skip -> skip;
	   fi;
	   printf ("code_ok = %d\n", code_ok);
	od
}

active proctype entorno() {
	do
	:: if
	   :: P_ALARMA = 1;
	   :: P_LUZ = 1;
	   :: B = 1;
	   :: skip -> skip;
	   fi;
	   printf ("estado = %e, estado_alarma = %e, B = %d, P_ALARMA = %d, P_LUZ = %d\n", estado, estado_alarma, B, P_ALARMA, P_LUZ);
	od
}