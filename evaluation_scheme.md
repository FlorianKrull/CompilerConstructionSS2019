# Final Submission Evaluation Scheme

Each checkbox represents 1 point to score.
The following key is used for calculating the resulting grade:

- **1:** ≥ 92%
- **2:** (92%, 84%]
- **3:** (84%, 76%]
- **4:** (76%, 68%]
- **5:** < 68%

It is required that for the *mandelbrot* test input, a respective executable can be built and run successfully.

Points *may* be subtracted for shortcomings not explicitly listed in this form.
This includes things like:

- Encountered issues not mentioned or justified in the *Known Issues* section
- Executables *segfaulting*
- Faulty code that is not even tested
- Overcomplicated build system setups
- Intermixing interfaces and implementation details
- Misuse of assertions
- Unnecessary waste of time or space (memory leaks)
- Inconsistently formatted or unreadable source code
- …

## Boundary Conditions

- [ ] Correct submission
    - Subject is correct
    - Attached file has correct name and structure

- [ ] README is present
    - Contains instructions
    - Contains dependencies
    - Contains *Known Issues*

- [ ] Code builds successfully
    - Warnings are enabled
    - No unjustified warnings of any kind

- [ ] All unit tests succeed

- [ ] All integration tests succeed
    - provided test inputs must be included

- [ ] Additional integration tests (provided by the instructor) succeed

- [ ] Architecture consists of shared library + executables

- [ ] All symbols exported by the library are prefixed with `mcc_`

## Front-end

Errors need to come with a meaningful error message and source location information (filename, start line, and start column).

- Syntactic checks:
    - [ ] Syntactically invalid mC programs are rejected with an error

    - [ ] AST data structure is present and instantiated by the parser

    - [ ] AST can be visualised using `mc_ast_to_dot`

- Semantic checks:
    - [ ] Shadowing is supported correctly

    - [ ] Error on use of undeclared variable

    - [ ] Error on conflicting variable declaration

    - [ ] Error on use of unknown function

    - [ ] Error on missing `main` function

    - [ ] Error on conflicting function names
        - includes built-in functions

    - [ ] Error on missing return-statement for non-void functions

    - [ ] Correct type checking on scalars

    - [ ] Correct type checking on arrays

    - [ ] Error on invalid call-expressions
        - Mismatching argument count
        - Mismatching argument types
        - Return type is taken into account by the type checker

    - [ ] Symbol table data structure is present

    - [ ] Symbol table can be visualised using `mc_symbol_table`

    - [ ] Type checking can be traced using `mc_type_check_trace`

## Core

- [ ] TAC data structure is present

- [ ] TAC can be visualised using `mc_ir`

- [ ] CFG data structure is present

- [ ] CFG can be visualised using `mc_cfg_to_dot`

## Back-end

- [ ] Assembly code can be obtained using `mc_asm`

- [ ] GCC is invoked to generate the final executable

## Driver

- [ ] `mcc` executable supports the requested command-line flags

- [ ] Multiple input files are supported
