import subprocess

def compile_and_run(input_file, output_file_qPath, output_file_qLeap):
    try:
        subprocess.run(['g++', 'qPath.cpp', '-o', 'qPath'], check=True)
        subprocess.run(['g++', 'qLeap.cpp', '-o', 'qLeap'], check=True)
        
        with open(input_file, 'r') as infile:
            with open(output_file_qPath, 'w') as outfile_qPath:
                res = subprocess.run(['./qPath', input_file], stdout=outfile_qPath)
            infile.seek(0)  
            with open(output_file_qLeap, 'w') as outfile_qLeap:
                subprocess.run(['./qLeap', input_file], stdout=outfile_qLeap)
    except Exception as e:
        print(f"Error occurred: {e}")

if __name__ == '__main__':
    import sys
    if len(sys.argv) != 4:
        print("Usage: python3 run_cpp.py <input_file> <output_file_for_qPath> <output_file_for_qLeap>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file_qPath = sys.argv[2]
    output_file_qLeap = sys.argv[3]

    compile_and_run(input_file, output_file_qPath, output_file_qLeap)
