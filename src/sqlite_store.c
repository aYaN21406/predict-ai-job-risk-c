#ifdef WITH_SQLITE
#include "sqlite_store.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int save_to_sqlite(const char *db_path, const occ_table_t *tbl) {
    if (!tbl || !tbl->rows) return 0;

    sqlite3 *db;
    char *err = NULL;

    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err);
        sqlite3_close(db);
        return 0;
    }

    const char *create_sql = "CREATE TABLE IF NOT EXISTS occupations ("
        "job TEXT, soc_code TEXT, median_salary REAL, "
        "employment_2023 REAL, growth_pct_2023_2033 REAL, "
        "ai_capability_score REAL, routine_task_share REAL, risk_score REAL)";

    if (sqlite3_exec(db, create_sql, 0, 0, &err) != SQLITE_OK) {
        fprintf(stderr, "Create failed: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err);
        sqlite3_close(db);
        return 0;
    }

    const char *insert_sql = "INSERT INTO occupations (job, soc_code, median_salary, "
        "employment_2023, growth_pct_2023_2033, ai_capability_score, routine_task_share, risk_score) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 0;
    }

    // purge existing
    if (sqlite3_exec(db, "DELETE FROM occupations;", 0, 0, &err) != SQLITE_OK) {
        sqlite3_free(err);
    }

    for (size_t i=0;i<tbl->n;i++) {
        sqlite3_reset(stmt);
        sqlite3_bind_text(stmt, 1, tbl->rows[i].job, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, tbl->rows[i].soc_code, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 3, tbl->rows[i].median_salary);
        sqlite3_bind_double(stmt, 4, tbl->rows[i].employment_2023);
        sqlite3_bind_double(stmt, 5, tbl->rows[i].growth_pct_2023_2033);
        sqlite3_bind_double(stmt, 6, tbl->rows[i].ai_capability_score);
        sqlite3_bind_double(stmt, 7, tbl->rows[i].routine_task_share);
        sqlite3_bind_double(stmt, 8, tbl->rows[i].risk_score);
        sqlite3_step(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}
#endif
