/* compute distance for i.th point */
#define EQ ALGO->equations

inline float AR_dist(struct_ARmodel *AR, struct_algo *ALGO, float *x1, int i)
{
  float dist;
  if (AR->nb_pt_corr &&
      EQ[i]-AR->nb_pt_corr>=0 && EQ[i]+AR->nb_pt_corr<ALGO->nb_couples) {
    /* multiple points */
    dist = 0;
    for (int j=EQ[i]-AR->nb_pt_corr; j<=EQ[i]+AR->nb_pt_corr; j++) {
//      dist += qprod_scal(AR->SYST->A[j], x1, AR->SYST->nc) - AR->SYST->b[j];
//    dist /= 2*AR->nb_pt_corr + 1;
      float aux = qprod_scal(AR->SYST->A[j], x1, AR->SYST->nc)-AR->SYST->b[j];
      dist += aux*aux;
    }
    dist = sqrt(dist)/(2*AR->nb_pt_corr+1);
  }
  else /* one point */
    dist = qprod_scal(x1, AR->SYST->A[EQ[i]], AR->SYST->nc)
      - AR->SYST->b[EQ[i]];

  return dist;
}

inline float AR_dist_sq(struct_ARmodel *AR, struct_algo *ALGO,
			float *x1, int i)
{
  float dist;
  if (AR->nb_pt_corr &&
      EQ[i]-AR->nb_pt_corr>=0 && EQ[i]+AR->nb_pt_corr<ALGO->nb_couples) {
    /* multiple points */
    dist = 0;
    for (int j=EQ[i]-AR->nb_pt_corr; j<=EQ[i]+AR->nb_pt_corr; j++) {
//      dist += qprod_scal(AR->SYST->A[j], x1, AR->SYST->nc) - AR->SYST->b[j];
//    dist /= 2*AR->nb_pt_corr + 1;
      float aux = qprod_scal(AR->SYST->A[j], x1, AR->SYST->nc)-AR->SYST->b[j];
      dist += aux*aux;
    }
    dist = dist/(2*AR->nb_pt_corr+1);
  }
  else { /* one point */
    dist = qprod_scal(x1, AR->SYST->A[EQ[i]], AR->SYST->nc)
      - AR->SYST->b[EQ[i]];
    dist *= dist;
  }

  return dist;
}

#undef EQ
