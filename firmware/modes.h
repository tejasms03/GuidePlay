#ifndef MODES_H
#define MODES_H

extern int wall_tol[];
extern int arc_tol[];      
extern int tempo_tol[];  

int run_mode(int mode, int diff);
int mode_shoreline(int diff);
int mode_ground(int diff);
int mode_sweep(int diff);  
int mode_touch(int diff);

#endif