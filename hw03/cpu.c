#include "cpu.h"
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(x) (void) (x)

struct arguments
{
    int32_t fst;
    int32_t snd;
};

struct operation_info
{
    int32_t num_of_args;
    bool (*operation_ptr)(struct cpu *, struct arguments *);
};

static bool operation_nop(struct cpu *cpu, struct arguments *arg);
static bool operation_halt(struct cpu *cpu, struct arguments *arg);
static bool operation_add(struct cpu *cpu, struct arguments *arg);
static bool operation_sub(struct cpu *cpu, struct arguments *arg);
static bool operation_mul(struct cpu *cpu, struct arguments *arg);
static bool operation_div(struct cpu *cpu, struct arguments *arg);
static bool operation_inc(struct cpu *cpu, struct arguments *arg);
static bool operation_dec(struct cpu *cpu, struct arguments *arg);
static bool operation_loop(struct cpu *cpu, struct arguments *arg);
static bool operation_movr(struct cpu *cpu, struct arguments *arg);
static bool operation_load(struct cpu *cpu, struct arguments *arg);
static bool operation_store(struct cpu *cpu, struct arguments *arg);
static bool operation_in(struct cpu *cpu, struct arguments *arg);
static bool operation_get(struct cpu *cpu, struct arguments *arg);
static bool operation_out(struct cpu *cpu, struct arguments *arg);
static bool operation_put(struct cpu *cpu, struct arguments *arg);
static bool operation_swap(struct cpu *cpu, struct arguments *arg);
static bool operation_push(struct cpu *cpu, struct arguments *arg);
static bool operation_pop(struct cpu *cpu, struct arguments *arg);

struct operation_info op_info[19] = {
    { 0, operation_nop },
    { 0, operation_halt },
    { 1, operation_add },
    { 1, operation_sub },
    { 1, operation_mul },
    { 1, operation_div },
    { 1, operation_inc },
    { 1, operation_dec },
    { 1, operation_loop },
    { 2, operation_movr },
    { 2, operation_load },
    { 2, operation_store },
    { 1, operation_in },
    { 1, operation_get },
    { 1, operation_out },
    { 1, operation_put },
    { 2, operation_swap },
    { 1, operation_push },
    { 1, operation_pop }
};

int32_t *cpuCreateMemory(FILE *program, size_t stackCapacity, int32_t **stackBottom)
{
    assert(program != NULL);
    assert(stackBottom != NULL);

    int32_t *mem = NULL;
    int32_t *old_mem;
    size_t alloc_size = 0;
    size_t size = 0;
    int32_t chr;
    int32_t counter = 0;
    int8_t tmp[4];

    while ((chr = fgetc(program)) != EOF) {
        if (size + 1 > alloc_size) {
            alloc_size += 1024;
            old_mem = mem;
            mem = realloc(mem, alloc_size * sizeof(int32_t));
            if (mem == NULL) {
                free(old_mem);
                return NULL;
            }
        }

        tmp[counter] = chr;
        counter++;
        if (counter == 4) {
            counter = 0;
            memcpy(mem + size, tmp, 4 * sizeof(int8_t));
            size++;
        }
    }

    if (counter != 0) {
        free(mem);
        return NULL;
    }

    size += stackCapacity;
    while (size > alloc_size) {
        alloc_size += 1024;
        old_mem = mem;
        mem = realloc(mem, alloc_size * sizeof(int32_t));
        if (mem == NULL) {
            free(old_mem);
            return NULL;
        }
    }
    *stackBottom = mem + alloc_size - 1;

    if (mem != NULL) {
        int32_t *mem_empty = mem + size - stackCapacity;
        memset(mem_empty, 0, (*stackBottom - mem_empty + 1) * sizeof(int32_t));
    }

    return mem;
}

void cpuCreate(struct cpu *cpu, int32_t *memory, int32_t *stackBottom, size_t stackCapacity)
{
    assert(cpu != NULL);
    assert(memory != NULL);
    assert(stackBottom != NULL);

    memset(cpu, 0, sizeof(struct cpu));

    cpu->memory = memory;
    cpu->stackBottom = stackBottom;
    cpu->stackLimit = stackBottom - stackCapacity;
}

