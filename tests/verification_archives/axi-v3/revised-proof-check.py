"""Validate the latest semantic-payload hashing receipt without product edits."""
import hashlib,json,pathlib,re,subprocess
BASE=pathlib.Path(__file__).resolve().parent
RUN=pathlib.Path((BASE/'latest-proof.txt').read_text())
R=json.loads((RUN/'results.json').read_text())
ROOT=pathlib.Path('C:/Ethos/ethos-logos')
def assess(text,digest):
    payload=text.split('  semantic_layer:\n',1)[1]
    ts=re.search(r'timestamp: (\d+)',text).group(1)
    data=(payload+ts).encode()
    expected=hashlib.sha3_256(data).hexdigest()
    return {'hash_input_bytes':len(data),'expected_sha3_256':expected,'actual':digest,'pass':expected==digest}
for name in ['packaged-foss','rebuilt-foss']:
    o=R['observations'][name]
    o['semantic_payload_hash']=assess(o['object'],o['ref'])
cwd=RUN/'semantic-long-payload';cwd.mkdir()
for name in ['one.txt','two.txt','three.txt']:(cwd/name).write_text('Payload fixture '+name)
exe=ROOT/'FOSS/axi_dvcs/axi.exe'
for cmd in ['init','wrap']:
    p=subprocess.run([str(exe),cmd],cwd=cwd,capture_output=True,text=True,timeout=30)
    R['commands'].append({'label':'long-payload-'+cmd,'args':[str(exe),cmd],'cwd':str(cwd),'exit':p.returncode,'stdout':p.stdout,'stderr':p.stderr})
digest=(cwd/'.axi/refs/heads/main').read_text().strip()
obj=(cwd/'.axi/objects'/digest).read_text()
R['observations']['long_payload_wrap']=assess(obj,digest)
R['observations']['long_payload_wrap']['object']=obj
revised=ROOT/'FOSS/axi_tui/.axi/objects/4614ffdfa6a16086bbf4479e390c58a5e88cbffc9c90ebfcbd50b86c2d1b0d2d'
R['observations']['revised_receipt_exists_at_supplied_path']=revised.exists()
if revised.exists(): R['observations']['revised_receipt']=assess(revised.read_text(),revised.name)
R['remote_verification']={'method':'git ls-remote origin refs/heads/master','remote':'https://github.com/Ethos-Project/ethos-public','master':'f16e4e405f89cfa3f6e3a477962bd6090a7c665c','prior_master':'207a69d5d3cf7727e99cb82cc4cbd3b5806216e7'}
R['after']={p:hashlib.sha256(pathlib.Path(p).read_bytes()).hexdigest() for p in R['before']}
R['observations']['inspected_files_unchanged']=R['before']==R['after']
(RUN/'results.json').write_text(json.dumps(R,indent=2))
print(json.dumps({k:R['observations'][k] for k in ['long_payload_wrap','revised_receipt_exists_at_supplied_path','inspected_files_unchanged']},indent=2))
print(json.dumps({k:R['observations'][k]['semantic_payload_hash'] for k in ['packaged-foss','rebuilt-foss']},indent=2))
