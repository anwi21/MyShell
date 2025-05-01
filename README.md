// myshell
// Anwi Gundavarapu
// CPSC 3500 – Operating Systems
// 4/16/25
myshell is a simple command-line shell written in C++. It supports running up
to 10 commands connected by pipes (|) in a single input line. Each command can
have up to 20 tokens (arguments). The shell uses dynamic memory allocation to
manage pipes and creates separate processes for each command.

Strengths:
•       Supports pipelines of up to 10 commands.
•       Handles extra whitespace.
•       Uses multiprocessing to run commands concurrently.
•       Dynamically allocates pipes based on the number of commands.
Weakness:
•       Maximum input length: 4026 characters.
•       Maximum commands per pipeline: 10.
•       Maximum tokens per command: 20.
•       Only supports single-line input of commands.
