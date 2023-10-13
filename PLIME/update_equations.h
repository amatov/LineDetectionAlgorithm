#ifndef UPDATE_EQUATIONS_H
#define UPDATE_EQUATIONS_H

void AR_model_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			struct_ARmodel *AR, float eps_threshold);
void line_detect_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			   struct_ldetect *LDT);
void cdetect_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
		       struct_cdetect *CDT, float eps_threshold);
void c2detect_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			struct_cdetect *CDT, float eps_threshold);
void c3detect_update_eq(struct_algo_param *P_ALGO, struct_algo *ALGO,
			struct_cdetect *CDT, float eps_threshold);

#endif
