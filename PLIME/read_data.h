#ifndef READ_DATA_H
#define READ_DATA_H

void read_AR_data (struct_ARmodel *);
void init_ar (struct_algo_param *,struct_algo *, struct_ARmodel *);

void read_optical_data (struct_optical *);
void init_optical (struct_algo_param *,struct_algo *, struct_optical *);

void read_ldetect_data (struct_ldetect *);
void init_ldetect (struct_algo_param *,struct_algo *, struct_ldetect *);

void read_cdetect_data (struct_cdetect *CDT);
void init_cdetect (struct_algo_param *, struct_algo *, struct_cdetect *);

#endif
