#ifndef CREATE_MOVEMENT_EQUATIONS_H
#define CREATE_MOVEMENT_EQUATIONS_H

void create_AR_equations(struct_ARmodel *);
void create_opt_equations(struct_optical *);
void create_ldetect_equations(struct_ldetect *, char *sys_file_name);
void create_cdetect_equations(struct_cdetect *);
void create_c23detect_equations(struct_cdetect *);

#endif
