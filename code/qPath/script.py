import subprocess
import os


def compile_and_run(input_file):
    try:
        subprocess.run(["g++", "--std=c++17", "qPath/qPath.cpp", "-o", "output/qPath"])
        subprocess.run(["g++", "--std=c++17", "qPath/qLeap.cpp", "-o", "output/qLeap"])
        subprocess.run(
            ["g++", "--std=c++17", "twoState/RLBSP/RLBSP.cpp", "-o", "output/RLBSP"]
        )
        subprocess.run(["./output/qPath", input_file])
        subprocess.run(["./output/qLeap", input_file])
        subprocess.run(["./output/RLBSP", input_file])

    except Exception as e:
        print(f"Error occurred: {e}")


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print(
            "Usage: python3 run_cpp.py <input_file>"
        )
        sys.exit(1)

    input_file = sys.argv[1]

    compile_and_run(input_file)
