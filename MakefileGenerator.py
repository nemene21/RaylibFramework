import tkinter, subprocess, os

CFLAGS   = "-O1 -Wall -Wno-missing-braces -I../src/game/ -I../src/framework/components/ -I../include/ -I../src/ -I../src/framework/ -I../src/framework/objects/ -I../src/framework/entities/"
LDFLAGS  = "-L../lib/"
LDLIBS   = "-lraylib -lopengl32 -lgdi32 -lwinmm"
OBJ_DIR  = "object_files"
SRC_DIR  = "../src"
ROOT_DIR = ".."
COMPILER = "g++"

makefile = ""

def next(n=1):
    global makefile
    for i in range(n):
        makefile += "\n"

def tab():
    global makefile
    makefile += "\t"

def add(cont):
    global makefile
    makefile += cont

def get_source_list(dir_path):
    source_files = []
    dir = os.fsencode(dir_path)

    for file in os.listdir(dir):
        filename = os.fsdecode(file)

        if filename.endswith(".cpp"):
            source_files.append(f"{dir_path}/{filename.split('.')[0]}")

        elif not "." in filename:
            source_files += get_source_list(f"{dir_path}/{filename}")
    
    return source_files

def generate_makefile():
    global makefile

    print("Starting...")
    source_files = get_source_list("src")
    print("Constructed Source List...")

    makefile = ""

    add("all: Build.exe")
    next(2)

    for source in source_files:
        add(f"{OBJ_DIR}/{get_filename(source)}.o: {ROOT_DIR}/{source}.cpp")
        next(); tab()

        add(f"{COMPILER} {CFLAGS} -c {ROOT_DIR}/{source}.cpp -o $(@)")
        next(2)

    add("Build.exe: ")
    for source in source_files:
        add(OBJ_DIR + "/" + get_filename(source) + ".o ")

    next(); tab()
    add(f"{COMPILER} -o Build.exe ")
    for source in source_files:
        add(OBJ_DIR + "/" + get_filename(source) + ".o ")

    add(LDFLAGS + " " + LDLIBS)

    next(2)
    add(".PHONY: clean")
    next()
    add("clean:")
    next()

    for source in source_files:
        tab()
        add("del /S /Q " + OBJ_DIR + "\\" + get_filename(source) + ".o")
        next()

    print("Generated...")

    file = open("build/Makefile", "w")
    file.write(makefile)
    file.close()

    print("Saved...\n")

def make():
    result = subprocess.run("make", shell=True, cwd="build")
    print("Make done?")
    print("")

def get_filename(string):
    return string.split("/")[-1]

def run():
    result = subprocess.run("Build.exe", shell=True, cwd="build") 

    if result:
        print("Game Sucessfully Ran!")
    else:
        print("Game didnt' Run :(")
    print("")

def clean():
    result = subprocess.run("make clean", shell=True, cwd="build")

    if result:
        print("Cleaned!")
    else:
        print("Cleanedn't :(")
    print("")

window = tkinter.Tk()
window.configure(bg='#333333')
window.geometry("512x512")
window.title("Auto Compiling and Running!")

font = ("Helvetica", 24)

generate_button = tkinter.Button(window, text="Generate Makefile", command=generate_makefile, font=font)
generate_button.place(x = 16, y = 16)

make_button = tkinter.Button(window, text="Make", command=make, font=font)
make_button.place(x = 16, y = 16 + 80)

run_button = tkinter.Button(window, text="Run", command=run, font=font)
run_button.place(x = 16, y = 16 + 160)

clean_button = tkinter.Button(window, text="Clean", command=clean, font=font)
clean_button.place(x = 16, y = 16 + 418)

window.mainloop()