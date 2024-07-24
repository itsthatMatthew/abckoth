# This script is used to process a folder of html source files and turn them
# into a single includable C-style header containing all sources as char arrays.

# Modify these values if the path or filename changes
SourcesPath = './src/net/websrc'
OutputPath = './src/net/websrc.h'

# Modify these values to influence the script's behaviour
IncludeGuard = 'NET_WEBSITE_H' # C style header include guard
RawStringGuard = '###' # Raw string guard for a non-trivial escape sequence
WarningText = 'THIS IS AN AUTOMATICALLY GENERATED FILE, DO NOT MODIFY IT DIRECTLY'

print('WEBCONVERTER running...')

# Script body
Import("env") # used by pio build to install necessary packages
env.Execute("$PYTHONEXE -m pip install BeautifulSoup4")

import os
from bs4 import BeautifulSoup, Comment

def process_source_type(path):
  if path.endswith('html'):
    return process_html_source(path)
  else:
    return process_arbitrary_source(path)

def process_html_source(path):
  with open(path, mode='r', encoding='utf8') as source:
    raw_html = BeautifulSoup(source.read(), 'html.parser')
  for element in raw_html:
    if isinstance(element, Comment):
      element.extract()
  return str(raw_html)

def process_arbitrary_source(path):
  with open(path, mode='r', encoding='utf8') as source:
    return source.read()

def replace_invalid_chars(path):
  invalid_chars = '\/.'
  for c in invalid_chars:
    path = path.replace(c, '_')
  return path

# recursively collect all sources
sources_collection = dict()
routes_collection = dict()
sources_count = 0
sources_raw_len = 0
sources_processed_len = 0

for root, dirs, files in os.walk(SourcesPath):
  for file in files:
    path = os.path.join(root, file)
    relative_path = path.removeprefix(SourcesPath)
    key = replace_invalid_chars(relative_path).lstrip('_')
    raw = process_source_type(path)
    clean = ''
    for line in raw.splitlines():
      clean += line.strip()
    raw_len = os.path.getsize(path) + 1
    clean_len = len(clean) + 1
    routes_collection[key] = [relative_path.replace('\\', '/'), os.path.splitext(relative_path)[1].lstrip('.')]
    sources_collection[key] = clean
    sources_count += 1
    sources_raw_len += raw_len
    sources_processed_len += clean_len
    print(f'\t{relative_path} converted: {clean_len} bytes (compressed from {raw_len})')

print('WEBCONVERTER writing output file...')

# write compressed html sources into output file
with open(OutputPath, mode='w', encoding='utf8') as header:
  header.write(f'/*{WarningText}*/\n')
  header.write(f'#ifndef {IncludeGuard}\n#define {IncludeGuard}\n')
  header.write('#include <Arduino.h>\n')
  for variable, data in sources_collection.items():
    header.write(f'constexpr char {variable}[] PROGMEM = R"{RawStringGuard}({data}){RawStringGuard}";\n')
  header.write('constexpr struct {const char* name; const char* type; const char* data; } websrc[] = {')
  for reference, route in routes_collection.items():
    header.write(f'{{ "{route[0]}", "text/{route[1]}", {reference} }},')
  header.write('};\n')
  header.write('#endif')

print(f'WEBCONVERTER generated C-style char array for {sources_count} files with a size of {sources_processed_len} bytes (compressed from {sources_raw_len} bytes).')