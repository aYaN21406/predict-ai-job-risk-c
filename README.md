# Predict-AI-Job-Risk (C)

**What:** CLI tool in C that computes AI automation `risk_score` for job roles from a CSV, writes processed CSV, prints top N risky jobs, and can optionally store results in a SQLite database.

**Stack:** Standard C (C11), small CSV parser, optional SQLite storage (requires `libsqlite3`).

## Features

- Reads `data/raw/fallback_occupations.csv` (or your own CSV with same columns).
- Computes `risk_score` as a weighted composite (ai_capability_score, routine_task_share, inverse growth).
- Outputs `output/occupations_features.csv`.
- Prints top N risky jobs (default N=10).
- Optional: store results to SQLite DB (`output/occupations.db`) if compiled with SQLite.

## CSV format expected

CSV must have headers:
```
job,soc_code,median_salary,employment_2023,growth_pct_2023_2033,ai_capability_score,routine_task_share
```

A fallback file is included at `data/raw/fallback_occupations.csv`.

## Quick start (Linux/macOS)

1. Install `gcc` and optionally `libsqlite3-dev` (for SQLite support).
   - Ubuntu: `sudo apt install build-essential libsqlite3-dev`
   - macOS (Homebrew): `brew install sqlite3`

2. Build:
   - Without SQLite: `make`
   - With SQLite: `make WITH_SQLITE=1`

3. Run:
   ```bash
   # default: reads fallback and writes output
   ./bin/predict_ai_job_risk
   
   # load a custom CSV and print top 5
   ./bin/predict_ai_job_risk data/raw/yourfile.csv 5
   ```

4. Output:
   - `output/occupations_features.csv` - processed CSV with `risk_score`.
   - optionally `output/occupations.db` - SQLite DB (if built with SQLite flag).

## Project Structure

```
predict-ai-job-risk-c/
+-- README.md
+-- .gitignore
+-- Makefile
+-- LICENSE
+-- data/
|   +-- raw/
|       +-- fallback_occupations.csv
+-- src/
|   +-- main.c
|   +-- csv_reader.c
|   +-- csv_reader.h
|   +-- risk_calc.c
|   +-- risk_calc.h
|   +-- sqlite_store.c
|   +-- sqlite_store.h
+-- output/   # created at runtime
```

## How this maps to the R project

- This C project implements the feature engineering & scoring step in a portable CLI.
- Use the produced CSV as input to R for modeling & visualization.

## License

MIT - see LICENSE file.
