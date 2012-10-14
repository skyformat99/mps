#include "config.h"

#include "eventdef.h"
#include "eventpro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define DATABASE_NAME_ENVAR   "MPS_EVENT_DATABASE"
#define DEFAULT_DATABASE_NAME "mpsevent.db"

#define TELEMETRY_FILENAME_ENVAR "MPS_TELEMETRY_FILENAME"
#define DEFAULT_TELEMETRY_FILENAME "mpsio.log"

/* we output rows of dots.  One dot per SMALL_TICK events,
 * BIG_TICK dots per row. */

#define SMALL_TICK 1000
#define BIG_TICK   50

/* Utility code for logging to stderr with multiple log levels,
 * and for reporting errors.
 */

unsigned int verbosity = 4; /* TODO command-line -v switches */

#define LOG_ALWAYS    0
#define LOG_OFTEN     1
#define LOG_SOMETIMES 2
#define LOG_SELDOM    3
#define LOG_RARELY    4

static void vlog(unsigned int level, const char *format, va_list args)
{
        if (level <= verbosity) {
                fflush(stderr); /* sync */
                fprintf(stderr, "log %d: ", level);
                vfprintf(stderr, format, args);
                fprintf(stderr, "\n");
        }
}

static void log(unsigned int level, const char *format, ...)
{
        va_list args;
        va_start(args, format);
        vlog(level, format, args);
        va_end(args);
}

static void error(const char *format, ...)
{
        va_list args;
        fprintf(stderr, "Fatal error:  ");
        va_start(args, format);
        vlog(LOG_ALWAYS, format, args);
        va_end(args);
        exit(1);
}

static void sqlite_error(int res, sqlite3 *db, const char *format, ...)
{
        log(LOG_ALWAYS, "Fatal SQL error %d", res);
        va_list args;
        va_start(args, format);
        vlog(LOG_ALWAYS, format, args);
        va_end(args);
        log(LOG_ALWAYS, "SQLite message: %s\n", sqlite3_errmsg(db));
        exit(1);
}

/* openDatabase(p) opens the database file and returns a SQLite 3
 * database connection object. */

static sqlite3 *openDatabase(void)
{
        sqlite3 *db;
        int res;
        
        const char *filename = getenv(DATABASE_NAME_ENVAR);
        if(filename == NULL)
                filename = DEFAULT_DATABASE_NAME;
        log(LOG_ALWAYS, "Opening %s.", filename);
        
        res = sqlite3_open_v2(filename,
                              &db,
                              SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                              NULL); /* use default sqlite_vfs object */
        
        if (res != SQLITE_OK)
                sqlite_error(res, db, "Opening %s failed", filename);
        return db;
}

/* closeDatabase(db) closes the database opened by openDatabase(). */

static void closeDatabase(sqlite3 *db)
{
        int res = sqlite3_close(db);
        if (res != SQLITE_OK)
                sqlite_error(res, db, "Closing database failed"); 
        log(LOG_ALWAYS, "Closed database.");
}

/* We need to be able to test for the existence of a table.  The
 * SQLite3 API seems to have no way to explore metadata like this,
 * unless it is compiled in a particular way (in which case the
 * function sqlite3_table_column_metadata could be used).  Without
 * that assistance, we can use a simple SQL trick (which could also
 * tell us the number of rows in the table if we cared): */

static int tableExists(sqlite3* db, const char *tableName)
{
        const char *format = "SELECT SUM(1) FROM %s";
        char *sql;
        int res;

        sql = malloc(strlen(format) + strlen(tableName));
        if (!sql)
                error("Out of memory.");
        sprintf(sql, format, tableName);
        res = sqlite3_exec(db,
                           sql,
                           NULL, /* put in a callback here if we really want to know the number of rows */
                           NULL, /* callback closure */
                           NULL); /* error messages handled by sqlite_error */
        switch(res) {
        case SQLITE_OK:
                return 1; /* table exists */
                break;
        case SQLITE_ERROR:
                return 0; /* table does not exist; we can
                             probably do a better test for this case. */
                break;
        default:
                sqlite_error(res, db, "Table test failed: %s", tableName);
        }
        /* UNREACHED */
        return 0;
}

/* Unit test for tableExists() */

static const char *tableTests[] = {
        "event_kind",
        "spong",
        "EVENT_SegSplit",
};

static void testTableExists(sqlite3 *db)
{
        int i;
        for (i=0; i < (sizeof(tableTests)/sizeof(tableTests[0])); ++i) {
                if (tableExists(db, tableTests[i]))
                        printf("Table exists: %s\n", tableTests[i]);
                else 
                        printf("Table does not exist: %s\n", tableTests[i]);
        }
}

