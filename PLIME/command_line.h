#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include "struct.h"

char** update_command_line(int *argc, char *argv[]);
int read_command_line_data_type(int argc, char *argv[]);
void read_command_line_common(int argc, char *argv[], struct_algo_param*);
void read_command_line_AR(int argc, char *argv[], struct_ARmodel*);
void read_command_line_opt(int argc, char *argv[], struct_optical *);
void read_command_line_ldetect(int argc, char *argv[], struct_ldetect *);
void read_command_line_cdetect(int argc,char *argv[],struct_cdetect *);

void valid_AR_parameters(struct_algo_param *, struct_algo *,
			 struct_ARmodel *);
void valid_opt_parameters(struct_algo_param *, struct_algo *,
			  struct_optical *);
void valid_ldetect_parameters(struct_algo_param *, struct_algo *,
			      struct_ldetect *);
void valid_cdetect_parameters(struct_algo_param *, struct_algo *,
			      struct_cdetect *);

#endif
