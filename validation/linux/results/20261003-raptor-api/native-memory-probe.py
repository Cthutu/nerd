import os,sys,shlex,tempfile
from pathlib import Path
sys.path.insert(0,str(Path('build').resolve()))
from test_std_thread_sync import run
root=Path.cwd()
env=dict(os.environ,NERD_LIB_PATH=str(root/'mods'),ASAN_OPTIONS='detect_leaks=1:halt_on_error=1')
with tempfile.TemporaryDirectory(prefix='nerd-raptor-asan-') as directory:
    for release in (False,True):
        for source,expected in [('tests/stdlib-raptor/contracts.n','Raptor deque, stealing, serial, nested waits, producers and drain passed'),('examples/task-parallel/task-parallel.n','Raptor tasks: sum of squares = 204')]:
            output=Path(directory)/'tasks.c'
            cmd=[str(root/'_bin/nerd-debug'),'build',source,'--cgen','--copts','-o',str(output)]
            if release: cmd.append('-r')
            opts=run(cmd,env)
            executable=Path(directory)/'tasks'
            run(['clang',str(output),*shlex.split(opts),'-fsanitize=address,undefined','-fno-omit-frame-pointer','-o',str(executable)],env)
            for _ in range(8):
                actual=run([str(executable)],env,15)
                assert actual==expected,actual
            print(f'PASS ASan/UBSan/leak detection {source} release={release} (8 runs)',flush=True)
