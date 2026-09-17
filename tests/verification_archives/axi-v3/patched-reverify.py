"""Additional evidence for the reported Axi v3 patches; scratch files only."""
import hashlib
import json
import pathlib
import subprocess

BASE = pathlib.Path(__file__).resolve().parent
RUN = pathlib.Path((BASE/'latest-run.txt').read_text())
ROOT = pathlib.Path('C:/Ethos/ethos-logos')
GCC = ROOT/'LANG/src/compiler/bootstrap/python_to_c_compiler/bin/mingw64/bin/gcc.exe'
INTERNAL = ROOT/'LANG/release/axiomc.exe'
PUBLIC = ROOT/'FOSS/axi_compiler/bin/windows-x64/axiomc_public.exe'
CORE = ROOT/'FOSS/axi_compiler/lib/core'
PLAN = pathlib.Path('C:/Users/theca/.gemini/antigravity/brain/7e7090fa-939b-4c07-971b-c13f49bab2a2/implementation_plan.md')
results = json.loads((RUN/'results.json').read_text())
extra = [GCC, PLAN, ROOT/'LANG/src/compiler/parser.cpp', ROOT/'LANG/src/compiler/emitter.cpp', ROOT/'LANG/release/test_dag.axi', ROOT/'LANG/release/lib/core/dag_native.c', ROOT/'LANG/release/lib/core/dag_native.h', ROOT/'LANG/release/.axi/objects/18d31a951fb7b0c800000000f760c9d5317ef257']
results['extra_before'] = {str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in extra}
def run(label,args,cwd):
    cwd.mkdir(parents=True,exist_ok=True)
    p = subprocess.run([str(a) for a in args],cwd=cwd,capture_output=True,text=True,errors='replace',timeout=30)
    item = {'label':label,'argv':[str(a) for a in args],'cwd':str(cwd),'exit':p.returncode,'stdout':p.stdout,'stderr':p.stderr}
    results['commands'].append(item)
    print(json.dumps(item),flush=True)
    return item
native = RUN/'native'
run('bundled-gcc-version',[GCC,'--version'],native)
build = run('bundled-gcc-native-build',[GCC,'-std=c17','-static','-I',CORE,native/'probe.c',CORE/'dag_native.c','-o',native/'probe.exe'],native)
if build['exit'] == 0:
    run('bundled-gcc-native-roundtrip',[native/'probe.exe'],native)
    results['observations']['native_committed_object'] = (native/'.axi/objects/probe').read_text()
run('bundled-gcc-checkout-link',[GCC,'-std=c17','-static','-I',CORE,native/'checkout.c',CORE/'dag_native.c','-o',native/'checkout.exe'],native)
run('patched-internal-dag-example',[INTERNAL,'build',ROOT/'LANG/release/test_dag.axi'],RUN/'patched-internal-example')
imports = RUN/'import-only'
imports.mkdir()
(imports/'main.axi').write_text('import core::dag\n\nHOW TO main():\n    WRITE 42 TO SCREEN\n')
run('patched-internal-import-emission',[INTERNAL,'build','main.axi'],imports)
results['observations']['import_emitted_c'] = (imports/'build_output.c').read_text() if (imports/'build_output.c').exists() else None
if (imports/'app.exe').exists():
    run('patched-internal-import-app',[imports/'app.exe'],imports)
ledger = RUN/'internal-ledger'
(ledger/'tracked/subdir').mkdir()
(ledger/'tracked/subdir/nested.txt').write_text('NESTED_PAYLOAD_MARKER')
(ledger/'tracked/fixture.txt').write_text('CHANGED_CONTENT_MARKER')
before = set(p.name for p in (ledger/'.axi/objects').iterdir())
run('patched-wrap-nested-content',[INTERNAL,'wrap'],ledger)
new = [p for p in (ledger/'.axi/objects').iterdir() if p.name not in before]
results['observations']['nested_wrap'] = {p.name:p.read_text() for p in new}
results['observations']['public_internal_identical'] = PUBLIC.read_bytes() == INTERNAL.read_bytes()
results['observations']['new_receipt'] = extra[-1].read_text()
results['observations']['new_receipt_matches_live_main'] = (ROOT/'LANG/release/.axi/refs/heads/main').read_text().strip() == extra[-1].name
results['after'] = {p:hashlib.sha256(pathlib.Path(p).read_bytes()).hexdigest() for p in results['before']}
results['extra_after'] = {str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in extra}
results['observations']['product_files_and_release_ref_unchanged'] = results['before'] == results['after'] and results['extra_before'] == results['extra_after']
(RUN/'results.json').write_text(json.dumps(results,indent=2))
print(json.dumps(results['observations'],indent=2))
