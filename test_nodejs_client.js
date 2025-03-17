const net = require('net');

const request = { 
    path: "testreq",
    data: "hello from client!"
};

const buffer = Buffer.alloc(1024);
buffer.writeInt32LE(request.type, 0); // start 0 pos
buffer.write(request.data, 64); // start 64 pos

const client = new net.Socket();

client.connect(3426, '127.0.0.1', () => {
    console.log('connected!');

    client.write(buffer);
    console.log('sent request:', request);
});

client.on('data', (data) => {
    console.log('response from server:', 
    data.toString());
    client.destroy();
});

client.on('close', () => {
    console.log('connection closed');
});

client.on('error', (err) => {
    console.error('err:', err);
});
