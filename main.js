const AgentClient = require('./agentclient.js');

async function main() {
  const client = new AgentClient();

  try {
    await client.connect();

    const cpuModel = await client.sendRequest("getCpuModel");
    console.log("cpu model:", cpuModel);

    const cpuStats = await client.sendRequest("getCpuStat");
    console.log("cpu stats:", cpuStats);

    const diskStat = await client.sendRequest("getDiskStat");
    console.log("disk stats:", diskStat);

    const ramStat = await client.sendRequest("getRamStat");
    console.log("ram stats:", ramStat);
    const [ramTotal, ramUsage, ramAvailable, ramCached, ramFree] = ramStat.split(" ");
    console.log("ram total: ", ramTotal, ", ram cached: ", ramCached);

    const netStat = await client.sendRequest("getNetStat");
    console.log("net stats:", netStat);

    const errReq = await client.sendRequest("notfound");
    console.log(errReq);
  } catch (err) {
    console.error("Error:", err);
  } finally {
    client.close();
  }
}

main();
