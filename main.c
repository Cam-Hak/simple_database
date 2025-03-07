#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* contains data about input lines */
typedef struct {
    char* buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

/* completion representation for meta commands */
typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

/* completion representation for statements */
typedef enum {
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

/* initializes input buffer struct */
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

/* meta command handler */
MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        exit(EXIT_SUCCESS);
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

/* represents different statement functionality */
typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

/* contains data about the statement called */
typedef struct {
    StatementType type;
} Statement;

/* returns the type of statement called based on whats passed in stdin */
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

/* handles statement calls */
void execute_statement(Statement* statement) {
    switch (statement->type) {
        case (STATEMENT_INSERT):
            printf("This is an insert.\n");
            break;
        case (STATEMENT_SELECT):
            printf("This is a select.\n");
            break;
    }
}

/* basic prompt */
void print_prompt() { printf("db > "); }

/* handles reading the input from stdin */
void read_input(InputBuffer* input_buffer) {

    /* getline takes buffer, size, filestream */
    ssize_t bytes_read = 
        getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    
    /* handling read error */
    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    /* getting rid of trailing newline */
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

/* frees input_buffer memory */
void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

int main(int argc, char *argv[]) {
    InputBuffer* input_buffer = new_input_buffer();

    /* main cmd loop */
    while (true) {

        print_prompt();
        read_input(input_buffer);

        /* checks validity of meta command / checks for .exit call */
        if (input_buffer->buffer[0] == '.') {
            switch (do_meta_command(input_buffer)) {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
            }
        }

        /* checks validity of statement command */
        Statement statement;
        switch (prepare_statement(input_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n",
                       input_buffer->buffer);    
                continue;
        }

        /* carries out statement instruction */
        execute_statement(&statement);
        printf("Executed.\n");
    }
}
