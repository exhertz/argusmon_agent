const net = require('net');

const request = {
    path: "getCpuModel",
    data: "hello from client!"
};

const buffer = Buffer.alloc(1024);
buffer.write(request.path, 0); // start 0 pos
buffer.write(request.data, 64); // start 64 pos

const client = new net.Socket();

client.connect(3426, '127.0.0.1', () => {
    console.log('connected!');
    client.write(buffer);
    console.log('sent request:', request);
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
