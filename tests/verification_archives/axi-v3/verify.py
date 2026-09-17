"""Bounded Axi v3 cross-agent verification. Never repairs product source."""
import datetime
import hashlib
import json
import pathlib
import subprocess
import uuid

BASE = pathlib.Path(__file__).resolve().parent
RUN = BASE / ('run-' + datetime.datetime.now().strftime('%Y%m%d-%H%M%S') + '-' + uuid.uuid4().hex[:6])
RUN.mkdir(parents=True)
ROOT = pathlib.Path('C:/Ethos/ethos-logos')
INTERNAL = ROOT / 'LANG/release/axiomc.exe'
PUBLIC = ROOT / 'FOSS/axi_compiler/bin/windows-x64/axiomc_public.exe'
CORE = ROOT / 'FOSS/axi_compiler/lib/core'
CLANG = pathlib.Path('C:/Program Files/LLVM/bin/clang.exe')
FILES = [INTERNAL, PUBLIC, CORE/'dag.axi', CORE/'dag_native.c', CORE/'dag_native.h', ROOT/'LANG/src/compiler/main.cpp', ROOT/'LANG/src/compiler/dvcs.cpp', ROOT/'LANG/src/compiler/dvcs.h', ROOT/'LANG/docs/handoffs/axi_infrastructure_v3_handoff.md', ROOT/'LANG/release/.axi/refs/heads/main', ROOT/'LANG/release/.axi/objects/18d318bc240f95bc00000000599ed38db0de1e36']
def hashes():
    return {str(p): hashlib.sha256(p.read_bytes()).hexdigest() for p in FILES}
results = {'run':str(RUN), 'notion':'https://app.notion.com/p/3d4b679aa7008167b710c236017b8769', 'before':hashes(), 'commands':[], 'observations':{}}
def run(label, argv, cwd):
    cwd.mkdir(parents=True, exist_ok=True)
    try:
        p = subprocess.run([str(a) for a in argv], cwd=cwd, capture_output=True, text=True, errors='replace', timeout=30)
        item = {'label':label, 'argv':[str(a) for a in argv], 'cwd':str(cwd), 'exit':p.returncode, 'stdout':p.stdout, 'stderr':p.stderr}
    except Exception as e:
        item = {'label':label, 'argv':[str(a) for a in argv], 'cwd':str(cwd), 'error':str(e)}
    results['commands'].append(item)
    print(json.dumps(item), flush=True)
    return item

ledger = RUN/'internal-ledger'
run('internal-help', [INTERNAL], ledger)
run('internal-init', [INTERNAL,'init'], ledger)
(ledger/'tracked').mkdir()
marker = 'AXI_V3_VERIFICATION_PAYLOAD_73912'
(ledger/'tracked/fixture.txt').write_text(marker)
run('internal-track', [INTERNAL,'track','tracked'], ledger)
run('internal-wrap', [INTERNAL,'wrap'], ledger)
run('internal-ship', [INTERNAL,'ship'], ledger)
objects = {p.name:p.read_text() for p in (ledger/'.axi/objects').iterdir() if p.is_file()}
results['observations']['internal_objects'] = objects
results['observations']['tracked_payload_present'] = any(marker in text for text in objects.values())
results['observations']['internal_main_ref'] = (ledger/'.axi/refs/heads/main').read_text()
results['observations']['tracked_config'] = (ledger/'.axi/axi_config.toon').read_text()
run('internal-wrap-no-store', [INTERNAL,'wrap'], RUN/'no-store')
run('internal-build-missing-input', [INTERNAL,'build','absent.axi'], RUN/'missing-input')
for name,args in [('public-help', []), ('public-init-rejection', ['init'])]:
    run(name,[PUBLIC,*args],RUN/name)

public_compile = RUN/'public-compile'
public_compile.mkdir()
(public_compile/'hello.axi').write_text('HOW TO start():\n    WRITE "AXI_VERIFY_HELLO" TO SCREEN\n\nstart -> start\n')
run('public-build-smoke', [PUBLIC,'build','hello.axi'],public_compile)
results['observations']['public_build_files'] = {p.name:p.read_text(errors='replace') for p in public_compile.glob('*.c')}

native = RUN/'native'
native.mkdir()
(native/'.axi/objects').mkdir(parents=True)
(native/'.axi/refs/heads').mkdir(parents=True)
probe = native/'probe.c'
probe.write_text(r'''#include <stdio.h>
#include <string.h>
#include "dag_native.h"
int main(void) {
 const char *payload = "AXI_NATIVE_ROUNDTRIP_73912";
 const char *result = __native_dag_commit("probe", "verification", payload);
 struct DagNode node = __native_dag_query("probe");
 printf("commit=%s\nquery=%s\n", result, node.toon_content ? node.toon_content : "NULL");
 if (!node.toon_content || !strstr(node.toon_content, payload)) return 2;
 return 0;
}
''')
run('clang-version',[CLANG,'--version'],native)
build = run('native-backend-build',[CLANG,'-std=c17','-I',CORE,probe,CORE/'dag_native.c','-o',native/'probe.exe'],native)
if build.get('exit') == 0:
    run('native-roundtrip',[native/'probe.exe'],native)
    results['observations']['native_committed_object'] = (native/'.axi/objects/probe').read_text()
checkout = native/'checkout.c'
checkout.write_text('#include "dag_native.h"\nextern struct DagNode __native_dag_checkout(const char*);\nint main(void) { struct DagNode n = __native_dag_checkout("probe"); return n.hash == 0; }\n')
run('native-checkout-link',[CLANG,'-std=c17','-I',CORE,checkout,CORE/'dag_native.c','-o',native/'checkout.exe'],native)
results['after'] = hashes()
results['observations']['product_files_and_release_ref_unchanged'] = results['before'] == results['after']
(RUN/'results.json').write_text(json.dumps(results,indent=2))
(BASE/'latest-run.txt').write_text(str(RUN))
print(json.dumps({'run':str(RUN),'observations':results['observations']},indent=2),flush=True)