void cpuDestroy(struct cpu *cpu)
{
    assert(cpu != NULL);

    free(cpu->memory);
    memset(cpu, 0, sizeof(struct cpu));
}

void cpuReset(struct cpu *cpu)
{
    assert(cpu != NULL);

    cpu->A = 0;
    cpu->B = 0;
    cpu->C = 0;
    cpu->D = 0;
    cpu->status = 0;
    cpu->stackSize = 0;
    cpu->instructionPointer = 0;
    memset(cpu->stackLimit + 1, 0, (cpu->stackBottom - cpu->stackLimit) * sizeof(int32_t));
}

int cpuStatus(struct cpu *cpu)
{
    assert(cpu != NULL);

    return cpu->status;
}

int cpuStep(struct cpu *cpu)
{
    assert(cpu != NULL);

    if (cpuStatus(cpu) != cpuOK) {
        return 0;
    }

    int32_t ip = cpu->instructionPointer;

    if (cpu->memory + ip > cpu->stackLimit || cpu->memory + ip < cpu->memory) {
        cpu->status = cpuInvalidAddress;
        return 0;
    }

    int32_t instr = cpu->memory[ip];

    if (instr < 0 || instr > 18) {
        cpu->status = cpuIllegalInstruction;
        return 0;
    }

    struct arguments args;
    int32_t instr_num_of_args = op_info[instr].num_of_args;

    if (cpu->memory + ip + instr_num_of_args > cpu->stackLimit) {
        cpu->status = cpuInvalidAddress;
        return 0;
    }

    if (instr_num_of_args > 0) {
        args.fst = *(cpu->memory + ip + 1);
    }
    if (instr_num_of_args > 1) {
        args.snd = *(cpu->memory + ip + 2);
    }

    if (op_info[instr].operation_ptr(cpu, &args)) {
        return 1;
    }

    return 0;
}

int cpuRun(struct cpu *cpu, size_t steps)
{
    assert(cpu != NULL);

    if (cpuStatus(cpu) != cpuOK) {
        return 0;
    }

    int counter = 0;

    for (size_t i = 0; i < steps; i++) {
        counter++;
        if (cpuStep(cpu) == 0) {
            if (cpuStatus(cpu) == cpuHalted) {
                return counter;
            }
            return -counter;
        }
    }

    return counter;
}

int32_t cpuPeek(struct cpu *cpu, char reg)
{
    assert(cpu != NULL);

    switch (reg) {
    case 'A':
        return cpu->A;
    case 'B':
        return cpu->B;
    case 'C':
        return cpu->C;
    case 'D':
        return cpu->D;
    case 'S':
        return cpu->stackSize;
    case 'I':
        return cpu->instructionPointer;
    default:
        return 0;
    }
}

static int32_t *pointer_to_reg(struct cpu *cpu, int32_t reg)
{
    switch (reg) {
    case 0:
        return &cpu->A;
    case 1:
        return &cpu->B;
    case 2:
        return &cpu->C;
    case 3:
        return &cpu->D;
    default:
        cpu->status = cpuIllegalOperand;
        return NULL;
    }
}

static bool operation_nop(struct cpu *cpu, struct arguments *arg)
{
    UNUSED(arg);
    cpu->instructionPointer += 1;
    return true;
}

static bool operation_halt(struct cpu *cpu, struct arguments *arg)
{
    UNUSED(arg);
    cpu->status = cpuHalted;
    cpu->instructionPointer += 1;
    return false;
}

static bool operation_add(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    cpu->A += *reg_ptr;
    cpu->instructionPointer += 2;
    return true;
}

static bool operation_sub(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    cpu->A -= *reg_ptr;
    cpu->instructionPointer += 2;
    return true;
}

static bool operation_mul(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    cpu->A *= *reg_ptr;
    cpu->instructionPointer += 2;
    return true;
}

