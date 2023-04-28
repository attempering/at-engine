#ifndef AT_MB_WIN__DEF_H__
#define AT_MB_WIN__DEF_H__



/* struct for mb->bin2wins */
typedef struct at_mb_win_ids_t_ {
  int count;
  int curr_id_;
  int *ids;
} at_mb_win_ids_t;


/* Three types of windows for different quantities 
 *
 * 1. unrestricted grid values
 *    e.g., reconstructed energy histograms
 *    Such quantities do not have to work with
 *    the window-accumulator module.
 *    The window boundary variables are js_grid_unres and jt_grid_unres.
 *    Such windows are not necessarily symmetric.
 *
 * 2. bin values
 *    e.g., bin-averaged internal energy
 *    Such quantities are supposed to work with
 *    the window-accumulator modules.
 *    The window boundary variables are js_bin and jt_bin.
 *    The windows are usually symmetric
 *    and are contained in the unrestricted grid windows
 *
 * 3. restricted grid values
 *    e.g., internal energy, heat capacity at the grid points
 *    Such quantities are supposed to work with
 *    the window-accumulator modules.
 *    The window boundary variables are js_grid_res and jt_grid_res.
 *    The windows are contained in the corresponding bin values
 *
 */

typedef struct at_mb_win_t_ {
  int n;
  int bwmod;
  double bwdel;

  int *js_grid_unres;
  int *jt_grid_unres;

  int *js_bin;
  int *jt_bin;

  int *js_grid_res;
  int *jt_grid_res;

  /* mapping from bin to affected windows */
  at_mb_win_ids_t *bin2wins;

} at_mb_win_t;


#endif
