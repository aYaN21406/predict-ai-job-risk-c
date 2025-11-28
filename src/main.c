#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_reader.h"
#include "risk_calc.h"
#include "sqlite_store.h"
#include <sys/stat.h>

static void ensure_output_dir() {
    struct stat st = {0};
    if (stat("output", &st) == -1) {
#ifdef _WIN32
        mkdir("output");
#else
        mkdir("output", 0755);
#endif
    }
}

int main(int argc, char **argv) {
    const char *input = "data/raw/fallback_occupations.csv";
    size_t topn = 10;
    if (argc >= 2) input = argv[1];
    if (argc >= 3) topn = (size_t)atoi(argv[2]);

    printf("=== AI Job Risk Predictor (C) ===\n");
    printf("Reading data from: %s\n", input);

    occ_table_t tbl = read_csv_file(input);
    if (!tbl.rows || tbl.n == 0) {
        fprintf(stderr, "Failed to read CSV or no data found.\n");
        return 1;
    }

    printf("Loaded %zu occupations.\n", tbl.n);
    printf("Computing risk scores...\n");

    compute_risk_scores(&tbl);

    print_top_n(&tbl, topn);

    ensure_output_dir();
    const char *outcsv = "output/occupations_features.csv";
    if (write_csv_output(outcsv, &tbl)) {
        printf("\nWrote: %s\n", outcsv);
    } else {
        printf("\nFailed to write CSV output.\n");
    }

#ifdef WITH_SQLITE
    const char *dbp = "output/occupations.db";
    if (save_to_sqlite(dbp, &tbl)) {
        printf("Saved results to SQLite DB: %s\n", dbp);
    } else {
        printf("SQLite save failed or not available.\n");
    }
#endif

    print_top_n(&tbl, topn);
    free_occ_table(&tbl);
    return 0;
}