/* Utility functions for SQLite statements. */

static sqlite3_stmt *prepareStatement(sqlite3 *db,
                                      const char *sql)
{
        int res;
        sqlite3_stmt *statement;
        log(LOG_SELDOM, "Preparing statement %s", sql);
        res = sqlite3_prepare_v2(db, sql,
                                 -1, /* prepare whole string as statement */
                                 &statement,
                                 NULL);
        if (res != SQLITE_OK)
                sqlite_error(res, db, "statementpreparation failed: %s", sql);
        return statement;
}

static void finalizeStatement(sqlite3 *db,
                              sqlite3_stmt *statement)
{
        int res;
        res = sqlite3_finalize(statement);
        if (res != SQLITE_OK)
                sqlite_error(res, db, "event_type finalize failed");
}

/* Macro magic to make a CREATE TABLE statement for each event type. */

#define EVENT_PARAM_SQL_TYPE_A "INTEGER"
#define EVENT_PARAM_SQL_TYPE_P "INTEGER"
#define EVENT_PARAM_SQL_TYPE_U "INTEGER"
#define EVENT_PARAM_SQL_TYPE_W "INTEGER"
#define EVENT_PARAM_SQL_TYPE_D "REAL   "
#define EVENT_PARAM_SQL_TYPE_S "TEXT   "
#define EVENT_PARAM_SQL_TYPE_B "INTEGER"

#define EVENT_PARAM_SQL_COLUMN(X, index, sort, ident) \
        "\"" #ident "\" " EVENT_PARAM_SQL_TYPE_##sort ", "

#define EVENT_TABLE_CREATE(X, name, code, always, kind) \
        "CREATE TABLE IF NOT EXISTS EVENT_" #name " ( " \
        EVENT_##name##_PARAMS(EVENT_PARAM_SQL_COLUMN, X) \
        "time INTEGER, " \
        "log_serial INTEGER)",

/* An array of table-creation statement strings. */

const char *createStatements[] = {
        "CREATE TABLE IF NOT EXISTS event_kind (name    TEXT,"
        "                                       description TEXT,"
        "                                       enum    INTEGER PRIMARY KEY)",

        "CREATE TABLE IF NOT EXISTS event_type (name    TEXT,"
        "                                       code    INTEGER PRIMARY KEY,"
        "                                       always  INTEGER,"
        "                                       kind    INTEGER,"
        "  FOREIGN KEY (kind) REFERENCES event_kind(enum));",

        "CREATE TABLE IF NOT EXISTS event_log (name TEXT,"
        "                                      file_id INTEGER,"
        "                                      size INTEGER,"
        "                                      time_sec INTEGER,"
        "                                      time_nsec INTEGER,"
        "                                      completed INTEGER,"
        "                                      serial INTEGER PRIMARY KEY AUTOINCREMENT)",

EVENT_LIST(EVENT_TABLE_CREATE, X)
};

/* makeTables makes all the tables. */

static void makeTables(sqlite3 *db)
{
        int i;
        int res;
        
        for (i=0; i < (sizeof(createStatements)/sizeof(createStatements[0])); ++i) {
                log(LOG_SOMETIMES, "Creating tables.  SQL command: %s", createStatements[i]);
                res = sqlite3_exec(db,
                                   createStatements[i],
                                   NULL, /* No callback */
                                   NULL, /* No callback closure */
                                   NULL); /* error messages handled by sqlite_error */
                if (res != SQLITE_OK)
                        sqlite_error(res, db, "Table creation failed: %s", createStatements[i]);
        }
}

/* Populate the metadata "glue" tables event_kind and event_type. */

