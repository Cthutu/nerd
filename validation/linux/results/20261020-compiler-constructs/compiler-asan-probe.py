from pathlib import Path
import os,sys,subprocess,tempfile,json
sys.path.insert(0,str(Path('build').resolve()))
from test_render_threads import sanitized_compiler
from test import split_sections
root=Path.cwd()
env=dict(os.environ,NERD_LIB_PATH=os.pathsep.join([str(root/'mods'),str(root/'tests/mods')]),ASAN_OPTIONS='detect_leaks=1:halt_on_error=1')
with tempfile.TemporaryDirectory(prefix='nerd-constructs-front-') as work:
 work=Path(work);compiler=sanitized_compiler(work/'compiler','address')
 for fixture in sorted((root/'tests/language').glob('2[45][0-9]-*.t')):
  if int(fixture.name[:3])<244:continue
  source=work/(fixture.stem+'.n');source.write_text(split_sections(fixture)[0])
  result=subprocess.run([str(compiler),'build',str(source),'-o',str(work/'program')],env=env,capture_output=True,text=True,timeout=60)
  assert result.returncode==0 and not result.stderr,(fixture,result.returncode,result.stdout,result.stderr)
  print('PASS compiler ASan:',fixture.name,flush=True)
 for fixture in [root/'tests/errors/152-scientific-floats.e',root/'tests/errors/153-compiler-constructs.e']:
  parts=split_sections(fixture)
  for i in range(0,len(parts)-1,2):
   if not parts[i].strip():continue
   source=work/'negative.n';source.write_text(parts[i])
   result=subprocess.run([str(compiler),'check',str(source)],env=dict(env,NERD_ERROR_RENDER_TEST='1'),capture_output=True,text=True,timeout=20)
   assert result.returncode!=0,(fixture,i,result.returncode,result.stdout,result.stderr)
   diagnostic=json.loads(result.stdout if result.stdout.strip() else result.stderr)
   assert diagnostic.get("primary_location"),diagnostic
  print('PASS compiler ASan diagnostics:',fixture.name,flush=True)
 depth=24
 source=work/'deep.n';source.write_text('Cell :: plex [T] { value T }\nDeep :: '+'Cell['*depth+'i32'+']'*depth+'\nmain :: fn () -> i32 { value: Deep = '+'{ value: '*depth+'42'+' }'*depth+'\nreturn value'+'.value'*depth+'\n}')
 result=subprocess.run([str(compiler),'build',str(source),'-o',str(work/'program')],env=env,capture_output=True,text=True,timeout=60)
 assert result.returncode==0 and not result.stderr,(result.returncode,result.stdout,result.stderr)
 result=subprocess.run([str(work/'program')],env=env,capture_output=True,text=True,timeout=10)
 assert result.returncode==42 and not result.stderr,(result.returncode,result.stdout,result.stderr)
 print('PASS compiler ASan: 24 nested type applications',flush=True)
