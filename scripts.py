#!/usr/bin/env python3
import os, sys

def main(opt):
    print(opt)
    if not os.path.exists('./build/'):
        os.makedirs('./build')

    if opt == 'build-release':
        if not os.path.exists('./bin/release/'):
            os.makedirs('./bin/release')

        os.system("cd build; cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..; ninja")

    elif opt == 'build-debug':
        if not os.path.exists('./bin/debug/'):
            os.makedirs('./bin/debug')

        os.system("cd build; cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..; ninja")

    elif opt == 'clean':
        os.system("cd build; rm -rdf *")

    else:
        print("invalid command")

if __name__ == "__main__":
    main(sys.argv[1])
