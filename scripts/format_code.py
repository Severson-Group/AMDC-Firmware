from yapf.yapflib.yapf_api import FormatFile
import pathlib as pl

path = pl.Path('.')

py_files_to_ignore = ['format_code', 'run-clang-format']

#loop over all python files and format the files in place
for file in path.iterdir():
    if file.suffix == '.py' and file.stem != 'format_code':
        FormatFile(str(file), style_config='setup.cfg', in_place = True)
