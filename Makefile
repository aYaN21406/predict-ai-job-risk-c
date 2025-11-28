CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -I./src
SRC = src/main.c src/csv_reader.c src/risk_calc.c
LIBS =
BIN = bin/predict_ai_job_risk

# If user sets WITH_SQLITE=1, compile sqlite_store and link sqlite3
ifeq ($(WITH_SQLITE),1)
SRC += src/sqlite_store.c
CFLAGS += -DWITH_SQLITE
LIBS += -lsqlite3
endif

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -rf bin/*.o bin/predict_ai_job_risk output/*
