from pathlib import Path
import os,subprocess,tempfile,shlex,sys
sys.path.insert(0,str(Path("build").resolve()))
from test import split_sections
root=Path.cwd()
env=dict(os.environ,NERD_LIB_PATH=os.pathsep.join([str(root/'mods'),str(root/'tests/mods')]),ASAN_OPTIONS='detect_leaks=1:halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1')
def run(command):
 r=subprocess.run(command,capture_output=True,text=True,env=env,timeout=90)
 if r.returncode or r.stderr: raise RuntimeError((command,r.returncode,r.stdout,r.stderr))
 return r.stdout.strip()
with tempfile.TemporaryDirectory(prefix='nerd-constructs-') as work:
 work=Path(work)
 for fixture in sorted((root/'tests/language').glob('2[45][0-9]-*.t')):
  if int(fixture.name[:3])<244: continue
  source,status,expected,*_=split_sections(fixture)
  path=work/(fixture.stem+'.n');path.write_text(source)
  for cgen in (False,True):
   for release in (False,True):
    binary=work/'program';output=work/'program.c' if cgen else binary
    cmd=[str(root/'_bin/nerd-debug'),'build',str(path),'-o',str(output)]
    if release:cmd.append('-r')
    if cgen:
     cmd+=['--cgen','--copts'];options=run(cmd)
     run(['clang',str(output),*shlex.split(options),'-fsanitize=address,undefined','-fno-omit-frame-pointer','-o',str(binary)])
    else:run(cmd)
    result=subprocess.run([str(binary)],env=env,capture_output=True,text=True,timeout=15)
    assert result.returncode==int(status) and result.stdout.strip()==expected.strip() and not result.stderr,(fixture,cgen,release,result.returncode,result.stdout,result.stderr)
    print(f'PASS {fixture.name}: {"C ASan/UBSan/leaks" if cgen else "LLVM"}, release={release}',flush=True)
