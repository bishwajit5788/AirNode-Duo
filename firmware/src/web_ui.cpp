#include "web_ui.h"

const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<meta name="apple-mobile-web-app-capable" content="yes">
<title>AirNode Duo</title>
<style>
:root{color-scheme:dark;--bg:#0d1110;--card:#161c19;--border:#2a3330;--text:#e8efe9;--muted:#8a9a90;--accent:#5ecf8a;--stop:#e85d5d;--cool:#5eb0e8;--warn:#e8c35e}
*{box-sizing:border-box}
body{margin:0;font-family:system-ui,-apple-system,Segoe UI,Roboto,sans-serif;background:var(--bg);color:var(--text);-webkit-tap-highlight-color:transparent}
main{max-width:420px;margin:0 auto;padding:16px 14px 40px}
header{text-align:center;padding:8px 0 4px}
h1{margin:0;font-size:1.55rem;letter-spacing:.02em}
.sub{color:var(--muted);font-size:.85rem;margin-top:2px}
.conn{display:inline-flex;align-items:center;gap:6px;margin-top:10px;font-size:.9rem;font-weight:600}
.dot{width:10px;height:10px;border-radius:50%;background:var(--accent);box-shadow:0 0 8px var(--accent)}
.dot.off{background:var(--stop);box-shadow:0 0 8px var(--stop)}
.dot.warn{background:var(--warn);box-shadow:0 0 8px var(--warn)}
.ip{color:var(--muted);font-size:.8rem;margin-top:2px}
.card{background:var(--card);border:1px solid var(--border);border-radius:14px;padding:14px 16px;margin:12px 0}
.card h2{margin:0 0 4px;font-size:.75rem;text-transform:uppercase;letter-spacing:.08em;color:var(--muted);font-weight:600}
.motor-name{font-size:.95rem;font-weight:700;margin-bottom:2px}
.pct{font-size:1.6rem;font-weight:700;font-variant-numeric:tabular-nums;text-align:center;margin:6px 0 2px}
.applied{text-align:center;font-size:.75rem;color:var(--muted);margin-bottom:6px}
input[type=range]{-webkit-appearance:none;width:100%;height:36px;background:transparent;margin:4px 0}
input[type=range]::-webkit-slider-runnable-track{height:8px;border-radius:4px;background:#2a3530}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:28px;height:28px;border-radius:50%;background:var(--accent);margin-top:-10px;border:2px solid #0d1110;box-shadow:0 2px 6px rgba(0,0,0,.4)}
input[type=range]::-moz-range-track{height:8px;border-radius:4px;background:#2a3530}
input[type=range]::-moz-range-thumb{width:28px;height:28px;border-radius:50%;background:var(--accent);border:2px solid #0d1110}
.btn-row{display:flex;gap:10px;margin-top:8px}
button{flex:1;border:0;border-radius:12px;padding:16px 12px;font-size:1rem;font-weight:700;cursor:pointer;touch-action:manipulation}
.btn-start{background:var(--accent);color:#071009}
.btn-stop{background:var(--stop);color:#1b0505}
.btn-cool{background:var(--cool);color:#07111b}
.btn-off{background:#3a4240;color:#ccc}
.btn-start:disabled{opacity:.4}
.row{display:flex;justify-content:space-between;align-items:center;margin:6px 0;font-size:.95rem}
.badge{padding:3px 10px;border-radius:8px;font-size:.8rem;font-weight:700}
.badge-on{background:#1a3a28;color:var(--accent)}
.badge-off{background:#2a2e2c;color:var(--muted)}
.badge-prime{background:#3a3020;color:var(--warn)}
.sys-line{display:flex;justify-content:space-between;padding:4px 0;font-size:.9rem}
.sys-val{font-weight:700;font-variant-numeric:tabular-nums}
.fail{color:var(--stop)!important}
.ok{color:var(--accent)}
</style>
</head>
<body>
<main>
<header>
  <h1>AirNode Duo</h1>
  <div class="sub">Personal Airflow &amp; Cooling Controller</div>
  <div class="conn"><span class="dot" id="dot"></span><span id="connText">Connecting…</span></div>
  <div class="ip" id="ipText">192.168.4.1</div>
</header>

<div class="card">
  <h2>Motor 1</h2>
  <div class="motor-name">2312 920KV CW</div>
  <div class="pct" id="v1">0%</div>
  <div class="applied" id="a1">Applied: 0%</div>
  <input id="m1" type="range" min="0" max="100" value="0">
</div>

<div class="card">
  <h2>Motor 2</h2>
  <div class="motor-name">2312 920KV CW</div>
  <div class="pct" id="v2">0%</div>
  <div class="applied" id="a2">Applied: 0%</div>
  <input id="m2" type="range" min="0" max="100" value="0">
</div>

<div class="card">
  <h2>Master Speed</h2>
  <div class="pct" id="vm">0%</div>
  <input id="master" type="range" min="0" max="100" value="0">
  <div class="sub" style="text-align:center;margin-top:4px">Sets both motors; adjust individually after</div>
</div>

<div class="card">
  <div class="btn-row">
    <button class="btn-start" id="btnStart" onclick="doStart()">START</button>
    <button class="btn-stop" onclick="doStop()">STOP</button>
  </div>
</div>

<div class="card">
  <h2>Thermoelectric Cooling</h2>
  <div class="row"><span>Cooling</span><span class="badge badge-off" id="coolBadge">OFF</span></div>
  <div class="row"><span>Pump</span><span class="badge badge-off" id="pumpBadge">OFF</span></div>
  <div class="row"><span>TEC</span><span class="badge badge-off" id="tecBadge">OFF</span></div>
  <div class="btn-row" style="margin-top:10px">
    <button class="btn-cool" onclick="setCooling(1)">COOLING ON</button>
    <button class="btn-off" onclick="setCooling(0)">OFF</button>
  </div>
  <div class="sub" style="margin-top:8px">Pump primes 2 s before TEC. Requires Motor 1 ≥ 20%.</div>
</div>

<div class="card">
  <h2>System Status</h2>
  <div class="sys-line"><span>State</span><span class="sys-val" id="sysState">—</span></div>
  <div class="sys-line"><span>Armed</span><span class="sys-val" id="sysArmed">—</span></div>
  <div class="sys-line"><span>Failsafe</span><span class="sys-val" id="sysFs">—</span></div>
  <div class="sys-line"><span>Clients</span><span class="sys-val" id="sysClients">—</span></div>
</div>
</main>
<script>
const $=id=>document.getElementById(id);
let lastSend=0, connected=false, wasFailsafe=false;

async function api(url, opts={}) {
  try {
    const r = await fetch(url, {cache:'no-store', ...opts});
    return r;
  } catch(e) { return null; }
}

function labels() {
  $('v1').textContent = $('m1').value + '%';
  $('v2').textContent = $('m2').value + '%';
  $('vm').textContent = $('master').value + '%';
}

async function sendControl() {
  labels();
  if (Date.now() - lastSend < 80) return;
  lastSend = Date.now();
  await api('/api/control', {
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:`m1=${$('m1').value}&m2=${$('m2').value}`
  });
}

$('m1').addEventListener('input', () => { sendControl(); });
$('m2').addEventListener('input', () => { sendControl(); });
$('master').addEventListener('input', () => {
  const v = $('master').value;
  $('m1').value = v;
  $('m2').value = v;
  sendControl();
});

async function doStart() {
  await api('/api/start', {method:'POST'});
  refresh();
}
async function doStop() {
  await api('/api/stop', {method:'POST'});
  $('m1').value = 0;
  $('m2').value = 0;
  $('master').value = 0;
  labels();
  refresh();
}
async function setCooling(on) {
  await api('/api/cooling', {
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'on=' + on
  });
  refresh();
}

function setBadge(el, on, textOn, textOff, priming) {
  if (priming) {
    el.className = 'badge badge-prime';
    el.textContent = 'PRIMING…';
  } else if (on) {
    el.className = 'badge badge-on';
    el.textContent = textOn || 'ON';
  } else {
    el.className = 'badge badge-off';
    el.textContent = textOff || 'OFF';
  }
}

async function refresh() {
  const r = await api('/api/status');
  if (!r) {
    connected = false;
    $('dot').className = 'dot off';
    $('connText').textContent = 'DISCONNECTED';
    return;
  }
  let s;
  try { s = await r.json(); } catch(e) { return; }
  connected = true;

  if (s.failsafe) {
    $('dot').className = 'dot warn';
    $('connText').textContent = wasFailsafe ? 'FAILSAFE' : 'FAILSAFE';
    wasFailsafe = true;
  } else {
    $('dot').className = 'dot';
    $('connText').textContent = wasFailsafe ? 'CONNECTION RESTORED' : 'CONNECTED';
    if (!s.running) wasFailsafe = false;
  }

  if (s.ip) $('ipText').textContent = s.ip;

  $('a1').textContent = 'Applied: ' + s.motor1_applied + '%';
  $('a2').textContent = 'Applied: ' + s.motor2_applied + '%';

  // Don't fight user while dragging; only sync when not focused
  if (document.activeElement !== $('m1')) $('m1').value = s.motor1_target;
  if (document.activeElement !== $('m2')) $('m2').value = s.motor2_target;
  labels();

  const priming = s.cooling_requested && s.pump && !s.tec;
  setBadge($('coolBadge'), s.cooling_requested && s.tec, 'ACTIVE', 'OFF', priming);
  setBadge($('pumpBadge'), s.pump, 'ON', 'OFF', false);
  setBadge($('tecBadge'), s.tec, 'ON', 'OFF', false);

  let state = 'STOPPED';
  if (s.failsafe) state = 'FAILSAFE';
  else if (s.running) state = 'RUNNING';
  else if (s.armed) state = 'ARMED';
  else state = 'ARMING…';
  $('sysState').textContent = state;
  $('sysState').className = 'sys-val' + (s.failsafe ? ' fail' : s.running ? ' ok' : '');
  $('sysArmed').textContent = s.armed ? 'YES' : 'NO';
  $('sysFs').textContent = s.failsafe ? 'TRIGGERED' : 'OK';
  $('sysFs').className = 'sys-val' + (s.failsafe ? ' fail' : ' ok');
  $('sysClients').textContent = s.clients;
  $('btnStart').disabled = !s.armed || s.running;
}

setInterval(async () => {
  await api('/api/heartbeat', {method:'POST'});
  refresh();
}, 500);
setInterval(refresh, 1500);
refresh();
labels();
</script>
</body>
</html>
)HTML";
