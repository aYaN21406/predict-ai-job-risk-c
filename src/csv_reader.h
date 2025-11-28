#ifndef CSV_READER_H
#define CSV_READER_H

#include <stddef.h>

typedef struct {
    char *job;
    char *soc_code;
    double median_salary;
    double employment_2023;
    double growth_pct_2023_2033;
    double ai_capability_score;
    double routine_task_share;
    double risk_score;
} occupation_t;

typedef struct {
    occupation_t *rows;
    size_t n;
} occ_table_t;

occ_table_t read_csv_file(const char *path);
void free_occ_table(occ_table_t *tbl);
int write_csv_output(const char *path, const occ_table_t *tbl);
void print_top_n(const occ_table_t *tbl, size_t n);

#endif
