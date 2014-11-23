import os
import os.path
import sys

join = os.path.join

#Tell Scons to rebuild files whose timestamp changes
Decider("timestamp-match")

Command("lib", [], Mkdir("lib"))
Command("bin", [], Mkdir("bin"))

BASE = os.getcwd()

LIBPATH = [join(BASE, "lib")]
LIBS = ["m", "parser", "parser_util"]

print("BASE={}".format(BASE))

INCLUDES = ["parser", "ptree", "util"]
INCLUDES = map(lambda i: join(BASE, "src", i), INCLUDES)

env = Environment(ENV=os.environ, CCFLAGS="-std=c1x -Wall -Werror", CPPPATH=INCLUDES)

debug = ARGUMENTS.get("debug", 0)
if int(debug):
    env.Append(CCFLAGS = " -g")

SConscript([join("src", "SConstruct")], exports="env BASE LIBS LIBPATH")

demos = ARGUMENTS.get("demos", 0)
if int(demos):
	SConscript([join("src", "demos", "SConstruct")], exports="env BASE LIBS LIBPATH")

install = ARGUMENTS.get("install", "")
if install != "":
	lib = install
	include = install
	LIBEXTENSION = ""
	if sys.platform=="win32":
		lib = join(lib, "VC", "Lib")
		include = join(include, "VC", "Include")
		LIBEXTENSION = "lib"
	else:
		lib = join(lib, "lib")
		include = join(include, "include")
		LIBEXTENSION = "a"
	HEADERS = [join("src", "parser", "parser.h"), join("src", "ptree", "ptree.h")]
	HEADERS += Glob(join("src", "util", "*.h"))
	Install(lib, Glob(join("lib", "*."+LIBEXTENSION)))
	Install(include, HEADERS)

