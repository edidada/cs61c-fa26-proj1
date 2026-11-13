#!/usr/bin/env bash
set -euo pipefail
python3 - <<'PY'
import subprocess,random,json,os,pty,select,time,re
from pathlib import Path
checks=[]
def run(board,args=None):
 p=subprocess.run(['./snake','--stdin']+(args or []),input=board.encode(),stdout=subprocess.PIPE,stderr=subprocess.PIPE,timeout=10)
 assert p.returncode==0,p.stderr
 return p.stdout.decode()
# Independent coordinate oracle: each separate row has a three-cell snake.
# No food is supplied here, so expected changes do not depend on food RNG.
r=random.Random(6101); cases=0
for board_index in range(64):
 width=r.randrange(8,80); rows=['#'*width]; snakes=[]
 for row in range(1,7):
  w=r.randrange(8,width+1); line=['#']+[' ']*(w-2)+['#']; tail=r.randrange(1,w-3)
  right=bool(r.getrandbits(1))
  if right:
   line[tail:tail+3]='d>D'; snakes.append([row,tail,row,tail+2,1,True])
  else:
   line[tail:tail+3]='A<a'; snakes.append([row,tail+2,row,tail,-1,True])
  rows.append(''.join(line))
 rows.append('#'*width); board='\n'.join(rows)+'\n'
 for step in range(8):
  grid=[list(x) for x in board.splitlines()]
  for snake in snakes:
   tr,tc,hr,hc,d,alive=snake
   if not alive: continue
   cell=grid[hr][hc+d]
   if cell=='#' or cell in 'wasd^<v>WASDx': grid[hr][hc]='x'; snake[5]=False
   else:
    grid[hr][hc]='>' if d==1 else '<'; grid[hr][hc+d]='D' if d==1 else 'A'
    grid[tr][tc]=' '; grid[tr][tc+d]='d' if d==1 else 'a'
    snake[1]+=d; snake[3]+=d
  expected='\n'.join(''.join(x) for x in grid)+'\n'
  # Each new CLI run re-discovers dead heads too, ensuring freeze across saves.
  actual=run(board); assert actual==expected,(board_index,step)
  board=actual; cases+=1
checks.append('512-ragged-multisnake-steps-independent-oracle')
# All input/output modes, default game, and explicit failure contracts.
default=subprocess.run(['./snake'],capture_output=True,check=True,timeout=5).stdout.decode()
assert default.splitlines()[2]=='#  d>D   *         #'
p=Path('test-results/default.snk');p.write_text(default)
assert run(Path('tests/01-simple-in.snk').read_text(),['-o','test-results/pipe-file.snk'])==''
assert Path('test-results/pipe-file.snk').read_bytes()==Path('tests/01-simple-ref.snk').read_bytes()
assert subprocess.run(['./snake','-i','absent-file.snk'],capture_output=True).returncode==255
assert subprocess.run(['./snake','-o','absent-directory/out.snk'],capture_output=True).returncode==255
assert subprocess.run(['./snake','--stdin','-i','x'],capture_output=True).returncode==1
checks.append('default-pipe-file-modes-and-io-errors')
# Exercise actual original interactive program through a terminal, not a game
# helper substitute. Its starter has no working quit key; terminate externally.
def interactive(delay,keys):
 master,slave=pty.openpty()
 proc=subprocess.Popen(['./interactive-snake','-d',str(delay)],stdin=slave,stdout=slave,stderr=slave,close_fds=True)
 os.close(slave); data=b''
 def collect(timeout):
  nonlocal data
  end=time.monotonic()+timeout
  while time.monotonic()<end:
   if select.select([master],[],[],max(0,end-time.monotonic()))[0]:
    try: chunk=os.read(master,65536)
    except OSError: break
    if not chunk: break
    data+=chunk
 def frames():
  text=data.decode(errors='replace').replace('\r','')
  return [part.splitlines()[:18] for part in text.split('\x1b[2J\x1b[H')[1:] if len(part.splitlines())>=18]
 try:
  collect(.15); assert frames()
  if keys:
   for key in keys:
    os.write(master,key.encode()); collect(.08)
    assert frames()[-1][2][4]==key.upper(),(key,frames()[-1][2])
  else:
   collect(.5)
   assert any(frame[2][5]=='D' for frame in frames())
   assert any(frame[2][9]=='D' for frame in frames())
  assert proc.poll() is None
  return data
 finally:
  proc.terminate();proc.wait(timeout=5);os.close(master)
Path('test-results/interactive-directions.txt').write_bytes(interactive(5,'wasd'))
Path('test-results/interactive-movement.txt').write_bytes(interactive(.05,''))
checks.append('original-interactive-pty-four-directions-and-timed-food-movement')
result=dict(oracle_cases=cases,checks=checks,interactive_limit='Original starter has no effective quit handler; fixture terminates process. Not a Valgrind proof for starter interactive lifecycle.')
Path('test-results/extra.json').write_text(json.dumps(result,indent=2)+'\n')
print('PROJ1_EXTRA_OK cases=512 checks='+str(len(checks)))
PY
