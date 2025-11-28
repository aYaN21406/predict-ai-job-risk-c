#include "risk_calc.h"
#include <stdlib.h>
#include <string.h>

static double rescale(double val, double min_val, double max_val) {
    if (max_val == min_val) return 0.5;
    return (val - min_val) / (max_val - min_val);
}

void compute_risk_scores(occ_table_t *tbl) {
    if (!tbl || tbl->n == 0) return;

    double min_ai = tbl->rows[0].ai_capability_score, max_ai = min_ai;
    double min_routine = tbl->rows[0].routine_task_share, max_routine = min_routine;
    double min_growth = tbl->rows[0].growth_pct_2023_2033, max_growth = min_growth;

    for (size_t i=0;i<tbl->n;i++) {
        double a = tbl->rows[i].ai_capability_score;
        double b = tbl->rows[i].routine_task_share;
        double g = tbl->rows[i].growth_pct_2023_2033;
        if (a < min_ai) min_ai = a; if (a > max_ai) max_ai = a;
        if (b < min_routine) min_routine = b; if (b > max_routine) max_routine = b;
        if (g < min_growth) min_growth = g; if (g > max_growth) max_growth = g;
    }

    for (size_t i=0;i<tbl->n;i++) {
        double ai = tbl->rows[i].ai_capability_score;
        double routine = tbl->rows[i].routine_task_share;
        double growth = tbl->rows[i].growth_pct_2023_2033;

        double ai_scaled = rescale(ai, min_ai, max_ai);
        double routine_scaled = rescale(routine, min_routine, max_routine);

        // growth_inverse: higher when growth is lower. We map growth range -> inverse
        double growth_inverse = (max_growth - growth) + (0.0); // shift so higher growth means lower score
        // To rescale, use same min/max logic: min_growth_inverse = max_growth - max_growth = 0
        double min_gi = 0;
        double max_gi = max_growth - min_growth;
        double gi_scaled = (max_gi == 0) ? 0.5 : (growth_inverse - min_gi) / (max_gi - min_gi);

        // Weighted average: AI 50%, routine 30%, growth inverse 20%
        double risk = 0.5 * ai_scaled + 0.3 * routine_scaled + 0.2 * gi_scaled;
        tbl->rows[i].risk_score = risk;
    }
}
