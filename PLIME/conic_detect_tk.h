#ifndef CDETECT_TK_H
#define CDETECT_TK_H

#include "leon_tcltk.h"

void conic_detect_tk_init_window(struct_cdetect*);

void conic_detect_tk_put_start(struct_cdetect *CDT);
void conic_detect_tk_remove_start(struct_cdetect *CDT);
void conic_detect_tk_update_canvas(struct_cdetect*, struct_algo*,
				   struct_algo_param*);
void conic_detect_tk_update_mon_canvas(struct_cdetect *, float *);

void conic2_detect_tk_put_start(struct_cdetect *CDT);
void conic2_detect_tk_remove_start(struct_cdetect *CDT);
void conic2_detect_tk_update_canvas(struct_cdetect*, struct_algo*,
				    struct_algo_param*);
void conic2_detect_tk_update_mon_canvas(struct_cdetect *, float *,
					int i1, int i2, float current_temp);

void conic3_detect_tk_put_start(struct_cdetect *CDT);
void conic3_detect_tk_remove_start(struct_cdetect *CDT);
void conic3_detect_tk_update_canvas(struct_cdetect*, struct_algo*,
				    struct_algo_param*);
void conic3_1_detect_tk_update_mon_canvas(struct_cdetect *, float *, int i1,
					  point_2d_float u1,
					  point_2d_float u2,
					  point_2d_int f1, point_2d_int f2,
					  float current_temp);
void conic3_2_detect_tk_update_mon_canvas(struct_cdetect *, float *, int i1,
					  point_2d_float f1_est,
					  point_2d_float f2_est,
					  point_2d_int f1, point_2d_int f2);

void conic_detect_tk_flag_classified(struct_tk *TK, float *point,
				     int index_subsyst);
#define CDT_UNFLAG -1
void conic_detect_tk_flag_non_classified(struct_cdetect *CDT,
					 struct_algo *ALGO);
void conic_detect_tk_flag_rejected_point(struct_cdetect *CDT, int index);
#endif
