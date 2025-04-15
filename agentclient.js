const net = require('net');

class AgentClient {
  constructor(host = '127.0.0.1', port = 3444) {
    this.host = host;
    this.port = port;
    this.reqId = 0;
    this.requests = new Map();
    this.client = new net.Socket();
    this.setup();
  }

  setup() {
    this.client.on('data', (data) => {
      const id = data.readInt32LE(0);
      const status = data.readInt32LE(4);
      const respData = data.slice(8).toString('utf-8').replace(/\0/g, '');

      console.log("<- data: id:", id, " status: ", status, " data: ", respData);
      if (this.requests.has(id)) {
        const { resolve, reject } = this.requests.get(id);
        if (status === 0) {
          resolve(respData);
        } else {
          reject(new Error(`Server error: ${this.getErrorByCode(status)}`));
        }
        this.requests.delete(id);
      }
    });

    this.client.on('error', (err) => {
      if (err.code === 'ECONNREFUSED') {
        console.error("Agent is not running or server is unavailable");
      }

      this.requests.forEach(({ reject }) => reject(err));
      this.requests.clear();
    });
  }

  connect() {
    return new Promise((resolve, reject) => {
      this.client.connect(this.port, this.host, () => {
        resolve(this);
      });

      this.client.once('error', reject);
    });
  }

  sendRequest(path, data = "") {
    return new Promise((resolve, reject) => {
      const buffer = Buffer.alloc(1024);
      buffer.fill(0);
      const id = this.reqId++;
      buffer.writeInt32LE(id, 0);
      buffer.write(path, 4);
      buffer.write(data, 68);

      this.requests.set(id, { resolve, reject });
      this.client.write(buffer);
    });
  }

  getErrorByCode(code) {
    const errors = {
      0: "Success",
      1: "Bad request",
      2: "Path not found",
      3: "Access denied",
      4: "Data error"
    };
    return errors[code] || `Unknown error (${code})`;
  }

  close() {
    this.client.end();
  }
}

module.exports = AgentClient;
