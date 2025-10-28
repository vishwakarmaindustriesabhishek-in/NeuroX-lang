#include "lexer.h"
#include <ctype.h>

// Keyword table
typedef struct {
    const char *text;
    token_type_t type;
} keyword_t;

static keyword_t keywords[] = {
    {"robot", TOKEN_ROBOT},
    {"motor", TOKEN_MOTOR},
    {"servo", TOKEN_SERVO},
    {"sensor", TOKEN_SENSOR},
    {"gpio", TOKEN_GPIO},
    {"bus", TOKEN_BUS},
    {"net", TOKEN_NET},
    {"mqtt", TOKEN_MQTT},
    {"topic", TOKEN_TOPIC},
    {"publish", TOKEN_PUBLISH},
    {"on", TOKEN_ON},
    {"task", TOKEN_TASK},
    {"schedule", TOKEN_SCHEDULE},
    {"limits", TOKEN_LIMITS},
    {"when", TOKEN_WHEN},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"let", TOKEN_LET},
    {"wait", TOKEN_WAIT},
    {"stop", TOKEN_STOP},
    {"turn", TOKEN_TURN},
    {"estop", TOKEN_ESTOP},
    {"message", TOKEN_MESSAGE},
    {"as", TOKEN_AS},
    {"type", TOKEN_TYPE},
    {"mode", TOKEN_MODE},
    {"broker", TOKEN_BROKER},
    {"client_id", TOKEN_CLIENT_ID},
    {"qos", TOKEN_QOS},
    {"priority", TOKEN_PRIORITY},
    {"max", TOKEN_MAX},
    {"min", TOKEN_MIN},
    {"json", TOKEN_JSON},
    {"now", TOKEN_NOW},
    {"value", TOKEN_VALUE},
    {"power", TOKEN_POWER},
    {"reads", TOKEN_READS},
    {"clockwise", TOKEN_CLOCKWISE},
    {"counterclockwise", TOKEN_COUNTERCLOCKWISE},
    
    // Types
    {"Percent", TOKEN_TYPE_PERCENT},
    {"ms", TOKEN_TYPE_MS},
    {"cm", TOKEN_TYPE_CM},
    {"deg", TOKEN_TYPE_DEG},
    {"Hz", TOKEN_TYPE_HZ},
    {"Distance", TOKEN_TYPE_DISTANCE},
    {"Angle", TOKEN_TYPE_ANGLE},
    {"Speed", TOKEN_TYPE_SPEED},
    
    // Priority
    {"HIGH", TOKEN_HIGH},
    {"MEDIUM", TOKEN_MEDIUM},
    {"LOW", TOKEN_LOW},
    
    // Pin modes
    {"Input", TOKEN_INPUT},
    {"Output", TOKEN_OUTPUT},
    {"InputPullup", TOKEN_INPUT_PULLUP},
    {"InputPulldown", TOKEN_INPUT_PULLDOWN},
    
    // Bus types
    {"I2C", TOKEN_I2C},
    {"SPI", TOKEN_SPI},
    {"CAN", TOKEN_CAN},
    {"UART", TOKEN_UART},
    
    {NULL, TOKEN_EOF}
};

void lexer_init(lexer_t *lexer, const char *source, const char *filename) {
    lexer->source = source;
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
    lexer->column = 1;
    lexer->filename = filename;
}

static bool is_at_end(lexer_t *lexer) {
    return *lexer->current == '\0';
}

static char advance(lexer_t *lexer) {
    lexer->current++;
    lexer->column++;
    return lexer->current[-1];
}

static char peek(lexer_t *lexer) {
    return *lexer->current;
}

static char peek_next(lexer_t *lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->current[1];
}

static bool match(lexer_t *lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    lexer->current++;
    lexer->column++;
    return true;
}

static token_t make_token(lexer_t *lexer, token_type_t type) {
    token_t token;
    token.type = type;
    token.start = lexer->start;
    token.length = (size_t)(lexer->current - lexer->start);
    token.line = lexer->line;
    token.column = lexer->column - (int)token.length;
    return token;
}

static token_t error_token(lexer_t *lexer, const char *message) {
    token_t token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = strlen(message);
    token.line = lexer->line;
    token.column = lexer->column;
    return token;
}

