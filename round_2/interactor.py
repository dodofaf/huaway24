import argparse
import tempfile
import os
import subprocess
import threading

print_details = None

def make_fifo(name):
    fifo_path = os.path.join(tempfile.gettempdir(), f"fifo_pipe_{name}")
    if os.path.exists(fifo_path):
        os.remove(fifo_path)
    os.mkfifo(fifo_path)
    return fifo_path

def run_checker(checker_executable, solution_executable, input_file, checker_log, solution_timeout):
    pipe_checker_to_solution = make_fifo("checker_to_solution")
    pipe_solution_to_checker = make_fifo("solution_to_checker")
     
    checker_args = checker_executable.strip().split(' ') + [pipe_checker_to_solution, pipe_solution_to_checker, "0", "0", checker_log]
    solution_args = solution_executable.strip().split(' ')

    print_details()
    print_details(f"Starting checker. Command: <{' '.join(checker_args)}>.")
    checker_process = subprocess.Popen(checker_args, stdin=open(input_file, "r"), stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
 
    with open(pipe_checker_to_solution, 'r') as pipe_checker_to_solution_r:
        with open(pipe_solution_to_checker, 'w') as pipe_solution_to_checker_w:
            print_details(f"Starting solution. Command: <{' '.join(solution_args)}>.")
            solution_process = subprocess.Popen(solution_args, stdin=pipe_checker_to_solution_r, stdout=pipe_solution_to_checker_w, stderr=subprocess.PIPE, text=True)
     
    try:
        print_details()
        print_details(f"Waiting for solution and checker to communicate.")
        solution_process.communicate(timeout=solution_timeout)
        out, err = checker_process.communicate(timeout=solution_timeout)
        if solution_process.returncode != 0:
            print(f"Solution exit code: {solution_process.returncode}")
        if len(out.strip()) > 0:
            print_details(f"Checker returned an error: ", end="")
            print(out.strip())
            return False
    except subprocess.TimeoutExpired as e:
        checker_process.kill()
        solution_process.kill()
        checker_process.communicate()
        solution_process.communicate()
        print_details(f"Solution and checker were running for over {solution_timeout} seconds. Terminating.")
        return False

    print_details(f"Solution was checked successfully! Checker log saved to {checker_log}.")
    return True


def run_scorer(scorer_executable, input_file, checker_log):
    scorer_args = scorer_executable.strip().split(' ') + [input_file, checker_log]

    print_details()
    print_details(f"Starting scorer. Command: <{' '.join(scorer_args)}>. ")
    scorer_process = subprocess.Popen(scorer_args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    stdout, stderr = scorer_process.communicate()
    print_details(f"Scorer output: ", end="")
    print(stdout.strip())

def run_generator(generator_executable, generator_seed, test_file):
    generator_args = generator_executable.strip().split(' ') + [str(generator_seed)]
    print_details(f"Starting generator with seed={generator_seed}. Command: <{' '.join(generator_args)}>")
    generator_process = subprocess.Popen(generator_args, stdout=open(test_file, "w"), text=True)
    generator_process.communicate()
    print_details(f"Generator provided output to file {test_file}")

def test_solution(
        solution,
        test_file,
        *,
        time_limit=30,
        checker="./checker",
        scorer="./scorer",
        generator="./generator",
        seed=None,
        results_only=False,
        ):
    global print_details
    print_details = (lambda *a, **k: None) if results_only else print

    checker_out_file = tempfile.NamedTemporaryFile(prefix="checker_log_")

    if seed is not None:
        run_generator(generator, seed, test_file)
    else:
        print_details("No generator seed provided. Skipping the test generaton step.")

    if run_checker(checker, solution, test_file, checker_out_file.name, time_limit):
        run_scorer(scorer, test_file, checker_out_file.name)

def main():
    parser = argparse.ArgumentParser(description="This is an interactor that allows generating tests, checking and scoring your solutions to the \"HOT 2024 - Round 2\" problem. "
                                                 "For all the sources, please, use the path to the executable or command, used to run sources. For example, './executable', 'java executable' or 'python3 source_code.py'. "
                                                 "You should compile all sources manually before running this interactor!"
                                                 , epilog="Good luck!")
    
    parser.add_argument("--solution",       required=True,           help='Path to the solution executable. (i.e: \'./executable\', \'java executable\' or \'python3 source_code.py\')')
    parser.add_argument("--test_file",      required=True,           help='Path to the test input file. Will be overwritten if the generator seed is specified.')
    parser.add_argument("--time_limit",     default = 30, type=int,  help='How long the solution is allowed to run (in seconds) before it will be terminated.')
    parser.add_argument("--checker",        default = "./checker",   help='Path to the checker executable (default: \'%(default)s\').')
    parser.add_argument("--scorer",         default = "./scorer",    help='Path to the scorer executable (default: \'%(default)s\').')
    parser.add_argument("--generator",      default = "./generator", help='Path to the generator executable (default: \'%(default)s\').')
    parser.add_argument("--seed",           type=int,                help='Seed, passed to the generator. Generator will not be executed without seed specified.')
    parser.add_argument("--results-only",   action="store_true",     help="Print only the verdict of the judge")
    args = parser.parse_args()

    test_solution(
        args.solution,
        args.test_file,
        generator=args.generator,
        seed=args.seed,
        checker=args.checker,
        time_limit=args.time_limit,
        scorer=args.scorer,
        results_only=args.results_only,
    )

if __name__ == "__main__":
    main()

