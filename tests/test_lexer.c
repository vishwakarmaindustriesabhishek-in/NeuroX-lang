#include "../compiler/lexer.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_keywords() {
    const char *source = "robot motor task schedule";
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    token_t tok1 = lexer_next_token(&lexer);
    assert(tok1.type == TOKEN_ROBOT);
    
    token_t tok2 = lexer_next_token(&lexer);
    assert(tok2.type == TOKEN_MOTOR);
    
    token_t tok3 = lexer_next_token(&lexer);
    assert(tok3.type == TOKEN_TASK);
    
    token_t tok4 = lexer_next_token(&lexer);
    assert(tok4.type == TOKEN_SCHEDULE);
    
    printf("✓ Keywords test passed\n");
}

void test_identifiers() {
    const char *source = "myRobot left_motor sensor123";
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    token_t tok1 = lexer_next_token(&lexer);
    assert(tok1.type == TOKEN_IDENTIFIER);
    assert(token_equals(&tok1, "myRobot"));
    
    token_t tok2 = lexer_next_token(&lexer);
    assert(tok2.type == TOKEN_IDENTIFIER);
    
    token_t tok3 = lexer_next_token(&lexer);
    assert(tok3.type == TOKEN_IDENTIFIER);
    
    printf("✓ Identifiers test passed\n");
}

void test_numbers() {
    const char *source = "42 3.14 100";
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    token_t tok1 = lexer_next_token(&lexer);
    assert(tok1.type == TOKEN_NUMBER);
    
    token_t tok2 = lexer_next_token(&lexer);
    assert(tok2.type == TOKEN_NUMBER);
    
    token_t tok3 = lexer_next_token(&lexer);
    assert(tok3.type == TOKEN_NUMBER);
    
    printf("✓ Numbers test passed\n");
}

void test_strings() {
    const char *source = "\"hello\" \"world\"";
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    token_t tok1 = lexer_next_token(&lexer);
    assert(tok1.type == TOKEN_STRING);
    
    token_t tok2 = lexer_next_token(&lexer);
    assert(tok2.type == TOKEN_STRING);
    
    printf("✓ Strings test passed\n");
}

void test_operators() {
    const char *source = "+ - * / == != < > <= >=";
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    assert(lexer_next_token(&lexer).type == TOKEN_PLUS);
    assert(lexer_next_token(&lexer).type == TOKEN_MINUS);
    assert(lexer_next_token(&lexer).type == TOKEN_STAR);
    assert(lexer_next_token(&lexer).type == TOKEN_SLASH);
    assert(lexer_next_token(&lexer).type == TOKEN_EQUAL_EQUAL);
    assert(lexer_next_token(&lexer).type == TOKEN_BANG_EQUAL);
    assert(lexer_next_token(&lexer).type == TOKEN_LESS);
    assert(lexer_next_token(&lexer).type == TOKEN_GREATER);
    assert(lexer_next_token(&lexer).type == TOKEN_LESS_EQUAL);
    assert(lexer_next_token(&lexer).type == TOKEN_GREATER_EQUAL);
    
    printf("✓ Operators test passed\n");
}

void test_comments() {
    const char *source = "robot // line comment\n/* block\ncomment */ motor";
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    token_t tok1 = lexer_next_token(&lexer);
    assert(tok1.type == TOKEN_ROBOT);
    
    token_t tok2 = lexer_next_token(&lexer);
    assert(tok2.type == TOKEN_MOTOR);
    
    printf("✓ Comments test passed\n");
}

int main() {
    printf("Running lexer tests...\n");
    
    test_keywords();
    test_identifiers();
    test_numbers();
    test_strings();
    test_operators();
    test_comments();
    
    printf("\n✓ All lexer tests passed!\n");
    return 0;
}