static void skip_whitespace(lexer_t *lexer) {
    for (;;) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;
            case '\n':
                // Don't skip newlines - they might be significant
                return;
            case '/':
                if (peek_next(lexer) == '/') {
                    // Line comment
                    while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                        advance(lexer);
                    }
                } else if (peek_next(lexer) == '*') {
                    // Block comment
                    advance(lexer); // /
                    advance(lexer); // *
                    while (!is_at_end(lexer)) {
                        if (peek(lexer) == '*' && peek_next(lexer) == '/') {
                            advance(lexer); // *
                            advance(lexer); // /
                            break;
                        }
                        if (peek(lexer) == '\n') {
                            lexer->line++;
                            lexer->column = 0;
                        }
                        advance(lexer);
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static token_type_t check_keyword(const char *start, size_t length) {
    for (int i = 0; keywords[i].text != NULL; i++) {
        if (strlen(keywords[i].text) == length &&
            memcmp(start, keywords[i].text, length) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static token_t identifier(lexer_t *lexer) {
    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }
    
    token_type_t type = check_keyword(lexer->start, 
                                      (size_t)(lexer->current - lexer->start));
    return make_token(lexer, type);
}

static token_t number(lexer_t *lexer) {
    while (isdigit(peek(lexer))) {
        advance(lexer);
    }
    
    // Decimal part
    if (peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        advance(lexer); // .
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
    }
    
    return make_token(lexer, TOKEN_NUMBER);
}

static token_t string(lexer_t *lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') {
            lexer->line++;
            lexer->column = 0;
        }
        advance(lexer);
    }
    
    if (is_at_end(lexer)) {
        return error_token(lexer, "Unterminated string");
    }
    
    advance(lexer); // Closing "
    return make_token(lexer, TOKEN_STRING);
}

token_t lexer_next_token(lexer_t *lexer) {
    skip_whitespace(lexer);
    
    lexer->start = lexer->current;
    
    if (is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }
    
    char c = advance(lexer);
    
    if (isalpha(c) || c == '_') {
        return identifier(lexer);
    }
    
    if (isdigit(c)) {
        return number(lexer);
    }
    
    switch (c) {
        case '(': return make_token(lexer, TOKEN_LEFT_PAREN);
        case ')': return make_token(lexer, TOKEN_RIGHT_PAREN);
        case '{': return make_token(lexer, TOKEN_LEFT_BRACE);
        case '}': return make_token(lexer, TOKEN_RIGHT_BRACE);
        case '[': return make_token(lexer, TOKEN_LEFT_BRACKET);
        case ']': return make_token(lexer, TOKEN_RIGHT_BRACKET);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case ';': return make_token(lexer, TOKEN_SEMICOLON);
        case '+': return make_token(lexer, TOKEN_PLUS);
        case '-': 
            if (match(lexer, '>')) {
                return make_token(lexer, TOKEN_ARROW);
            }
            return make_token(lexer, TOKEN_MINUS);
        case '*': return make_token(lexer, TOKEN_STAR);
        case '/': return make_token(lexer, TOKEN_SLASH);
        case '%': return make_token(lexer, TOKEN_PERCENT);
        case '@': return make_token(lexer, TOKEN_AT);
        case '!':
            return make_token(lexer, match(lexer, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return make_token(lexer, match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return make_token(lexer, match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return make_token(lexer, match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '&':
            if (match(lexer, '&')) {
                return make_token(lexer, TOKEN_AMP_AMP);
            }
            break;
        case '|':
            if (match(lexer, '|')) {
                return make_token(lexer, TOKEN_PIPE_PIPE);
            }
            break;
        case ':':
            if (match(lexer, ':')) {
                return make_token(lexer, TOKEN_DOUBLE_COLON);
            }
            return make_token(lexer, TOKEN_COLON);
        case '"':
            return string(lexer);
        case '\n':
            lexer->line++;
            lexer->column = 1;
            return make_token(lexer, TOKEN_NEWLINE);
    }
    
    return error_token(lexer, "Unexpected character");
}

const char *token_type_to_string(token_type_t type) {
    switch (type) {
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_ROBOT: return "ROBOT";
        case TOKEN_MOTOR: return "MOTOR";
        case TOKEN_TASK: return "TASK";
        case TOKEN_SCHEDULE: return "SCHEDULE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void token_print(token_t *token) {
    printf("[%d:%d] %s: '%.*s'\n",
           token->line,
           token->column,
           token_type_to_string(token->type),
           (int)token->length,
           token->start);
}

bool token_equals(token_t *token, const char *text) {
    return strlen(text) == token->length &&
           memcmp(token->start, text, token->length) == 0;
}

char *token_to_string(token_t *token) {
    char *str = NEUROX_MALLOC(token->length + 1);
    memcpy(str, token->start, token->length);
    str[token->length] = '\0';
    return str;
}
