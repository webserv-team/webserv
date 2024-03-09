from sys import argv, exit
from datetime import datetime
import os

red = "\033[91m"
green = "\033[92m"
yellow = "\033[93m"
reset = "\033[0m" 

def colored_print(str, color):
    print(f"{color}{str}{reset}")

def get_class_name(str):
    words = str.split()
    name = ""
    for word in words:
        word = word[0].upper() + word[1:]
        name += word
    return name

def check_file_existance(file_name, override):
    if override:
        return False
    if os.path.exists(file_name):
        print(f"file {file_name} already exists")
        while True:
            try:
                answer = input("woud you like to overwrite it? [y/n]: ")
                if answer == "y":
                    break
                elif answer == "n":
                    colored_print(f"file {file_name} not created", red)
                    return True
                else:
                    continue
            except (KeyboardInterrupt, EOFError):
                exit(0)
    return False

def create_header_file(class_name, override):
    header_file_name = class_name + ".hpp"
    if check_file_existance(header_file_name, override):
        return
    with open(header_file_name, "w") as f:
        # f.write(return_header(header_file_name))
        f.write("\n\n#ifndef " + class_name.upper() + "_HPP\n")
        f.write("#define " + class_name.upper() + "_HPP\n\n")
        f.write("class " + class_name + "\n{\n")
        f.write("\tpublic:\n")
        f.write("\t\t" + class_name + "();\n")
        f.write("\t\t~" + class_name + "();\n")
        #copy constructor
        f.write("\t\t" + class_name + "(const " + class_name + "& other);\n")
        #copy assignment operator
        f.write("\t\t" + class_name + "& operator=(const " + class_name + "& other);\n")
        f.write("\tprivate:\n")
        f.write("};\n\n")
        f.write("#endif\n")
    colored_print(f"created: {header_file_name} successfully", green)

def create_source_file(class_name, override):
    source_file_name = class_name + ".cpp"
    header_file_name = class_name + ".hpp"
    if check_file_existance(source_file_name, override):
        return
    with open(source_file_name, "w") as f:
        # f.write(return_header(source_file_name))
        f.write("\n\n#include \"" + header_file_name + "\"\n\n")
        f.write(class_name + "::" + class_name + "()\n{\n\n}\n\n")
        f.write(class_name + "::~" + class_name + "()\n{\n\n}\n\n")
        #copy constructor
        f.write(class_name + "::" + class_name + "(const " + class_name + "& other)\n{\n\n}\n\n")
        #copy assignment operator
        f.write(class_name + "& " + class_name + "::operator=(const " + class_name + "& other)\n{\n\n}\n\n")
    colored_print(f"created: {source_file_name} successfully", green)

def create_main_file():
    main_file_name = "main.cpp"
    if not os.path.exists(main_file_name):
        with open(main_file_name, "w") as f:
            # f.write(return_header(main_file_name))
            f.write("\n\n#include <iostream>\n\n");
            f.write("int main()\n{\n\treturn (0);\n}\n")
        colored_print(f"created: {main_file_name} successfully", green)

if __name__ == "__main__":
    if len(argv) < 2:
        colored_print("Usage: python main.py [class_name]", yellow)
        exit(1)
    override = False
    start = 1
    if argv[start][0] == '-':
        if argv[start] in ["-f", "--force"]:
            override = True
            start = 2
        else:
            colored_print("invlid option: use -f or --force to force create the files", yellow)
            colored_print(f"example: python3 {__file__} -f/--force test1 test2 test3", yellow)
            exit(1)
    if argv[start] == "help":
        colored_print("Usage: python3 main.py [all calss names separated by space]", yellow)
        exit(0)

    for i in range(start, len(argv)):
        class_name = get_class_name(argv[i])
        if  not class_name[0].isalpha():
            colored_print("file not created, class name must start with a letter", red)
            continue
        if class_name == "":
            colored_print(f"file {class_name} not created, class name is empty", red)
            continue
        create_header_file(class_name, override)
        create_source_file(class_name, override)
    create_main_file()
