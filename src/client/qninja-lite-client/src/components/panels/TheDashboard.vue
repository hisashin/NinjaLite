<template>
  <div class="panel panel--dashboard">
    <div class="panel__menu">
    </div>
    <section class="section section--dashboard-protocols">
      <div>
        <select v-model="protocolIndex" :disabled="!protocols || protocols.length==0">
            <option v-for="(item, index) in protocols" :key="index" :value="index">
              {{ item.b }}
            </option>
        </select>
        <b-button variant="primary" class="ml-1 btn-sm" :disabled="!protocols || protocols.length==0"
          @click.stop="selectProtocol">
          Use this protocol
        </b-button>
      </div>
      <div>
        <b-button variant="primary" class="ml-1 btn-sm"
          @click.stop="newProtocol">
          + New protocol
        </b-button>
      </div>
      <div>
        <ul>
            <li v-for="(item, index) in experiments" :key="index" :value="index">
              <a href="#" @click.stop="selectExperiment(item)">
                {{ new Date(item.t)}} 
              </a>
              (<a href="#" @click.stop="dl(item, 'csv')">
                CSV
              </a>
              /
              <a href="#" @click.stop="dl(item, 'json')">
                JSON
              </a>)

            </li>
        </ul>
      </div>

      <div>
        <b-button v-b-toggle.advanced_settings variant="link">Advanced settings</b-button>
        <b-collapse id="advanced_settings" class="mt-2">
          <h2>PID Tuning</h2>
          <textarea v-model="pid_config" cols="60" rows="4"></textarea>
          <br/>
          <!--
            :disabled="!connectionStatus.device.connected"
            -->
          <b-button class="mr-1 btn-sm"
            @click.stop="confPID">
            Set PID Constants
          </b-button>
          <b-button class="mr-1 btn-sm"
            @click.stop="validatePID">
            Validate
          </b-button>
          <h2>Calibration (ToDo)</h2>
        </b-collapse>
      </div>

    </section>
  </div>
</template>
<script>

import appState from "../../lib/AppState.js";
import device from "../../lib/Device.js";
import pidValidator from "../../lib/pid_validation.js";
/*
const DEFAULT_PID_CONF = {
  "c":[
    {"p":0.3, "i":0, "d":0, "x":55},
    {"p":0.3, "i":0, "d":0, "n":55, "x":80},
    {"p":0.3, "i":0, "d":0, "n":80}
  ]
}
  */

const DEFAULT_PID_CONF = {
  "c":[{"p":0.12,"i":0.009,"d":0.02}]
}
export default {
  name: 'TheDashboard',
  components: {
  },
  props: {
  },
  data() {
    return {
      connectionStatus: device.Connection.INITIAL,
      pid_config:JSON.stringify(DEFAULT_PID_CONF),
      deviceState:{},
      protocols:[],
      protocolIndex:0,
      experiments:[],
    }
  },
  created: function () {
    device.network.connectionStatus.observe((status)=>{
      this.connectionStatus = status;
      if (status.server.connected && this.protocols.length == 0) {
        console.log("presence filter=" + device.aws_command_topic_filter("presence"));
        device.loadProtocols((items)=>{
          this.protocols = items;
        }, ()=>{
          // onError
          appState.toast(this, "Error", "Failed to load protocols.");
        });
      }
      const params = new URLSearchParams(location.search);
      const urlTid = params.get("tid")
      const urlEid = params.get("eid")
      if (status.server.connected && this.experiments.length == 0) {
        device.loadExperiments((items)=>{
          this.experiments = items;

          if (urlTid && urlEid) {
            const experiment = this.experiments.find((e)=>{return e.eid==urlEid && e.tid==urlTid})
            if (experiment) {
              this.selectExperiment(experiment)
            } else {
              
              appState.toast(this, "Not found", "Experiment not found");
            }
          }
        }, ()=>{
          // onError
          appState.toast(this, "Error", "Failed to load protocols.");
        });
      }
    });
    device.deviceState.observe((state)=>{
      this.deviceState = state;
    });
  },
  methods: {
    onAppear () {
      if (this.connectionStatus.server.connected) {
        device.loadProtocols((items)=>{
          this.protocols = items;
        }, ()=>{
          // onError
          appState.toast(this, "Error", "Failed to load protocols.");
        });
      }
    },
    selectProtocol () {
      appState.pushPanel(appState.PANELS.PROTOCOL_EDITOR, {protocol:this.protocols[this.protocolIndex]});
    },
    newProtocol () {
      appState.pushPanel(appState.PANELS.PROTOCOL_EDITOR, null);
    },
    validatePID(){
      const errors = pidValidator.validate(JSON.parse(this.pid_config))
      console.log(errors)
      if (errors.length > 0) {
        appState.toast(this, "PID Config Error", JSON.stringify(errors));
      } else {
        appState.toast(this, "PID Config", "OK");
      }

    },
    selectExperiment(experiment) {
      console.log(JSON.stringify(experiment))
      appState.pushPanel(appState.PANELS.EXPERIMENT_DETAIL, experiment);
    },
    confPID () {
      console.log(this.pid_config);
      const pidObj = JSON.parse(this.pid_config);
      const errors = pidValidator.validate(JSON.parse(this.pid_config))
      if (errors.length > 0) {
        console.log("Error.")
        appState.toast(this, "PID Config Error", JSON.stringify(errors));
        return;
      }
      device.confPID(pidObj, ()=>{
        appState.toast(this, "PID Config", "PID constants have changed.");
      })
    },
    dl (experiment, fileType) {
      device.downloadExperimentLog(experiment, fileType);
    },
    title () { return "Dashboard" }
  }
}
</script>