#ifndef NEUROX_LEXER_H
#define NEUROX_LEXER_H

#include "common.h"

// Token types
typedef enum {
    // Literals
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    
    // Keywords
    TOKEN_ROBOT,
    TOKEN_MOTOR,
    TOKEN_SERVO,
    TOKEN_SENSOR,
    TOKEN_GPIO,
    TOKEN_BUS,
    TOKEN_NET,
    TOKEN_MQTT,
    TOKEN_TOPIC,
    TOKEN_PUBLISH,
    TOKEN_ON,
    TOKEN_TASK,
    TOKEN_SCHEDULE,
    TOKEN_LIMITS,
    TOKEN_WHEN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_LET,
    TOKEN_WAIT,
    TOKEN_STOP,
    TOKEN_TURN,
    TOKEN_ESTOP,
    TOKEN_MESSAGE,
    TOKEN_AS,
    TOKEN_TYPE,
    TOKEN_MODE,
    TOKEN_BROKER,
    TOKEN_CLIENT_ID,
    TOKEN_QOS,
    TOKEN_PRIORITY,
    TOKEN_MAX,
    TOKEN_MIN,
    TOKEN_JSON,
    TOKEN_NOW,
    TOKEN_VALUE,
    TOKEN_POWER,
    TOKEN_READS,
    TOKEN_CLOCKWISE,
    TOKEN_COUNTERCLOCKWISE,
    
    // Types
    TOKEN_TYPE_PERCENT,
    TOKEN_TYPE_MS,
    TOKEN_TYPE_CM,
    TOKEN_TYPE_DEG,
    TOKEN_TYPE_HZ,
    TOKEN_TYPE_DISTANCE,
    TOKEN_TYPE_ANGLE,
    TOKEN_TYPE_SPEED,
    
    // Priority levels
    TOKEN_HIGH,
    TOKEN_MEDIUM,
    TOKEN_LOW,
    
    // Pin modes
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_INPUT_PULLUP,
    TOKEN_INPUT_PULLDOWN,
    
    // Bus types
    TOKEN_I2C,
    TOKEN_SPI,
    TOKEN_CAN,
    TOKEN_UART,
    
    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_BANG_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_AMP_AMP,
    TOKEN_PIPE_PIPE,
    TOKEN_BANG,
    TOKEN_AT,
    TOKEN_COLON,
    TOKEN_DOUBLE_COLON,
    TOKEN_ARROW,
    
    // Delimiters
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    
    // Special
    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_ERROR,
} token_type_t;

typedef struct {
    token_type_t type;
    const char *start;
    size_t length;
    int line;
    int column;
} token_t;

typedef struct {
    const char *source;
    const char *start;
    const char *current;
    int line;
    int column;
    const char *filename;
} lexer_t;

// Lexer API
void lexer_init(lexer_t *lexer, const char *source, const char *filename);
token_t lexer_next_token(lexer_t *lexer);
const char *token_type_to_string(token_type_t type);
void token_print(token_t *token);

// Token utilities
bool token_equals(token_t *token, const char *text);
char *token_to_string(token_t *token);

#endif // NEUROX_LEXER_H
