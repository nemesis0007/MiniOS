import assert from "node:assert/strict";
import test from "node:test";

async function render() {
  const workerUrl = new URL("../dist/server/index.js", import.meta.url);
  workerUrl.searchParams.set("test", `${process.pid}-${Date.now()}`);
  const { default: worker } = await import(workerUrl.href);
  return worker.fetch(new Request("http://localhost/", { headers: { accept: "text/html" } }), {
    ASSETS: { fetch: async () => new Response("Not found", { status: 404 }) },
  }, { waitUntil() {}, passThroughOnException() {} });
}

test("renders the complete MiniOS laboratory shell", async () => {
  const response = await render();
  assert.equal(response.status, 200);
  const html = await response.text();
  assert.match(html, /<title>MiniOS Lab<\/title>/i);
  assert.match(html, /Watch the CPU make/);
  assert.match(html, /MiniOS terminal/);
  assert.match(html, /Process table/);
  assert.match(html, /CPU timeline/);
  assert.doesNotMatch(html, /codex-preview|Your site is taking shape/);
});
