#include "common.h"

void neurox_report_error(neurox_diagnostic_t *diag) {
    fprintf(stderr, "\033[1;31merror\033[0m: %s:%d:%d: %s\n",
            diag->filename ? diag->filename : "<unknown>",
            diag->line,
            diag->column,
            diag->message);
}

void neurox_report_warning(neurox_diagnostic_t *diag) {
    fprintf(stderr, "\033[1;33mwarning\033[0m: %s:%d:%d: %s\n",
            diag->filename ? diag->filename : "<unknown>",
            diag->line,
            diag->column,
            diag->message);
}
