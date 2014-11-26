import os
import os.path
import sys

join = os.path.join

BASE = os.getcwd()

LIBPATH = [join(BASE, "lib")]
LIBS = ["m", "parser", "parser_util"]
LIBNAME = "lib{name}.a"
EXENAME = "{name}"
WERROR = "-Werror "
STD = "-std=c1x "
WALL = "-Wall "
if sys.platform == "win32":
	LIBS = ["parser", "parser_util"]
	LIBNAME = "{name}.lib"
	EXENAME = "{name}.exe"
	WERROR = ""
	STD = ""

#Tell Scons to rebuild files whose timestamp changes
Decider("timestamp-match")

Command("lib", [], Mkdir("lib"))
Command("bin", [], Mkdir("bin"))

print("BASE={}".format(BASE))

INCLUDES = ["parser", "ptree", "util"]
INCLUDES = map(lambda i: join(BASE, "src", i), INCLUDES)

env = Environment(ENV=os.environ, CCFLAGS=STD+WALL+WERROR, CPPPATH=INCLUDES)

debug = ARGUMENTS.get("debug", 0)
if int(debug):
    env.Append(CCFLAGS = " -g")

SConscript([join("src", "SConstruct")], exports="env BASE LIBS LIBPATH LIBNAME EXENAME")

demos = ARGUMENTS.get("demos", 0)
if int(demos):
	SConscript([join("src", "demos", "SConstruct")], exports="env BASE LIBS LIBPATH EXENAME")

install = ARGUMENTS.get("install", "")
if install != "":
	lib = install
	include = install
	if sys.platform == "win32":
		lib = join(lib, "VC", "Lib")
		include = join(include, "VC", "Include")
	else:
		lib = join(lib, "lib")
		include = join(include, "include")
	HEADERS = [join("src", "parser", "parser.h"), join("src", "ptree", "ptree.h")]
	HEADERS += Glob(join("src", "util", "*.h"))
	Install(lib, Glob(join("lib", "*")))
	Install(include, HEADERS)

