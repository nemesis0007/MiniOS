"use client";

import { useEffect, useMemo, useRef, useState } from "react";
import type { V86 } from "v86";

type ProcessRow = {
  pid: number;
  name: string;
  state: string;
  priority: number;
  arrival: number;
  burst: number;
  remaining: number;
};

type TimelineEvent = { pid: number; start: number; end: number };
type Snapshot = {
  scheduler: string;
  quantum: number;
  processes: ProcessRow[];
  events: TimelineEvent[];
  stats: {
    waiting: number;
    turnaround: number;
    response: number;
    switches: number;
    utilization: number;
    throughput: number;
  };
};

const emptySnapshot: Snapshot = {
  scheduler: "FCFS",
  quantum: 2,
  processes: [],
  events: [],
  stats: { waiting: 0, turnaround: 0, response: 0, switches: 0, utilization: 0, throughput: 0 },
};

const schedulerCommands: Record<string, string> = {
  FCFS: "fcfs",
  SJF: "sjf",
  Priority: "priority",
  "Round Robin": "rr",
};

function parseMetric(value: number, suffix = "") {
  return `${(value / 100).toFixed(2)}${suffix}`;
}

export default function MiniOSDashboard() {
  const screenRef = useRef<HTMLDivElement>(null);
  const emulatorRef = useRef<V86 | null>(null);
  const [status, setStatus] = useState("Loading emulator");
  const [snapshot, setSnapshot] = useState<Snapshot>(emptySnapshot);
  const [quantum, setQuantum] = useState(2);

  useEffect(() => {
    let disposed = false;
    let serialLine = "";
    let pending: Snapshot | null = null;

    function consume(line: string) {
      if (!line.startsWith("@MINIOS|")) return;
      const fields = line.split("|");
      const kind = fields[1];

      if (kind === "BEGIN") {
        pending = structuredClone(emptySnapshot);
      } else if (kind === "SCHEDULER" && pending) {
        pending.scheduler = fields[2];
        pending.quantum = Number(fields[3]);
      } else if (kind === "PROCESS" && pending) {
        pending.processes.push({
          pid: Number(fields[2]), name: fields[3], state: fields[4],
          priority: Number(fields[5]), arrival: Number(fields[6]),
          burst: Number(fields[7]), remaining: Number(fields[8]),
        });
      } else if (kind === "EVENT" && pending) {
        pending.events.push({ pid: Number(fields[2]), start: Number(fields[3]), end: Number(fields[4]) });
      } else if (kind === "STATS" && pending) {
        pending.stats = {
          waiting: Number(fields[2]), turnaround: Number(fields[3]),
          response: Number(fields[4]), switches: Number(fields[5]),
          utilization: Number(fields[6]), throughput: Number(fields[7]),
        };
      } else if (kind === "END" && pending) {
        const complete = pending;
        pending = null;
        setSnapshot(complete);
        setQuantum(complete.quantum);
        setStatus("MiniOS online");
      }
    }

    async function boot() {
      const { V86: V86Runtime } = await import("v86");
      if (disposed || !screenRef.current) return;
      const emulator = new V86Runtime({
        wasm_path: "/v86/v86.wasm",
        memory_size: 32 * 1024 * 1024,
        vga_memory_size: 2 * 1024 * 1024,
        screen_container: screenRef.current,
        bios: { url: "/v86/seabios.bin" },
        vga_bios: { url: "/v86/vgabios.bin" },
        cdrom: { url: "/images/minios.iso" },
        autostart: true,
        disable_mouse: true,
        disable_speaker: true,
      });
      emulatorRef.current = emulator;
      emulator.add_listener("emulator-ready", () => setStatus("Booting MiniOS"));
      emulator.add_listener("serial0-output-byte", (byte) => {
        if (byte === 10) {
          consume(serialLine.replace(/\r/g, ""));
          serialLine = "";
        } else {
          serialLine += String.fromCharCode(byte);
        }
      });
    }

    boot().catch(() => setStatus("Emulator could not start"));
    return () => {
      disposed = true;
      const emulator = emulatorRef.current;
      emulatorRef.current = null;
      if (emulator) void emulator.destroy();
    };
  }, []);

  const sendCommand = (command: string) => {
    emulatorRef.current?.keyboard_send_text(`${command}\n`);
  };

  const totalTicks = useMemo(
    () => snapshot.events.reduce((maximum, event) => Math.max(maximum, event.end), 0),
    [snapshot.events],
  );

  return (
    <main className="app-shell">
      <header className="topbar">
        <div className="brand-lockup">
          <span className="brand-mark">M</span>
          <div><strong>MiniOS Lab</strong><span>Process scheduling, made visible</span></div>
        </div>
        <div className="live-status"><i />{status}</div>
      </header>

      <section className="hero-copy">
        <p className="eyebrow">A REAL 32-BIT KERNEL · RUNNING LOCALLY</p>
        <h1>Watch the CPU make<br />every scheduling decision.</h1>
        <p>Boot MiniOS, create workloads, and compare algorithms. Every result below comes directly from the operating system running inside the emulator.</p>
      </section>

      <section className="workspace">
        <article className="terminal-card">
          <div className="panel-heading">
            <div><span className="section-number">01</span><h2>MiniOS terminal</h2></div>
            <span className="hint">Click the screen to type</span>
          </div>
          <div className="monitor-frame">
            <div className="monitor-label"><span>VGA / 80 × 25</span><span>v0.19</span></div>
            <div className="emulator-screen" ref={screenRef} aria-label="MiniOS emulated display">
              <div className="text-screen" />
              <canvas className="vga-canvas" />
            </div>
          </div>
          <div className="quick-actions" aria-label="Quick commands">
            <button onClick={() => sendCommand("help")}>help</button>
            <button onClick={() => sendCommand("run short")}>+ short</button>
            <button onClick={() => sendCommand("run long")}>+ long</button>
            <button className="primary-action" onClick={() => sendCommand("start")}>Start scheduler</button>
          </div>
        </article>

        <aside className="control-column">
          <section className="control-panel">
            <div className="panel-heading"><div><span className="section-number">02</span><h2>Scheduler</h2></div></div>
            <label htmlFor="scheduler">Algorithm</label>
            <select
              id="scheduler"
              value={schedulerCommands[snapshot.scheduler] ?? "fcfs"}
              onChange={(event) => sendCommand(`scheduler ${event.target.value}`)}
            >
              <option value="fcfs">First Come First Serve</option>
              <option value="sjf">Shortest Job First</option>
              <option value="priority">Priority</option>
              <option value="rr">Round Robin</option>
            </select>
            <div className="quantum-row">
              <div><label htmlFor="quantum">Time quantum</label><small>Round Robin only</small></div>
              <div className="stepper">
                <button aria-label="Decrease quantum" onClick={() => setQuantum(Math.max(1, quantum - 1))}>−</button>
                <output>{quantum}</output>
                <button aria-label="Increase quantum" onClick={() => setQuantum(Math.min(100, quantum + 1))}>+</button>
              </div>
            </div>
            <button className="apply-button" onClick={() => sendCommand(`quantum ${quantum}`)}>Apply quantum</button>
          </section>

          <section className="stats-panel">
            <div className="panel-heading"><div><span className="section-number">03</span><h2>Live statistics</h2></div></div>
            <div className="stats-grid">
              <div><span>Avg. wait</span><strong>{parseMetric(snapshot.stats.waiting)}</strong><small>ticks</small></div>
              <div><span>Turnaround</span><strong>{parseMetric(snapshot.stats.turnaround)}</strong><small>ticks</small></div>
              <div><span>Response</span><strong>{parseMetric(snapshot.stats.response)}</strong><small>ticks</small></div>
              <div><span>CPU use</span><strong>{parseMetric(snapshot.stats.utilization, "%")}</strong><small>busy time</small></div>
            </div>
            <div className="stat-footer"><span>Context switches</span><strong>{snapshot.stats.switches}</strong></div>
          </section>
        </aside>
      </section>

      <section className="data-grid">
        <article className="data-panel process-panel">
          <div className="panel-heading"><div><span className="section-number">04</span><h2>Process table</h2></div><span className="count">{snapshot.processes.length} PCB{snapshot.processes.length === 1 ? "" : "s"}</span></div>
          <div className="table-wrap">
            <table>
              <thead><tr><th>PID</th><th>Program</th><th>State</th><th>Priority</th><th>Burst</th><th>Remaining</th></tr></thead>
              <tbody>
                {snapshot.processes.length === 0 ? (
                  <tr><td colSpan={6} className="empty-cell">No processes yet. Add a workload above.</td></tr>
                ) : snapshot.processes.map((process) => (
                  <tr key={process.pid}>
                    <td>P{process.pid}</td><td>{process.name}</td>
                    <td><span className={`state state-${process.state.toLowerCase()}`}>{process.state}</span></td>
                    <td>{process.priority}</td><td>{process.burst}</td><td>{process.remaining}</td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        </article>

        <article className="data-panel timeline-panel">
          <div className="panel-heading"><div><span className="section-number">05</span><h2>CPU timeline</h2></div><span className="count">{totalTicks} ticks</span></div>
          {snapshot.events.length === 0 ? (
            <div className="timeline-empty"><span>CPU</span><div /><p>Run queued processes to record ownership.</p></div>
          ) : (
            <div className="timeline-chart">
              <div className="timeline-track">
                {snapshot.events.map((event, index) => (
                  <div
                    className={`timeline-segment pid-${event.pid % 5}`}
                    style={{ flexGrow: event.end - event.start }}
                    title={`P${event.pid}: ticks ${event.start}–${event.end}`}
                    key={`${event.pid}-${event.start}-${index}`}
                  >P{event.pid}</div>
                ))}
              </div>
              <div className="timeline-scale"><span>0</span><span>{totalTicks} ticks</span></div>
              <div className="timeline-legend">
                {[...new Set(snapshot.events.map((event) => event.pid))].map((pid) => <span key={pid}><i className={`pid-${pid % 5}`} />P{pid}</span>)}
              </div>
            </div>
          )}
        </article>
      </section>

      <footer><span>MiniOS · x86 · GRUB · PIT 100 Hz</span><span>The browser observes. The kernel decides.</span></footer>
    </main>
  );
}
