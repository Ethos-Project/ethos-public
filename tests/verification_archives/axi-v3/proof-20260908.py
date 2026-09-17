"""Verify reported builds, SHA3, FOSS receipts and compiler command separation."""
import datetime, hashlib, json, pathlib, re, subprocess, uuid
BASE=pathlib.Path(__file__).resolve().parent
RUN=BASE/('proof-'+datetime.datetime.now().strftime('%Y%m%d-%H%M%S')+'-'+uuid.uuid4().hex[:6]); RUN.mkdir()
ROOT=pathlib.Path('C:/Ethos/ethos-logos')
SRC=ROOT/'LANG/src/compiler'
DVCS=ROOT/'FOSS/axi_dvcs'
GPP=SRC/'bootstrap/python_to_c_compiler/bin/mingw64/bin/g++.exe'
PUB=ROOT/'FOSS/axi_compiler/axiomc_public.exe'
INT=ROOT/'LANG/release/axiomc_internal.exe'
RECEIPT=ROOT/'FOSS/axi_tui/.axi/objects/1a1125a9f5540714a7f3af564c07210a14a9771a11a39289657e2ed9dea45dbe'
paths=[PUB,INT,DVCS/'main.cpp',DVCS/'sha3.hpp',DVCS/'axi.exe',ROOT/'FOSS/axi_tui/axi.exe',RECEIPT,SRC/'main.cpp',SRC/'lexer.cpp',SRC/'parser.cpp',SRC/'analyzer.cpp',SRC/'emitter.cpp',SRC/'dvcs.cpp',SRC/'sha3.hpp']
def manifest(): return {str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths}
R={'run':str(RUN),'before':manifest(),'commands':[],'observations':{}}
def run(label,args,cwd=None,input=None):
    cwd=cwd or RUN; cwd.mkdir(parents=True,exist_ok=True)
    p=subprocess.run([str(a) for a in args],cwd=cwd,input=input,capture_output=True,text=True,errors='replace',timeout=60)
    row={'label':label,'args':[str(a) for a in args],'cwd':str(cwd),'exit':p.returncode,'stdout':p.stdout,'stderr':p.stderr}
    R['commands'].append(row); print(json.dumps(row),flush=True); return row
run('build-standalone-foss',[GPP,'-std=c++17','-static',DVCS/'main.cpp','-o',RUN/'rebuilt-axi.exe'])
units=[SRC/(n+'.cpp') for n in ['main','lexer','parser','analyzer','emitter']]
run('build-internal',[GPP,'-std=c++17','-static','-DAXIOM_INTERNAL_BUILD',*units,SRC/'dvcs.cpp','-o',RUN/'rebuilt-internal.exe'])
run('build-public',[GPP,'-std=c++17','-static',*units,'-o',RUN/'rebuilt-public.exe'])
probe=RUN/'hash_probe.cpp'
probe.write_text('#include <iostream>\n#include <iterator>\n#include "sha3.hpp"\nint main(){std::string s((std::istreambuf_iterator<char>(std::cin)),{});std::cout<<axiom::crypto::SHA3_256::hash(s);}\n')
run('build-hash-probe',[GPP,'-std=c++17','-static','-I',DVCS,probe,'-o',RUN/'hash_probe.exe'])
vectors=[]
if (RUN/'hash_probe.exe').exists():
    for s in ['', 'abc','a'*135,'a'*136,'a'*137,'a'*272,'a'*1000]:
        row=run('sha3-vector-'+str(len(s)),[RUN/'hash_probe.exe'],input=s)
        vectors.append({'bytes':len(s),'expected':hashlib.sha3_256(s.encode()).hexdigest(),'actual':row['stdout'].strip(),'pass':row['stdout'].strip()==hashlib.sha3_256(s.encode()).hexdigest()})
R['observations']['sha3_vectors']=vectors
for label,exe in [('packaged-foss',DVCS/'axi.exe'),('rebuilt-foss',RUN/'rebuilt-axi.exe')]:
    if not exe.exists():continue
    cwd=RUN/label; cwd.mkdir(); (cwd/'fixture.txt').write_text('PAYLOAD_A')
    run(label+'-init',[exe,'init'],cwd);run(label+'-wrap',[exe,'wrap'],cwd)
    ref=(cwd/'.axi/refs/heads/main').read_text().strip()
    p=cwd/'.axi/objects'/ref
    text=p.read_text(); ts=re.search(r'timestamp: (\d+)',text).group(1)
    R['observations'][label]={'ref':ref,'hex64':bool(re.fullmatch('[0-9a-f]{64}',ref)),'object':text,'timestamp_hash_matches':ref==hashlib.sha3_256(('foss_wrap_'+ts).encode()).hexdigest(),'object_hash_matches':ref==hashlib.sha3_256(p.read_bytes()).hexdigest()}
    bad=RUN/(label+'-failed-write');(bad/'.axi/refs/heads').mkdir(parents=True);(bad/'.axi/objects').write_text('blocked');(bad/'.axi/refs/heads/main').write_text('BASELINE')
    run(label+'-failed-write',[exe,'wrap'],bad)
    R['observations'][label]['failed_write_ref_preserved']=(bad/'.axi/refs/heads/main').read_text()=='BASELINE'
for label,exe in [('packaged-public',PUB),('rebuilt-public',RUN/'rebuilt-public.exe')]:
    if exe.exists():
        cwd=RUN/label
        run(label+'-init',[exe,'init'],cwd)
        R['observations'][label+'_created_ledger']=(cwd/'.axi').exists()
text=RECEIPT.read_text(); ts=re.search(r'timestamp: (\d+)',text).group(1)
R['observations']['supplied_receipt']={'filename':RECEIPT.name,'hex64':bool(re.fullmatch('[0-9a-f]{64}',RECEIPT.name)),'object':text,'expected_timestamp_hash':hashlib.sha3_256(('foss_wrap_'+ts).encode()).hexdigest(),'timestamp_hash_matches':RECEIPT.name==hashlib.sha3_256(('foss_wrap_'+ts).encode()).hexdigest(),'object_hash':hashlib.sha3_256(RECEIPT.read_bytes()).hexdigest()}
R['after']=manifest();R['observations']['inspected_files_unchanged']=R['before']==R['after']
(RUN/'results.json').write_text(json.dumps(R,indent=2));(BASE/'latest-proof.txt').write_text(str(RUN))
print(json.dumps({'run':str(RUN),'observations':R['observations']},indent=2))
