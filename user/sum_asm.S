.global main

.section .rodata
fmt_input:             .string "|%s|\n"
fmt_sum:               .string "%d\n"
error_many_spaces:     .string "Некорректный ввод!\n"
error_buffer_overflow: .string "Переполнение буфера!\n"

.section .text
main:
    addi sp, sp, -32
    mv a0, sp
    call gets
    mv s1, a0
    call strip_newline
    la a0, fmt_input
    mv a1, sp
    call printf
    li s2, 31
    li s3, 0
    mv a0, sp
    call cnt_chars
    bgt s3, s2, overflow_error
    mv a0, sp
    call find_first_space
    addi a0, a0, 1
    call single_space
    li s2, ' '
    mv a0, sp
    lb s4, 0(a0)
    beq s4, s2, format_error
    mv a0, sp
    call cnt_chars
    add a0, sp, s3
    lb s4, -1(a0)
    beq s4, s2, format_error
    call parse_first_number
    j finish

strip_newline:
    lb t0, 0(s1)
    beqz t0, sn_done
    li t1, '\n'
    beq t0, t1, sn_replace
    addi s1, s1, 1
    j strip_newline
sn_replace:
    sb zero, 0(s1)
sn_done:
    ret

cnt_chars:
    lb t0, 0(a0)
    beqz t0, cnt_end
    addi s3, s3, 1
    addi a0, a0, 1
    j cnt_chars
cnt_end:
    ret

find_first_space:
    lb t0, 0(a0)
    beqz t0, format_error
    li t1, ' '
    beq t0, t1, fs_found
    addi a0, a0, 1
    j find_first_space
fs_found:
    ret

single_space:
    lb t0, 0(a0)
    beqz t0, vs_done
    li t1, ' '
    beq t0, t1, format_error
    addi a0, a0, 1
    j single_space
vs_done:
    ret

parse_first_number:
    li s2, 0
    li s3, '-'
    mv a0, sp
    lb t0, 0(a0)
    beq t0, s3, first_negative
    call first_number
    ret

first_negative:
    li s2, 1
    addi a0, a0, 1
    call first_number
    sub a0, zero, a0
    ret

first_number:
    call atoi
    bne s2, zero, first_negate
    j first_proceed
first_negate:
    sub a0, zero, a0
first_proceed:
    j parse_second_number

parse_second_number:
    li s2, 0
    li s3, '-'
    mv s4, a0
    mv a0, sp
    call find_first_space
    addi a0, a0, 1
    lb t0, 0(a0)
    beq t0, s3, second_negative
    call second_number
    ret

second_negative:
    li s2, 1
    addi a0, a0, 1
    call second_number
    sub a0, zero, a0
    ret

second_number:
    call atoi
    bne s2, zero, second_negate
    j second_proceed
second_negate:
    sub a0, zero, a0
second_proceed:
    j do_sum

do_sum:
    mv s5, a0
    add a0, s4, s5
    mv a1, a0
    la a0, fmt_sum
    call printf
    li a0, 0
    call exit

format_error:
    la a0, error_many_spaces
    call printf
    li a0, 1
    call exit

overflow_error:
    la a0, error_buffer_overflow
    call printf
    li a0, 1
    call exit

finish:
    j exit_program

exit_program:
    li a0, 0
    call exit