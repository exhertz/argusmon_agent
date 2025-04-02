const net = require('net');

function createRequestBuffer(path, data = "") {
  const buffer = Buffer.alloc(1024);
  buffer.fill(0);
  buffer.write(path, 0) // start 0 pos
  buffer.write(data, 64); // start 64 pos
  return buffer;
}

const client = new net.Socket();

client.connect(3444, '127.0.0.1', () => {
    console.log('connected!');

    // getCpuModelReq = createRequestBuffer("getCpuModel");
    // client.write(getCpuModelReq);

    getCpuStatReq = createRequestBuffer("getCpuStat");
    client.write(getCpuStatReq);
});

client.on('data', (data) => {
    const status = data.readInt32LE(0);

    const responseData = data.slice(4).toString('utf-8').replace(/\0/g, '');

    console.log('response from server:');
    console.log('status:', status);
    console.log('data:', responseData);

    client.destroy();
});

client.on('close', () => {
    console.log('connection closed');
});

client.on('error', (err) => {
    console.error('err:', err);
});
