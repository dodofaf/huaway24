all: generator scorer checker solution

clean:
	rm solution generator checker scorer

generator: generator.cpp
	g++ $< -O2 -o $@

checker: checker.cpp
	g++ $< -O2 -o $@

scorer: scorer.cpp
	g++ $< -O2 -o $@

solution: solution.cpp
	g++ $< -O2 -o $@

test_1: all
	python interactor.py --solution "./solution" --test_file test1 --seed 1
