from pathlib import Path
import os,sys,shlex,tempfile
sys.path.insert(0,str(Path('build').resolve()))
from test_std_thread_sync import run
root=Path.cwd()
env=dict(os.environ,NERD_LIB_PATH=os.pathsep.join([str(root/'mods'),str(root/'tests/mods')]),ASAN_OPTIONS='detect_leaks=1:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1')
with tempfile.TemporaryDirectory(prefix='nerd-box-borrows-') as work:
    work=Path(work)
    source=work/'borrow.n'
    source.write_text((root/'tests/language/243-implicit-box-borrows.t').read_text().split('\n¬\n')[0])
    for release in (False,True):
        output=work/'borrow.c'
        command=[str(root/'_bin/nerd-debug'),'build',str(source),'--cgen','--copts','-o',str(output)]
        if release: command.append('-r')
        options=run(command,env)
        binary=work/'borrow'
        run(['clang',str(output),*shlex.split(options),'-fsanitize=address,undefined','-fno-omit-frame-pointer','-o',str(binary)],env)
        assert run([str(binary)],env,15)=='Implicit box borrows passed'
        print(f'PASS C debug-compiler release={release}: ASan, UBSan and leak detection',flush=True)
