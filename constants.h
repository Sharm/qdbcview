#ifndef CONSTANTS_H
#define CONSTANTS_H

/* -----------------------------CONSTANTS----------------------------- */
#define DBC_HEADER 1128416343

enum DBCFileError {
    DBCFILE_ERROR_OK,
    DBCFILE_ERROR_FILE_DOES_NOT_EXIST,
    DBCFILE_ERROR_FAILED_TO_OPEN_A_FILE,
    DBCFILE_ERROR_FILE_IS_EMPTY,
    DBCFILE_ERROR_FILE_IS_TOO_SMALL,
    DBCFILE_ERROR_FILE_IS_NOT_A_DBC_FILE,
    DBCFILE_ERROR_UNUSUAL_RECORD_SIZE,
    DBCFILE_ERROR_INVALID_DBC_TABLE_PARAMETERS,
    DBCFILE_ERROR_RECORD_DATA_IS_EMPTY,
    DBCFILE_ERROR_STRING_DATA_IS_EMPTY,
    DBCFILE_ERROR_AUTO_DETECT_FAILED_INVALID_FIELD,
    DBCFILE_ERROR_AUTO_DETECT_FAILED_UNK_FIELD_SIZE
};

enum DBCFileErrorCorrection {
    DBCFILE_IGNORE_FIELD_SIZE         = 1,
    DBCFILE_IGNORE_EMPTY_STRING_BLOCK = 2
};

enum DBCFieldType {
    FIELD_TYPE_TEXT,
    FIELD_TYPE_FLOAT,
    FIELD_TYPE_UINT,
    FIELD_TYPE_BOOL,
    FIELD_TYPE_INT,
    FIELD_TYPE_NULLS,
    FIELD_TYPE_NULLTEXT,
    FIELD_TYPE_BITMASK,
    FIELD_TYPE_UNKNOWN,
    FIELD_TYPE_ID = FIELD_TYPE_UINT,
    FIELD_TYPE_DEFAULT = FIELD_TYPE_UINT
};

enum SQLFieldType {
    FIELD_TYPE_SQL_TINYINT,
    FIELD_TYPE_SQL_TINYINT_UNSIGNED,
    FIELD_TYPE_SQL_SMALLINT,
    FIELD_TYPE_SQL_SMALLINT_UNSIGNED,
    FIELD_TYPE_SQL_MEDIUMINT,
    FIELD_TYPE_SQL_MEDIUMINT_UNSIGNED,
    FIELD_TYPE_SQL_INT,
    FIELD_TYPE_SQL_INT_UNSIGNED,
    FIELD_TYPE_SQL_FLOAT,
    FIELD_TYPE_SQL_BOOL,
    FIELD_TYPE_SQL_TINYTEXT,
    FIELD_TYPE_SQL_TEXT,
    FIELD_TYPE_SQL_MEDIUMTEXT,
    FIELD_TYPE_SQL_LONGTEXT,
    FIELD_TYPE_SQL_BITMASK = FIELD_TYPE_SQL_INT_UNSIGNED,
    FIELD_TYPE_SQL_ID = FIELD_TYPE_SQL_INT_UNSIGNED,
    FIELD_TYPE_SQL_UNKNOWN,
    FIELD_TYPE_SQL_DEFAULT = FIELD_TYPE_SQL_INT
};

#define SQL_TINYINT_LOW            -128
#define SQL_TINYINT_HI              127
#define SQL_TINYINT_UNSIGNED_LOW    0
#define SQL_TINYINT_UNSIGNED_HI     255
#define SQL_SMALLINT_LOW           -32768
#define SQL_SMALLINT_HI             32767
#define SQL_SMALLINT_UNSIGNED_LOW   0
#define SQL_SMALLINT_UNSIGNED_HI    65535
#define SQL_MEDIUMINT_LOW          -8388608
#define SQL_MEDIUMINT_HI            8388607
#define SQL_MEDIUMINT_UNSIGNED_LOW  0
#define SQL_MEDIUMINT_UNSIGNED_HI   16777215
#define SQL_INT_LOW                -2147483648
#define SQL_INT_HI                  2147483647
#define SQL_INT_UNSIGNED_LOW        0
#define SQL_INT_UNSIGNED_HI         4294967295
#define SQL_TINYTEXT                255
#define SQL_TEXT                    65535
#define SQL_MEDIUMTEXT              16777215
#define SQL_LONGTEXT                4294967295

#define BITMASK_MAX_BIT 31

enum BitmaskRepresentation {
    BITMASK_VIEW_COMMA_SEPARATED_POSITIVE_BITS,
    BITMASK_VIEW_COMMA_SEPARATED_POSITIVE_BITS_ALT,
    BITMASK_VIEW_DEFAULT = BITMASK_VIEW_COMMA_SEPARATED_POSITIVE_BITS
};

#endif // CONSTANTS_H
