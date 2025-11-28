#define _GNU_SOURCE
#include "csv_reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *strip_quotes(char *s) {
    if (!s) return s;
    size_t len = strlen(s);
    if (len >= 2 && s[0] == '"' && s[len-1] == '"') {
        s[len-1] = '\0';
        return s+1;
    }
    return s;
}

occ_table_t read_csv_file(const char *path) {
    occ_table_t tbl = {NULL, 0};
    FILE *f = fopen(path, "r");
    if (!f) return tbl;

    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    // read header
    if ((len = getline(&line, &cap, f)) == -1) {
        fclose(f);
        return tbl;
    }

    size_t capacity = 16;
    tbl.rows = calloc(capacity, sizeof(occupation_t));
    tbl.n = 0;

    while ((len = getline(&line, &cap, f)) != -1) {
        // naive CSV split by commas; assumes no embedded commas except within quotes
        char *p = line;
        char *fields[7];
        int idx = 0;
        char *token;
        // parse tokens
        while (idx < 7) {
            if (*p == '"') {
                // quoted token
                char *start = ++p;
                while (*p && !(*p == '"' && (*(p+1) == ',' || *(p+1) == '\n' || *(p+1) == '\0'))) p++;
                if (*p == '"') {
                    *p = '\0';
                    fields[idx++] = strdup(start);
                    p++;
                    if (*p == ',') p++;
                }
            } else {
                token = p;
                while (*p && *p != ',' && *p != '\n') p++;
                char old = *p;
                *p = '\0';
                fields[idx++] = strdup(token);
                if (old == ',') p++;
                else break;
            }
        }

        if (idx < 7) {
            // skip malformed
            for (int i=0;i<idx;i++) free(fields[i]);
            continue;
        }

        if (tbl.n >= capacity) {
            capacity *= 2;
            tbl.rows = realloc(tbl.rows, capacity * sizeof(occupation_t));
        }

        occupation_t *r = &tbl.rows[tbl.n++];
        r->job = fields[0];
        r->soc_code = fields[1];
        r->median_salary = atof(fields[2]);
        r->employment_2023 = atof(fields[3]);
        r->growth_pct_2023_2033 = atof(fields[4]);
        r->ai_capability_score = atof(fields[5]);
        r->routine_task_share = atof(fields[6]);
        r->risk_score = 0.0;
    }

    free(line);
    fclose(f);
    return tbl;
}

void free_occ_table(occ_table_t *tbl) {
    if (!tbl || !tbl->rows) return;
    for (size_t i = 0; i < tbl->n; i++) {
        free(tbl->rows[i].job);
        free(tbl->rows[i].soc_code);
    }
    free(tbl->rows);
    tbl->rows = NULL;
    tbl->n = 0;
}

static int cmp_risk_desc(const void *a, const void *b) {
    const occupation_t *A = a;
    const occupation_t *B = b;
    if (A->risk_score < B->risk_score) return 1;
    if (A->risk_score > B->risk_score) return -1;
    return 0;
}

void print_top_n(const occ_table_t *tbl, size_t n) {
    if (!tbl || !tbl->rows || tbl->n == 0) return;
    // sort a copy
    occupation_t *copy = malloc(tbl->n * sizeof(occupation_t));
    memcpy(copy, tbl->rows, tbl->n * sizeof(occupation_t));
    qsort(copy, tbl->n, sizeof(occupation_t), cmp_risk_desc);
    printf("\n--- Top %zu AI Job Risk Scores ---\n", n > tbl->n ? tbl->n : n);
    for (size_t i = 0; i < n && i < tbl->n; i++) {
        printf("%zu. %s (SOC: %s) - Risk: %.2f%%\n",
               i+1, copy[i].job, copy[i].soc_code,
               copy[i].risk_score * 100.0);
    }
    free(copy);
}

int write_csv_output(const char *path, const occ_table_t *tbl) {
    if (!tbl || !tbl->rows) return 0;
    FILE *f = fopen(path, "w");
    if (!f) {
        fprintf(stderr, "Could not open output file: %s\n", path);
        return 0;
    }
    fprintf(f, "job,soc_code,median_salary,employment_2023,growth_pct_2023_2033,ai_capability_score,routine_task_share,risk_score\n");
    for (size_t i=0;i<tbl->n;i++) {
        occupation_t *r = &tbl->rows[i];
        fprintf(f, "\"%s\",%s,%.2f,%.0f,%.2f,%.4f,%.4f,%.6f\n",
               r->job,
               r->soc_code,
               r->median_salary,
               r->employment_2023,
               r->growth_pct_2023_2033,
               r->ai_capability_score,
               r->routine_task_share,
               r->risk_score);
    }
    fclose(f);
    return 1;
}
