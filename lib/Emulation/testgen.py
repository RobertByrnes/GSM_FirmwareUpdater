#!/usr/bin/python3
import os, re, sys, getopt

def main(argv):
    """Runtime

    Args:
        argv (list): command line arguments
    """
    testName = ''
    subDir = ''
    subSubDir = ''
    opts, args = getopt.getopt(argv,"ht:s:S:",["testName=","subDir=", "subSubDir="])
    for opt, arg in opts:
        if opt == '-h':
            printHelp()
            sys.exit()
        elif opt in ("-t", "--testName"):
            testName = arg
        elif opt in ("-s", "--subDir"):
            subDir = arg
        elif opt in ("-S", "--subSubDir"):
            subSubDir = arg

    createTest(testName, subDir, subSubDir)  


def printHelp():
    """Prints help message
    """
    print("*** Tests names with spaces should be wrapped in \"\" ***\n")
    print("*** We will add the word test for you :) ***\n")
    print("testgen.py -t <testName>\n")
    print("testgen.py -t <testName> -s <subDir>\n")
    print("testgen.py -t <testName> -s <subDir> -S <subSubDir>\n")


def createTest(testName, subDir, subSubDir):
    """Creates the test stub file based on \
    user input

    Args:
        testName (string): the name to use as a file name
        subDir (string): the directory directly under test/
        subSubDir (string): the directory under test/subDir/

    Returns:
        None:
    """
    testNameParts = re.sub( r"(\s)", r" \1", testName).split()
    testName = 'test'

    for part in testNameParts:
        testName += '_' + part.lower()
    
    subDirExists = os.path.exists(os.getcwd() + '/test/' + subDir)
    if not subDirExists:
        os.mkdir(os.getcwd() + '/test/' + subDir)

    subSubDirExists = os.path.exists(os.getcwd() + '/test/' + subDir + '/' + subSubDir)
    if not subSubDirExists:
        os.mkdir(os.getcwd() + '/test/' + subDir + '/' + subSubDir)

    fileExists = os.path.exists(os.getcwd() + '/test/' + testName + 'cpp')
    fileExistsWithSubDir = os.path.exists(os.getcwd() + '/test/' + subDir + testName + '.cpp')
    fileExistsWithSubSubDir = os.path.exists(os.getcwd() + '/test/' + subDir + '/' + subSubDir + '/' + testName + '.cpp')

    if (len(subDir) > 0 and len(subSubDir) > 0 and not fileExistsWithSubSubDir):
        print("If one")
        testStub = open(os.getcwd() + '/test/' + subDir + '/' + subSubDir + '/' + testName + '.cpp', 'w')
    elif (len(subDir) > 0 and len(subSubDir) == 0 and not fileExistsWithSubDir):
        print("If two")
        testStub = open(os.getcwd() + '/test/' + subDir + '/' + testName + '.cpp', 'w')
    elif (not fileExists and len(subDir) == 0 and len(subSubDir) == 0):
        print("If three")
        testStub = open(os.getcwd() + '/test/' + testName + '.cpp', 'w')
    else:
        print("Check path does not already exists")
        sys.exit()

    Lines = [
        "#include <emulation.h>\n",
        "\n",
        "void setUp(void) {}\n",
        "\n",
        "void tearDown(void) {}\n",
        "\n",
        "void runTests() {\n",
        "\tUNITY_BEGIN();\n",
        "\t// TODO add tests\n",
        "\tUNITY_END();\n",
        "}\n",
        "\n",
        "#if defined(ARDUINO)\n",
        "#include <Arduino.h>\n",
        "\n",
        "void setup() {\n",
        "\trunTests();\n",
        "}\n",
        "\n",
        "void loop() {}\n",
        "\n",
        "#else\n"
        "\n",
        "int main(int argc, char **argv) {\n",
        "\trunTests();\n",
        "\treturn 0;\n",
        "}\n",
        "\n",
        "#endif\n"
    ]

    testStub.writelines(Lines) 
    testStub.close()

    print("Test stub generated...")


if __name__ == "__main__":
   main(sys.argv[1:])
