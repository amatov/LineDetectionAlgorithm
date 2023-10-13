#ifndef LDETECT_TK_H
#define LDETECT_TK_H

#include "leon_tcltk.h"

void line_detect_tk_init_window(struct_ldetect*);
void line_detect_tk_update_canvas(struct_ldetect*, struct_algo*,
			      struct_algo_param*);
void line_detect_tk_update_mon_canvas(struct_ldetect *LDT, float *x);

void line_detect_tk_flag_classified(struct_tk *TK, point_2d_float point,
				    int index_subsystem);
void line_detect_tk_flag_non_classified(struct_ldetect *LDT,
					struct_algo *ALGO);
void line_detect_tk_put_start(struct_ldetect *LDT);
void line_detect_tk_remove_start(struct_ldetect *LDT);

#endif
