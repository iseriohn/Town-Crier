const grpc = require('grpc');

const protoPath = require('path').join(__dirname, '../src/App/services');
const proto = grpc.load({root: protoPath, file: 'tc.proto' });

//Create a new client instance that binds to the IP and port of the grpc server.
const client = new proto.rpc.towncrier('localhost:12345', grpc.credentials.createInsecure());

const data = "Hello World";

client.participate(data, (error, response) => {
  if (!error) {
  	console.log("Succeed!");
  } else {
    console.log("Error:", error.message);
  }
});
