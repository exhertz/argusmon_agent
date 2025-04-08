const net = require('net');

let reqid = 0;

function createRequestBuffer(path, data = "") {
  const buffer = Buffer.alloc(1024);
  buffer.fill(0);
  buffer.writeInt32LE(reqid, 0)
  buffer.write(path, 4) // 4 bytes int
  buffer.write(data, 68); // 64 bytes path + 4

  reqid++;
  return buffer;
}

const client = new net.Socket();

client.connect(3444, '127.0.0.1', () => {
    console.log('connected!');

    // getCpuModelReq = createRequestBuffer("getCpuModel");
    // client.write(getCpuModelReq);

    // getCpuStatReq = createRequestBuffer("getCpuStat");
    // client.write(getCpuStatReq);
    
    // getDiskStatReq = createRequestBuffer("getDiskStat");
    // client.write(getDiskStatReq);

    // getRamStatReq = createRequestBuffer("getRamStat");
    // client.write(getRamStatReq);

    getNetStatReq = createRequestBuffer("getNetStat");
    client.write(getNetStatReq);
});

client.on('data', (data) => {
    const status = data.readInt32LE(0);

    const respData = data.slice(4).toString('utf-8').replace(/\0/g, '');

    console.log('response from server:');
    console.log('status:', status);
    console.log('data:', respData);

    // const respFormatted = responseData.split(" ");
    // const cpuModel = respData;
    // const [cpuTotal, cpuFree] = respFormatted;
    // const [diskTotal, diskFree] = respFormatted;
    // const [ramTotal, ramUsage, ramAvailable, ramCached, ramFree] = respFormatted;
    // const [netRxDownload, netTxUpload] = respFormatted;

    client.destroy();
});

client.on('close', () => {
    console.log('connection closed');
});

client.on('error', (err) => {
    if (err.code === 'ECONNREFUSED') {
        console.error("Agent is not running or server is unavailable");
    }

    console.error('err:', err);
});
