import subprocess
import os


def compile_and_run(input_file, output_file_qPath, output_file_qLeap):
    try:
        subprocess.run(["g++", "--std=c++17", "qPath/qPath.cpp", "-o", "output/qPath"])
        subprocess.run(["g++", "--std=c++17", "qPath/qLeap.cpp", "-o", "output/qLeap"])
        subprocess.run(
            ["g++", "--std=c++17", "twoState/RLBSP/RLBSP.cpp", "-o", "output/RLBSP"]
        )
        output = "output/"
        output_qPath = output + output_file_qPath
        output_qLeap = output + output_file_qLeap

        with open(output_qPath, "w") as outfile_qPath:
            subprocess.run(["./output/qPath", input_file], stdout=outfile_qPath)
        with open(output_qLeap, "w") as outfile_qLeap:
            subprocess.run(["./output/qLeap", input_file], stdout=outfile_qLeap)
        subprocess.run(["./output/RLBSP", input_file])

    except Exception as e:
        print(f"Error occurred: {e}")


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 4:
        print(
            "Usage: python3 run_cpp.py <input_file> <output_file_for_qPath> <output_file_for_qLeap>"
        )
        sys.exit(1)

    input_file = sys.argv[1]
    output_file_qPath = sys.argv[2]
    output_file_qLeap = sys.argv[3]

    compile_and_run(input_file, output_file_qPath, output_file_qLeap)
