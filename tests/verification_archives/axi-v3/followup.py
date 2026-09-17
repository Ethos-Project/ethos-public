"""Focused follow-up on failures found by verify.py; scratch output only."""
import hashlib
import json
import pathlib
import subprocess

BASE = pathlib.Path(__file__).resolve().parent
RUN = pathlib.Path((BASE/'latest-run.txt').read_text())
ROOT = pathlib.Path('C:/Ethos/ethos-logos')
PUBLIC = ROOT/'FOSS/axi_compiler/bin/windows-x64/axiomc_public.exe'
INTERNAL = ROOT/'LANG/release/axiomc.exe'
results = json.loads((RUN/'results.json').read_text())
def run(label,args,cwd):
    cwd.mkdir(parents=True,exist_ok=True)
    p = subprocess.run([str(a) for a in args],cwd=cwd,capture_output=True,text=True,errors='replace',timeout=30)
    item = {'label':label,'argv':[str(a) for a in args],'cwd':str(cwd),'exit':p.returncode,'stdout':p.stdout,'stderr':p.stderr}
    results['commands'].append(item)
    print(json.dumps(item),flush=True)
    return item
smoke = RUN/'minimal-compile'
smoke.mkdir()
(smoke/'main.axi').write_text('HOW TO main():\n    WRITE 42 TO SCREEN\n')
run('public-minimal-compile',[PUBLIC,'build','main.axi'],smoke)
results['observations']['minimal_emitted_c'] = (smoke/'build_output.c').read_text() if (smoke/'build_output.c').exists() else None
results['observations']['minimal_app_exists'] = (smoke/'app.exe').exists()
run('public-dag-api-compile',[PUBLIC,'build',ROOT/'FOSS/axi_compiler/lib/core/dag.axi'],RUN/'dag-api-compile')
run('public-repository-dag-example',[PUBLIC,'build',ROOT/'LANG/release/test_dag.axi'],RUN/'dag-example-compile')
bad = RUN/'write-failure'
(bad/'.axi/refs/heads').mkdir(parents=True)
(bad/'.axi/refs/heads/main').write_text('BASELINE_SENTINEL')
(bad/'.axi/objects').write_text('Deliberately not a directory, to simulate unavailable object storage.')
run('internal-ship-object-write-failure',[INTERNAL,'ship'],bad)
results['observations']['write_failure_ref'] = (bad/'.axi/refs/heads/main').read_text()
results['observations']['write_failure_ref_unchanged'] = results['observations']['write_failure_ref'] == 'BASELINE_SENTINEL'
clang = pathlib.Path('C:/Program Files/LLVM/bin/clang.exe')
run('native-sdk-header-check',[clang,'-std=c17','-fsyntax-only','-isystem','C:/Program Files (x86)/Windows Kits/10/Include/10.0.26100.0/ucrt',ROOT/'FOSS/axi_compiler/lib/core/dag_native.c'],RUN/'native')
results['after'] = {p:hashlib.sha256(pathlib.Path(p).read_bytes()).hexdigest() for p in results['before']}
results['observations']['product_files_and_release_ref_unchanged'] = results['before'] == results['after']
(RUN/'results.json').write_text(json.dumps(results,indent=2))
print(json.dumps(results['observations'],indent=2))
