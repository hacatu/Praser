import os
import os.path

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

env = Environment(CCFLAGS="-std=c1x -Wall -Werror", CPPPATH=INCLUDES)

debug = ARGUMENTS.get("debug", 0)
if int(debug):
    env.Append(CCFLAGS = " -g")

SConscript([join("src", "SConstruct")], exports="env BASE LIBS LIBPATH")

demos = ARGUMENTS.get("demos", 0)
if int(demos):
	SConscript([join("src", "demos", "SConstruct")], exports="env BASE LIBS LIBPATH")

install = ARGUMENTS.get("install", "")
if install != "":
	Install(install, Glob(join("lib", "*.a")))

