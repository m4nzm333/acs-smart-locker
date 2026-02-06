
Publish to homeassistant/device/lockerA1/config
```json
{
  "device": {
    "identifiers": ["lockerA1"],
    "name": "Locker A1",
    "manufacturer": "Custom",
    "model": "Relay Door Lock"
  },
  "o": {
    "name": "foobar"
  },
  "cmps": {
    "locka1-main": {
      "p": "lock",
      "name": "Locker A1 Lock",
      "unique_id": "lockerA1-lock-main",
      "command_topic": "homeassistant/device/lockerA1/lock/set",
      "state_topic": "homeassistant/device/lockerA1/lock/state",
      "payload_lock": "LOCK",
      "payload_unlock": "UNLOCK",
      "state_locked": "LOCKED",
      "state_unlocked": "UNLOCKED",
      "optimistic": false,
      "qos": 1,
      "retain": true
    }
  }
}
```