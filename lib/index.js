const addon = require("bindings")("simple_pulse")
const { Readable } = require('stream');


class simple_pulse{
  constructor(){
    this.pulse=new addon.pulse()
    this.stream=null;
  }
  createRecordStream(opts){
    this.stream=new Readable({read: () => {}})
    // format = S16LE
    // channels=1
    this.pulse.record((buffer)=>{this.stream.push(buffer)},opts.rate,opts.device)
    return this.stream;
  }
  stopRecord(){
    this.pulse.stop()
  }
}

module.exports = simple_pulse;
