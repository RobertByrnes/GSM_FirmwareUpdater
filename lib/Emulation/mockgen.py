#!/usr/bin/python3
import os, re, sys, getopt

def main(argv):
    """Runtime

    Args:
        argv (list): command line arguments
    """                     
    mockClassName = ''
    mockType = ''
    opts, args = getopt.getopt(argv,"hc:t:",["class=","type="])
    for opt, arg in opts:
        if opt == '-h':
            printHelp()
            sys.exit()
        elif opt in ("-c", "--class"):
            mockClassName = arg
        elif opt in ("-t", "--type"):
            mockType = arg

    if mockClassName != '':
        createMock(mockClassName, mockType)  


def printHelp():
    """Prints help message
    """
    print("*** Class names should be in 'PascalCase' ***\n")
    print("mockgen.py -c <className>\n")
    print("mockgen.py -c <className> -t <type>\n")


def createMock(className, type):
    """Creates the mock stub

    Args:
        className (string): The PascalCase class name for the MockPascalCase class to be generated
        type (string): Optionally, a type maybe given // TODO allow creation of Struct stub
    """
    nameParts = re.sub( r"([A-Z])", r" \1", className).split()
    define = 'MOCK'
    className = 'Mock' + className

    for part in nameParts:
        define += '_' + part.upper()

    define += '_H'
    mockClassStub = open(os.getcwd() + '/lib/Emulation/Mocks/' + className + '.hpp', 'w')

    Lines = [
        "#if not defined(" + define + ")\n",
        "#define " + define + "\n",
        "\n",
        "#include <Arduino.h>\n",
        "#include <Emulator.h>\n",
        "\n",
        "class " + className + " : public Emulator {\n",
        "public:\n",
        "\t" + className + "() {}\n",
        "\t~" + className + "() {}\n",
        "\n",
        "protected:",
        "\n",
        "\n",
        "private:",
        "\n",
        "\n",
        "};\n",
        "\n",
        "#endif\n"
    ]

    mockClassStub.writelines(Lines) 
    mockClassStub.close()

    print("Mock stub generated...")

if __name__ == "__main__":
   main(sys.argv[1:])
