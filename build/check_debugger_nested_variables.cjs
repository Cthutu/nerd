// Exercise the adapter's public DAP path without an extension host.
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');
const { createRequire } = require('node:module');
const { PassThrough } = require('node:stream');

function loadAdapter(spawn) {
    const filename = path.resolve(__dirname, '../syntax/nerd-vscode/out/extension.js');
    const localRequire = createRequire(filename);
    class EventEmitter {
        listeners = [];
        event = listener => { this.listeners.push(listener); return { dispose() {} }; };
        fire(message) { for (const listener of this.listeners) listener(message); }
        dispose() {}
    }
    const context = {
        module: { exports: {} }, exports: {}, Buffer, process, console,
        setTimeout, clearTimeout, __dirname: path.dirname(filename), __filename: filename,
        require(name) {
            if (name === 'vscode') return { EventEmitter, workspace: { workspaceFolders: [] } };
            if (name === 'vscode-languageclient/node') return {};
            if (name === 'child_process') return { ...require('node:child_process'), spawn };
            return localRequire(name);
        },
    };
    vm.runInNewContext(fs.readFileSync(filename, 'utf8') +
        '\nmodule.exports.Adapter = NerdCodeLldbDebugAdapter;', context, { filename });
    return context.module.exports.Adapter;
}

function frame(message) {
    const data = JSON.stringify(message);
    return `Content-Length: ${Buffer.byteLength(data)}\r\n\r\n${data}`;
}

function run() {
    const child = {
        stdin: new PassThrough(), stdout: new PassThrough(), stderr: new PassThrough(),
        on() {}, kill() {},
    };
    const Adapter = loadAdapter(() => child);
    const adapter = new Adapter('recorded-codelldb');
    const emitted = [], sent = [];
    adapter.onDidSendMessage(message => emitted.push(message));
    child.stdin.on('data', data => {
        const text = data.toString();
        sent.push(JSON.parse(text.slice(text.indexOf('\r\n\r\n') + 4)));
    });
    adapter.dynamicArrayDeclarations.add('locations');
    adapter.dynamicArrayDeclarations.add('mobs');
    function request(seq, command, args) {
        adapter.handleMessage({ type: 'request', seq, command, arguments: args });
    }
    function response(seq, command, body) {
        child.stdout.write(frame({ type: 'response', request_seq: seq, command, success: true, body }));
    }
    function result(seq) {
        const message = emitted.find(m => m.request_seq === seq);
        assert.ok(message, `Missing response ${seq}`);
        return message.body;
    }
    function evaluatePending() {
        while (sent.length) {
            const message = sent.shift();
            if (message.command !== 'evaluate') continue;
            assert.equal(message.arguments.frameId, 17);
            const expression = message.arguments.expression;
            let body;
            if (expression.includes('unsigned long long')) {
                assert.match(expression, /\? .* - (24|16)\)\) : 0$/);
                const count = expression.includes('- 24)');
                body = { result: expression.includes('.mobs') ? (count ? '7' : '16') : '1', variablesReference: 0 };
            } else if (expression.endsWith('.tag')) {
                body = { result: '0', variablesReference: 0 };
            } else if (expression.includes('encounter.payload')) {
                body = { result: 'Location', type: 'Location', variablesReference: 400 };
            } else if (expression.startsWith('(void*)')) {
                body = { result: '0x12340000', variablesReference: 0 };
            } else if (expression === 'world.locations[0]') {
                body = { result: 'Location', type: 'Location', variablesReference: 200 };
            } else {
                const index = expression.match(/\.mobs\[(\d+)\]$/);
                assert.ok(index, expression);
                body = { result: String(Number(index[1]) + 1), type: 'int', variablesReference: 0 };
            }
            response(message.seq, 'evaluate', body);
        }
    }
    request(1, 'scopes', { frameId: 17 });
    response(1, 'scopes', { scopes: [{ name: 'Local', variablesReference: 100 }] });
    request(2, 'variables', { variablesReference: 100 });
    response(2, 'variables', { variables: [
        { name: 'world', evaluateName: 'world', type: 'World', value: 'World', variablesReference: 101 },
    ] });
    request(3, 'variables', { variablesReference: 101 });
    response(3, 'variables', { variables: [
        { name: 'locations', evaluateName: 'world.locations', type: 'Location *', value: 'Location', variablesReference: 102 },
    ] });
    evaluatePending();
    const locations = result(3).variables[0];
    assert.equal(locations.type, '[..]Location');
    assert.equal(locations.value, '[..]Location (1/1)');
    request(4, 'variables', { variablesReference: locations.variablesReference });
    evaluatePending();
    const location = result(4).variables.find(v => v.name === '[0]');
    assert.equal(location.variablesReference, 200);
    request(5, 'variables', { variablesReference: location.variablesReference });
    response(5, 'variables', { variables: [
        { name: 'mobs', evaluateName: 'world.locations[0].mobs', type: 'int *', value: '1', variablesReference: 201 },
    ] });
    evaluatePending();
    const mobs = result(5).variables[0];
    assert.equal(mobs.type, '[..]i32');
    assert.equal(mobs.value, '[..]i32 (7/16)');
    request(6, 'variables', { variablesReference: mobs.variablesReference });
    evaluatePending();
    assert.deepEqual(Array.from(result(6).variables.filter(v => /^\[\d+\]$/.test(v.name)), v => v.value),
        ['1', '2', '3', '4', '5', '6', '7']);

    // Aggregate watch values must pass their frame on to expandable fields too.
    request(7, 'evaluate', { expression: 'world.locations[0]', frameId: 17, context: 'watch' });
    response(7, 'evaluate', { result: 'Location', type: 'Location', variablesReference: 300 });
    request(8, 'variables', { variablesReference: 300 });
    response(8, 'variables', { variables: [
        { name: 'mobs', evaluateName: 'world.locations[0].mobs', type: 'int *', value: '1', variablesReference: 301 },
    ] });
    evaluatePending();
    assert.equal(result(8).variables[0].value, '[..]i32 (7/16)');

    // A record payload reached through a synthetic enum expansion inherits it.
    adapter.enumDeclarations.set('Encounter', {
        name: 'Encounter', variants: [{ name: 'Found', discriminant: 0, payloadType: 'Location' }],
    });
    request(9, 'variables', { variablesReference: 100 });
    response(9, 'variables', { variables: [
        { name: 'encounter', evaluateName: 'encounter', type: 'Encounter', value: 'Encounter', variablesReference: 401 },
    ] });
    evaluatePending();
    const encounter = result(9).variables[0];
    request(10, 'variables', { variablesReference: encounter.variablesReference });
    evaluatePending();
    const payload = result(10).variables.find(v => v.name === 'payload');
    assert.equal(payload.variablesReference, 400);
    request(11, 'variables', { variablesReference: payload.variablesReference });
    response(11, 'variables', { variables: [
        { name: 'mobs', evaluateName: 'location.mobs', type: 'int *', value: '1', variablesReference: 402 },
    ] });
    evaluatePending();
    assert.equal(result(11).variables[0].value, '[..]i32 (7/16)');
    request(12, 'evaluate', { expression: 'world.mobs.count', frameId: 17, context: 'watch' });
    evaluatePending();
    assert.equal(result(12).result, '7');
    request(13, 'evaluate', { expression: 'world.mobs.capacity', frameId: 17, context: 'watch' });
    evaluatePending();
    assert.equal(result(13).result, '16');
    adapter.dispose();
    console.log('debugger nested variable expansion passed');
}

module.exports = { loadAdapter };
if (require.main === module) run();
