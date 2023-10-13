#ifndef LEON_H
#define LEON_H

/* Maximun nb of parameters in *.cfg file */
#define MAX_CMD_LINE_PARAM 100

/* Number of iterations between 2 TK updates (in full graphical monitoring) */
#define MONITORING_STEP 1500

/* Minimal float */
#define EPSILON 1E-10

/***********************************************************************/
/* Algorithm's core parameters                                         */
/***********************************************************************/

/* ********************* AR modeling */
//#define AR_RELATIVE_EPSILON 0.1

/* ********************* Circle detection */

/* min & max distances between the two pts (tan estimation with chord) */
#define MIN_AB 3
#define MAX_AB 35

#define ALPHA_DIST_CIRC 2 /* geo_dist=|delta|+ALPHA_DIST_CIRC*|delta_r|     */

/* Enhanced postprocessing (update_equations.cc) */
#define ANGULAR_THRESHOLD_CIRCLE 0.99 /* Point accepted if	            */
				 /* |cos(a1)| > ANGULAR_THRESHOLD	    */
#define ENHANCED_THRESHOLD_CIRCLE 0.25  /* Subsystem accepted if            */
				 /* enhanced_size > ENHANCED_THRESHOLD*size */

/* ********************* Ellipse detection */

/* geo_dist=(|delta1|+|delta2|)/2+ALPHA_DIST_ELLIPSE*|delta_r| */
#define ALPHA_DIST_ELLIPSE 2

#define CORRELATION_THRESHOLD 0.2

/* Enhanced postprocessing (update_equations.cc) */
#define ANGULAR_THRESHOLD_ELLIPSE 2*0.99 /* Point accepted if		    */
				 /* |cos(a1)|+|cos(a2)| > ANGULAR_THRESHOLD */
#define ENHANCED_THRESHOLD_ELLIPSE 0.25  /* Subsystem accepted if           */
				 /* enhanced_size > ENHANCED_THRESHOLD*size */

/*************/
/* Tk colors */
/*************/
#define CROSS_COLOR "snow"
#define CLUSTER_EDGES_COLOR "SpringGreen -stipple gray50"

/* Line detection */
#define GOOD_LINE_COLOR "snow"
#define ERR_LINE_COLOR "grey36"
#define START_POINT_LINE_COLOR "green"

/* circle detection */
#define GOOD_CIRCLE_COLOR "snow"
#define ERR_CIRCLE_DARK_COLOR "grey36"
#define ERR_CIRCLE_LIGHT_COLOR "snow"

/* Ellipse detection */
#define GOOD_ELLIPSE_COLOR "snow"
#define ESTIMATED_ELLIPSE_COLOR "snow -width 2"
#define ERR_ELLIPSE_COLOR "grey36"
#define NON_CLASSIFIED_POINTS_COLOR "red"
#define REJECTED_POINTS_COLOR "red"
#define REJECTED_AFTER_POSTPROCESSING_COLOR "grey36"

#define NB_SUBSYSTEM_COLOR 8
#define SUBSYS_DARK_COLORS {"gold4","green4","tomato4","DarkSeaGreen3","cyan4","turquoise4","chocolate4","yellow4"}
#define SUBSYS_LIGHT_COLORS {"gold1","green1","tomato1","DarkSeaGreen1","cyan2","turquoise2","chocolate1","yellow1"}

/**********/
/* DIVERS */
/**********/

#define WELL_CLASSIFIED_PT_SIZE 2 /* size of pt for well classifed pts monitoring */
//#define DISPLAY_ORIGIN		  /* enable origin monitoring in TK window (LDT)  */

#endif