static bool operation_div(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    if (*reg_ptr != 0) {
        cpu->A /= *reg_ptr;
    } else {
        cpu->status = cpuDivByZero;
        return false;
    }

    cpu->instructionPointer += 2;
    return true;
}

static bool operation_inc(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    *reg_ptr += 1;
    cpu->instructionPointer += 2;
    return true;
}

static bool operation_dec(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    *reg_ptr -= 1;
    cpu->instructionPointer += 2;
    return true;
}

static bool operation_loop(struct cpu *cpu, struct arguments *arg)
{
    if (cpu->C != 0) {
        cpu->instructionPointer = arg->fst;
    } else {
        cpu->instructionPointer += 2;
    }

    return true;
}

static bool operation_movr(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    *reg_ptr = arg->snd;
    cpu->instructionPointer += 3;
    return true;
}

static bool operation_load(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    int32_t d_val = cpu->D;
    if (d_val + arg->snd < cpu->stackSize && d_val + arg->snd >= 0) {
        *reg_ptr = *(cpu->stackBottom - cpu->stackSize + 1 + d_val + arg->snd);
    } else {
        cpu->status = cpuInvalidStackOperation;
        return false;
    }

    cpu->instructionPointer += 3;
    return true;
}

static bool operation_store(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    int32_t d_val = cpu->D;
    if (d_val + arg->snd < cpu->stackSize && d_val + arg->snd >= 0) {
        *(cpu->stackBottom - cpu->stackSize + 1 + d_val + arg->snd) = *reg_ptr;
    } else {
        cpu->status = cpuInvalidStackOperation;
        return false;
    }

    cpu->instructionPointer += 3;
    return true;
}

static bool operation_in(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    int32_t input, ret_val;
    ret_val = scanf("%" SCNd32, &input);

    if (ret_val == 1) {
        *reg_ptr = input;
    } else if (ret_val == EOF) {
        cpu->C = 0;
        *reg_ptr = -1;
    } else {
        cpu->status = cpuIOError;
        return false;
    }

    cpu->instructionPointer += 2;
    return true;
}

static bool operation_get(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    int32_t input;

    if ((input = getchar()) != EOF) {
        *reg_ptr = input;
    } else {
        cpu->C = 0;
        *reg_ptr = -1;
    }

    cpu->instructionPointer += 2;
    return true;
}

static bool operation_out(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    printf("%" SCNd32, *reg_ptr);
    cpu->instructionPointer += 2;
    return true;
}

static bool operation_put(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    if (*reg_ptr >= 0 && *reg_ptr <= 255) {
        putchar(*reg_ptr);
    } else {
        cpu->status = cpuIllegalOperand;
        return false;
    }

    cpu->instructionPointer += 2;
    return true;
}

static bool operation_swap(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr_fst;
    if ((reg_ptr_fst = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    int32_t *reg_ptr_snd;
    if ((reg_ptr_snd = pointer_to_reg(cpu, arg->snd)) == NULL) {
        return false;
    }

    int32_t tmp = *reg_ptr_fst;
    *reg_ptr_fst = *reg_ptr_snd;
    *reg_ptr_snd = tmp;

    cpu->instructionPointer += 3;
    return true;
}

static bool operation_push(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    if ((cpu->stackBottom - cpu->stackSize) > cpu->stackLimit) {
        *(cpu->stackBottom - cpu->stackSize) = *reg_ptr;
        cpu->stackSize++;
    } else {
        cpu->status = cpuInvalidStackOperation;
        return false;
    }

    cpu->instructionPointer += 2;
    return true;
}

static bool operation_pop(struct cpu *cpu, struct arguments *arg)
{
    int32_t *reg_ptr;
    if ((reg_ptr = pointer_to_reg(cpu, arg->fst)) == NULL) {
        return false;
    }

    if (cpu->stackSize > 0) {
        *reg_ptr = *(cpu->stackBottom - cpu->stackSize + 1);
        cpu->stackSize--;
    } else {
        cpu->status = cpuInvalidStackOperation;
        return false;
    }

    cpu->instructionPointer += 2;
    return true;
}
