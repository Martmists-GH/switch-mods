import json
import os
import sys
from textwrap import indent
from typing import Dict, Tuple, List, Union

type StringMap = Dict[str, StringMap | str]

def expand(strings: Dict[str, str]) -> StringMap:
    new = {}
    def _(ctx: dict, key: str) -> dict:
        if key in ctx:
            return ctx[key]
        else:
            ctx[key] = {}
            return ctx[key]

    def parse(ctx: dict, key: str) -> Tuple[str, dict]:
        if '.' in key:
            new_key, *remaining = key.split('.')
            new_ctx = _(ctx, new_key)
            return parse(new_ctx, '.'.join(remaining))
        else:
            return key, ctx

    for k, v in strings.items():
        nk, d = parse(new, k)
        d[nk] = v

    return new


def generate_structs(key: str, data: StringMap) -> List[str]:
    s = f"struct {key}_t {{\n"
    nested = []
    for k, v in data.items():
        if isinstance(v, str):
            s += f"    const char* {k};\n"
        else:
            nested += generate_structs(f"{key}_{k}", v)
            s += f"    struct {key}_{k}_t {k};\n"
    s += "};"
    return [*nested, s]

def generate_data(name: str, data: StringMap) -> str:
    def escape(x: str) -> str:
        return '"' + x.replace('\\', '\\\\').replace('"', '\\"') + '"'

    def generate_inner(nested: StringMap) -> str:
        si = "{\n"
        for i, (ki, vi) in enumerate(nested.items()):
            if isinstance(vi, str):
                si += f"    .{ki} = {escape(vi)}"
            else:
                si += f"    .{ki} = {indent(generate_inner(vi), '    ').strip()}"
            if i - 1 == len(nested):
                si += "\n"
            else:
                si += ",\n"
        si += "}"
        return si

    s = f"struct strings_t {name} = " + generate_inner(data) + ";\n"
    return s

def main(args: List[str]):
    if len(args) < 2:
        print("Usage: ./lang_generate_strings.py <out_folder> <in_folders...>")
        sys.exit(1)

    out_folder = args[0]
    in_folders = args[1:]
    langs: Dict[str, dict] = {}
    for in_folder in in_folders:
        if not os.path.exists(in_folder): continue
        for file in os.listdir(in_folder):
            if file.endswith(".json"):
                lang = file.removesuffix(".json")
                with open(os.path.join(in_folder, file), "r") as f:
                    data = json.load(f)
                if lang in langs:
                    langs[lang].update(data)
                else:
                    langs[lang] = data

    if langs:
        rand_entry = langs[list(langs.keys())[0]]
        mapping = expand(rand_entry)
        if not os.path.exists(out_folder):
            os.makedirs(out_folder)

        with open(os.path.join(out_folder, "strings.hpp"), "w") as f:
            f.write("#pragma once\n#include <string>\n")

            for struct in generate_structs("strings", mapping):
                f.write(struct)
                f.write("\n")
            for lang in langs.keys():
                f.write(f"extern struct strings_t {lang};\n")
    else:
        with open(os.path.join(out_folder, "strings.hpp"), "w") as f:
            f.write("#pragma once\n#include <string>\n")

    with open(os.path.join(out_folder, "strings.cpp"), "w") as f:
        f.write("#include <strings.hpp>\n")
        for lang in langs.keys():
            f.write(generate_data(lang, expand(langs[lang])))

if __name__ == '__main__':
    main(sys.argv[1:])
