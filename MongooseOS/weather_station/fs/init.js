load("api_config.js");
load("api_rpc.js");
load("api_dht.js");
load("api_timer.js");
load("api_mqtt.js");

let pin = Cfg.get("app.pin");
let dht = DHT.create(pin, DHT.DHT22);

Timer.set(
  1000 * 60 * 30,
  true,
  function() {
    let data = JSON.stringify({
      temperature: dht.getTemp(),
      humidity: dht.getHumidity()
    });

    MQTT.pub("home/temperature", data);
  },
  null
);
