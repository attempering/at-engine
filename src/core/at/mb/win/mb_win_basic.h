#ifndef AT__MB_WIN_BASIC_H__
#define AT__MB_WIN_BASIC_H__



//#include "mb_win__def.h"
#include "../mb__def.h"

#include <stdio.h>


/* windows for unrestricted grid values
 * They can used for reconstructing energy histograms.
 *
 * Such windows are not designed to work with the window-accumulator module.
 */
void mb_win__make_unres_windows_for_grid_estimators(
    int n, const double *barr, double bdel,
    int bw_mod, double bw_del,
    int *js_grid_raw, int *jt_grid_raw);


/* windows for bin estimators
 * such as the bin-averaged internal energy
 *
 * Such windows are designed to work with window-accumulator module.
 * They are usually symmetric. */
void mb_win__make_windows_for_bin_estimators(
    int n,
    int symmetric,
    const int *js_grid_raw, const int *jt_grid_raw,
    int *js_bin, int *jt_bin);


/* windows for grid estimators
 * such as the internal energy and heat capacity at the grid points.
 *
 * Such windows are designed to work with window-accumulator module.
 * They are contained in the bin estimators */
void mb_win__make_res_windows_for_grid_estimators(int n,
    int *js_bin, int *jt_bin,
    int *js_grid_res, int *jt_grid_res);


typedef struct cfg_t_ cfg_t;
typedef struct mb_t_ mb_t;

int mb_win__cfg_init(mb_win_t *win, cfg_t *cfg, mb_t *mb);

void mb_win__finish(mb_win_t *win);

void mb_win__manifest(const mb_win_t *win, FILE *fp, int arrmax);


#endif
