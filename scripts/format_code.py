from yapf.yapflib.yapf_api import FormatFile
import pathlib as pl

path = pl.Path('.')

#loop over all python files and format the files in place
for file in path.iterdir():
    if file.suffix == '.py' and file.stem != 'format_code':
        FormatFile(str(file), style_config='pep8', in_place = True)