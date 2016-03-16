#define ENVAR_C_SOURCE
// ======================================================================== */
//      Avian Kernel    Bryan Webb (C) 2016
//      File:           avian/envar.c
//      Purpose:        Define enviornmental global variables
// ======================================================================== */

#include <envar.h>
#include <util.h>

void ENVAR_init(void)
{
   ENVAR.FLAGS.listen    = false;
   ENVAR.FLAGS.repaint    = false;
   ENVAR.FLAGS.keypress = false;
   ENVAR.GLOBAL.echostate = ECHO;
   ENVAR.FLAGS.captain = false;
   ENVAR.GLOBAL.tabsize = 4;
   notify(this, "Starting enviornment\n");
}