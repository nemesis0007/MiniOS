import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";
import test from "node:test";
import { V86 } from "v86";

function exactArrayBuffer(buffer) {
  return buffer.buffer.slice(buffer.byteOffset, buffer.byteOffset + buffer.byteLength);
}

test("v86 boots the packaged MiniOS ISO and receives kernel telemetry", { timeout: 30000 }, async () => {
  const [wasm, bios, vgaBios, iso] = await Promise.all([
    readFile(new URL("../public/v86/v86.wasm", import.meta.url)),
    readFile(new URL("../public/v86/seabios.bin", import.meta.url)),
    readFile(new URL("../public/v86/vgabios.bin", import.meta.url)),
    readFile(new URL("../public/images/minios.iso", import.meta.url)),
  ]);

  const wasmBuffer = exactArrayBuffer(wasm);
  const emulator = new V86({
    wasm_fn: async (imports) => (await WebAssembly.instantiate(wasmBuffer, imports)).instance.exports,
    memory_size: 32 * 1024 * 1024,
    vga_memory_size: 2 * 1024 * 1024,
    bios: { buffer: exactArrayBuffer(bios) },
    vga_bios: { buffer: exactArrayBuffer(vgaBios) },
    cdrom: { buffer: exactArrayBuffer(iso) },
    autostart: true,
    disable_mouse: true,
    disable_speaker: true,
  });

  let serial = "";
  await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error("MiniOS telemetry timed out")), 25000);
    emulator.add_listener("serial0-output-byte", (byte) => {
      serial += String.fromCharCode(byte);
      if (serial.includes("@MINIOS|END")) {
        clearTimeout(timer);
        resolve();
      }
    });
  });

  assert.match(serial, /@MINIOS\|SCHEDULER\|FCFS\|2/);
  assert.match(serial, /@MINIOS\|STATS\|0\|0\|0\|0\|0\|0/);
  await emulator.destroy();
});