#define EVENT_KIND_DO_INSERT(X, name, description)    \
        res = sqlite3_bind_text(statement, 1, #name, -1, SQLITE_STATIC); \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "event_kind bind of name \"" #name "\" failed."); \
        res = sqlite3_bind_text(statement, 2, description, -1, SQLITE_STATIC); \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "event_kind bind of description \"" description "\" failed."); \
        res = sqlite3_bind_int(statement, 3, i);                        \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "event_kind bind of enum %d failed.", i); \
        ++i;                                                            \
        res = sqlite3_step(statement);                                  \
        if (res != SQLITE_DONE)                                         \
                sqlite_error(res, db, "event_kind insert of name \"" #name "\" failed."); \
        if (sqlite3_changes(db) != 0)                                   \
                log(LOG_SOMETIMES, "Insert of event_kind row for \"" #name "\" affected %d rows.", sqlite3_changes(db)); \
        res = sqlite3_reset(statement);                                 \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "Couldn't reset event_kind insert statement.");

#define EVENT_TYPE_DO_INSERT(X, name, code, always, kind)          \
        res = sqlite3_bind_text(statement, 1, #name, -1, SQLITE_STATIC); \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "event_type bind of name \"" #name "\" failed."); \
        res = sqlite3_bind_int(statement, 2, code);                     \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "event_type bind of code %d failed.", code); \
        res = sqlite3_bind_int(statement, 3, always);                   \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "event_type bind of always for name \"" #name "\" failed."); \
        res = sqlite3_bind_int(statement, 4, EventKind##kind);          \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "event_type bind of kind for name \"" #name "\" failed."); \
        res = sqlite3_step(statement);                                  \
        if (res != SQLITE_DONE)                                         \
                sqlite_error(res, db, "event_type insert of name \"" #name "\" failed."); \
        if (sqlite3_changes(db) != 0)                                   \
                log(LOG_SOMETIMES, "Insert of event_type row for \"" #name "\" affected %d rows.", sqlite3_changes(db)); \
        res = sqlite3_reset(statement);                                 \
        if (res != SQLITE_OK)                                           \
                sqlite_error(res, db, "Couldn't reset event_type insert statement.");

static void fillGlueTables(sqlite3 *db)
{
        int i;
        Res res;
        sqlite3_stmt *statement;
                
        statement = prepareStatement(db,
                                     "INSERT OR IGNORE INTO event_kind (name, description, enum)"
                                     "VALUES (?, ?, ?)");
        
        i = 0;
        EventKindENUM(EVENT_KIND_DO_INSERT, X);
        
        finalizeStatement(db, statement);
        
        statement = prepareStatement(db, 
                                     "INSERT OR IGNORE INTO event_type (name, code, always, kind)"
                                     "VALUES (?, ?, ?, ?)");
        EVENT_LIST(EVENT_TYPE_DO_INSERT, X);
        
        finalizeStatement(db, statement);
}

/* Populate the actual event tables. */

#define EVENT_TYPE_DECLARE_STATEMENT(X, name, code, always, kind) \
        sqlite3_stmt *stmt_##name;

#define EVENT_PARAM_PREPARE_IDENT(X, index, sort, ident) "\"" #ident "\", "

#define EVENT_PARAM_PREPARE_PLACE(X, index, sort, ident) "?, "

#define EVENT_TYPE_PREPARE_STATEMENT(X, name, code, always, kind) \
        stmt_##name = \
            prepareStatement(db, \
                             "INSERT INTO EVENT_" #name " (" \
                             EVENT_##name##_PARAMS(EVENT_PARAM_PREPARE_IDENT, X)        \
                             "log_serial, time) VALUES (" \
                             EVENT_##name##_PARAMS(EVENT_PARAM_PREPARE_PLACE,X) \
                             "?, ?)");

#define EVENT_TYPE_FINALIZE_STATEMENT(X, name, code, always, kind) \
        finalizeStatement(db, stmt_##name);

#define EVENT_PARAM_BIND_INTEGER(name, index, sort, ident) \
        res = sqlite3_bind_int64(statement, index+1, (unsigned long) event->name.f##index);

#define EVENT_PARAM_BIND_REAL(name, index, sort, ident) \
        res = sqlite3_bind_double(statement, index+1, event->name.f##index);

#define EVENT_PARAM_BIND_TEXT(name, index, sort, ident) \
        res = sqlite3_bind_text(statement, index+1, event->name.f##index, -1, SQLITE_STATIC);

#define EVENT_PARAM_BIND_A EVENT_PARAM_BIND_INTEGER
#define EVENT_PARAM_BIND_P EVENT_PARAM_BIND_INTEGER
#define EVENT_PARAM_BIND_U EVENT_PARAM_BIND_INTEGER
#define EVENT_PARAM_BIND_W EVENT_PARAM_BIND_INTEGER
#define EVENT_PARAM_BIND_D EVENT_PARAM_BIND_REAL   
#define EVENT_PARAM_BIND_S EVENT_PARAM_BIND_TEXT   
#define EVENT_PARAM_BIND_B EVENT_PARAM_BIND_INTEGER

#define EVENT_PARAM_BIND(name, index, sort, ident) \
        EVENT_PARAM_BIND_##sort (name, index, sort, ident) \
        if (res != SQLITE_OK) \
                sqlite_error(res, db, "Event " #name " bind of ident " #ident "failed."); \
        last_index = index+1;


#define EVENT_TYPE_WRITE_SQL(X, name, code, always, kind) \
        case code: \
        { \
                sqlite3_stmt *statement = stmt_##name; \
                int last_index = 0; \
                int res; \
                /* bind all the parameters of this particular event with macro magic. */ \
                EVENT_##name##_PARAMS(EVENT_PARAM_BIND, name) \
                /* bind the fields we store for every event */ \
                res = sqlite3_bind_int64(statement, last_index+1, log_serial); \
                if (res != SQLITE_OK) \
                        sqlite_error(res, db, "Event " #name " bind of log_serial failed."); \
                res = sqlite3_bind_int64(statement, last_index+2, clock); \
                if (res != SQLITE_OK) \
                        sqlite_error(res, db, "Event " #name " bind of clock failed."); \
                res = sqlite3_step(statement); \
                if (res != SQLITE_DONE) \
                        sqlite_error(res, db, "insert of event \"" #name "\" failed."); \
                res = sqlite3_reset(statement); \
                if (res != SQLITE_OK) \
                        sqlite_error(res, db, "Couldn't reset insert statement for \"" #name "\"."); \
        } \
        break;

/* readLog -- read and parse log
 */

static void readLog(EventProc proc,
                    sqlite3 *db)
{
        int log_serial = 0; /* TODO get this from event_log table */ 
        size_t eventCount = 0;
        /* declare statements for every event type */
        EVENT_LIST(EVENT_TYPE_DECLARE_STATEMENT, X);

        /* prepare statements for every event type */
        EVENT_LIST(EVENT_TYPE_PREPARE_STATEMENT, X);

        while (TRUE) { /* loop for each event */
                Event event;
                EventCode code;
                EventClock clock;
                Res res;
                
                /* Read and parse event. */
                res = EventRead(&event, proc);
                if (res == ResFAIL) break; /* eof */
                if (res != ResOK) error("Truncated log");
                clock = event->any.clock;
                code = event->any.code;
                
                /* Write event to SQLite. */
                switch (code) {
                        EVENT_LIST(EVENT_TYPE_WRITE_SQL, X);
                }
                EventDestroy(proc, event);
                eventCount++;
                if ((eventCount % SMALL_TICK) == 0) {
                        fprintf(stdout, ".");
                        fflush(stdout);
                        if (((eventCount / SMALL_TICK) % BIG_TICK) == 0) {
                                log(LOG_OFTEN, "%lu events.", (unsigned long)eventCount);
                        }
                }
        }
        log(LOG_OFTEN, "Wrote %lu events to SQL.", (unsigned long)eventCount);
        /* finalize all the statements */
        EVENT_LIST(EVENT_TYPE_FINALIZE_STATEMENT, X);
}



static Res logReader(void *file, void *p, size_t len)
{
        size_t n;
        
        n = fread(p, 1, len, (FILE *)file);
        return (n < len) ? (feof((FILE *)file) ? ResFAIL : ResIO) : ResOK;
}

#if 0
/* TODO. Find out whether the database already contains this log file.
 * Should probably also fopen it, and use fileno() and fstat() */

static int logFileSerial(sqlite3 *db, const char *filename)
{
        struct stat st;
        sqlite3_stmt *statement;

        stat(filename, &st);
        statement = prepareStatement(db,
                                     "SELECT serial, completed FROM event_log WHERE"
                                     "file_id = ? AND size = ? AND time_sec = ? AND time_nsec = ?");
        /* TODO: Get stat results, look up in event_log, return accordingly */
}
#endif

static FILE *openLog(void)
{
        const char *filename;
        FILE *input;

        filename = getenv(TELEMETRY_FILENAME_ENVAR);
        if(filename == NULL)
                filename = DEFAULT_TELEMETRY_FILENAME;
        input = fopen(filename, "rb");

        if (input == NULL)
                error("unable to open %s", filename);

        return input;
}

static void writeEventsToSQL(sqlite3 *db)
{
        Res res;
        EventProc proc;
        FILE *input;
        
        input = openLog();
        res = EventProcCreate(&proc, logReader, (void *)input);
        if (res != ResOK)
                error("Can't init EventProc module: error %d.", res);
        
        readLog(proc, db);

        EventProcDestroy(proc);
        (void)fclose(input);
}


int main(void)
{
        sqlite3 *db;

        /* TODO: command line args
         * -v verbosity;
         * -f log filename;
         * -d database filename;
         * -r rebuild glue tables (by dropping them);
         * -t unit tests?
         */
        
        db = openDatabase();
        makeTables(db);
        fillGlueTables(db);

        writeEventsToSQL(db);

        if (0) {
        testTableExists(db);
        }

        closeDatabase(db);
        return 0;
}
