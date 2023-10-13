#ifndef DATA_OUTPUT_H
#define DATA_OUTPUT_H

void display_last_subsyst_size (struct_algo *);
void display_time_informations(struct_algo_param *P_ALGO);
void create_eq_syst_files(struct_eq_system *, struct_algo_param *);

/* AR model */
void print_AR_param(struct_algo_param *, struct_algo *, struct_ARmodel *);
void print_AR_subsyst_results(struct_ARmodel *, struct_algo *, struct_eq_system *);
void print_AR_final_results(int nb_subsyst, struct_ARmodel *);
void print_AR_error(struct_ARmodel *);
void write_matlab_AR_file(struct_algo_param *, struct_algo *, struct_ARmodel *);
/* Optical flow */
void print_optical_param(struct_algo_param *, struct_algo *, struct_optical *);
/* Line detection  */
void print_ldetect_param(struct_algo_param *, struct_algo *, struct_ldetect *);
void calculate_lms_param(struct_ldetect *LDT);
void calculate_ldetect_bench_mark(struct_ldetect *, struct_algo_param *,
				 struct_algo *);
void create_ldetect_output_bmp(struct_ldetect *, struct_algo *, struct_algo_param *);
/* Conic detection */
void print_cdetect_param(struct_algo_param *, struct_algo *, struct_cdetect *);
void create_cdetect_output_bmp(struct_cdetect *, struct_algo *,
			       struct_algo_param *);
/* Conic_2 & conic_3 detection */
void display_last_subsystem_info_cdt (struct_algo *ALGO, struct_cdetect *CDT);
void calculate_cdetect_bench_mark(struct_cdetect *, struct_algo_param *,
				 struct_algo *);
void create_c23detect_output_bmp(struct_cdetect *, struct_algo *,
				 struct_algo_param *);

#endif

