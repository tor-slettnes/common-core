#!/usr/bin/python3
#===============================================================================
## @file generate_errnos.py
## @brief Create mapping of error codes to symbolic names
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import re, sys, os, argparse

#includedir = "usr/include/asm-generic"
inputs = ("errno-base.h", "errno.h")


class ArgParser (argparse.ArgumentParser):

    def __init__ (self, *args, **kwargs):
        argparse.ArgumentParser.__init__(self, prog=sys.argv[0], *args, **kwargs)
        self.add_argument('--output', type=str, default="/dev/stdout",
                          help='generated output file')
        self.add_argument("--mapname", type=str, default="errno_names",
                          help='C++ variable name for generated map')
        self.add_argument('--sysroot', type=str, default='/',
                          help='system root in which to look for headers')
        self.add_argument('--includedir', type=str, default='usr/include/asm-generic',
                          help='system include directory, relative to sysroot')
        self.add_argument('--inputs', type=str, nargs='+', default=inputs);

class ErrorTable (dict):
    rx = re.compile(r"^\s*#\s*define\s+(\w+)"
                    r"\s+(\d+)"
                    r"\s*(?:(?:/\*\s*|//\s*)((?:.(?!\*/))+))?")

    cpp_header = (
        '// -*- c++ -*-',
        '// Do not edit. This file is autogenerated by %s'%(os.path.normpath(sys.argv[0]),),
        '',
        '#include <map>',
        '#include <string>',
        ''
    )

    def __init__ (self, includedir, paths):
        for p in paths:
            abspath = os.path.join(includedir, p)
            if os.path.isfile(abspath):
                with open(abspath) as fp:
                    self.import_from(fp)

    def import_from (self, fp):
        for line in fp:
            try:
                symbol, code, comment = self.rx.match(line).groups()
            except AttributeError:
                pass
            else:
                self[int(code)] = (symbol, comment)

    def save_to (self, path, mapname, commentcol=48, indent=4):
        with open(path, "w") as fp:
            fp.write('\n'.join(self.cpp_header) + '\n')
            fp.write('const std::map<unsigned int, std::string> %s =\n'
                     '{\n'%(mapname,))

            for index, code in enumerate(sorted(self)):
                sep  = ("", ",")[index < len(self)-1]
                symbol, comment = self[code]
                entry = '%s{%s, "%s"}%s'%(''.ljust(indent), code, symbol, sep)
                if comment:
                    entry = "%s// %s"%(entry.ljust(commentcol), comment)
                fp.write("%s\n"%(entry,))

            fp.write('};\n')


if __name__ == '__main__':
    args = ArgParser().parse_args()
    table = ErrorTable(os.path.join(args.sysroot, args.includedir), args.inputs)
    table.save_to(args.output, args.mapname)
