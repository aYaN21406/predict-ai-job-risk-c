#ifndef SQLITE_STORE_H
#define SQLITE_STORE_H

#include "csv_reader.h"

#ifdef WITH_SQLITE
int save_to_sqlite(const char *db_path, const occ_table_t *tbl);
#else
static inline int save_to_sqlite(const char *db_path, const occ_table_t *tbl) { (void)db_path; (void)tbl; return 0; }
#endif

#endif
