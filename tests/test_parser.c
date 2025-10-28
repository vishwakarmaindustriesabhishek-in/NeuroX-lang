#include "../compiler/lexer.h"
#include "../compiler/parser.h"
#include "../compiler/ast.h"
#include <assert.h>
#include <stdio.h>

void test_parse_minimal() {
    const char *source = 
        "robot TestBot {\n"
        "  motor m1 on M1\n"
        "}";
    
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    parser_t parser;
    parser_init(&parser, &lexer);
    
    ast_robot_t *robot = parser_parse(&parser);
    
    assert(robot != NULL);
    assert(strcmp(robot->name, "TestBot") == 0);
    assert(robot->decl_count == 1);
    assert(robot->declarations[0]->type == DECL_MOTOR);
    
    ast_robot_free(robot);
    printf("✓ Minimal robot parse test passed\n");
}

void test_parse_task() {
    const char *source = 
        "robot TestBot {\n"
        "  task move(speed: Percent) {\n"
        "    wait(100ms)\n"
        "  }\n"
        "}";
    
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    parser_t parser;
    parser_init(&parser, &lexer);
    
    ast_robot_t *robot = parser_parse(&parser);
    
    assert(robot != NULL);
    assert(robot->decl_count == 1);
    assert(robot->declarations[0]->type == DECL_TASK);
    
    ast_decl_t *task_decl = robot->declarations[0];
    assert(strcmp(task_decl->as.task.name, "move") == 0);
    assert(task_decl->as.task.param_count == 1);
    
    ast_robot_free(robot);
    printf("✓ Task parse test passed\n");
}

void test_parse_schedule() {
    const char *source = 
        "robot TestBot {\n"
        "  schedule main @ 10Hz priority HIGH {\n"
        "    stop()\n"
        "  }\n"
        "}";
    
    lexer_t lexer;
    lexer_init(&lexer, source, "test");
    
    parser_t parser;
    parser_init(&parser, &lexer);
    
    ast_robot_t *robot = parser_parse(&parser);
    
    assert(robot != NULL);
    assert(robot->decl_count == 1);
    assert(robot->declarations[0]->type == DECL_SCHEDULE);
    
    ast_decl_t *sched_decl = robot->declarations[0];
    assert(strcmp(sched_decl->as.schedule.name, "main") == 0);
    assert(sched_decl->as.schedule.priority == PRIORITY_HIGH);
    
    ast_robot_free(robot);
    printf("✓ Schedule parse test passed\n");
}

int main() {
    printf("Running parser tests...\n");
    
    test_parse_minimal();
    test_parse_task();
    test_parse_schedule();
    
    printf("\n✓ All parser tests passed!\n");
    return 0;
}
